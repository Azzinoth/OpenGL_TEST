#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <windowsx.h>

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>
//#include "gl/GLU.h"
//#include "GLES\GLES2\gl2.h"

#include "MasterRenderer.h"
#include "Loader.h"
#include "Player.h"
#include "FreeCamera.h"
#include "ModelViewCamera.h"
#include "GuiRenderer.h"
#include "MousePicker.h"
#include "WaterRenderer.h"
#include "WaterFrameBuffers.h"

Loader* loader;
EntityRenderer* renderer;

WaterShader* waterShader;
WaterRenderer* waterRenderer;
WaterFrameBuffers* waterFrameBuffers;

//ModelViewCamera* camera;
FreeCamera* camera;
Player* player;

#pragma comment (lib, "opengl32.lib")


static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("123");
HINSTANCE hInst;

HWND main_hwnd;
HDC g_hDC;

#define WIN_W 1920
#define WIN_H 1080

#define RES_FOLDER "C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/resources/"

inline int getTerrainIndexOutOfWorldXZ(int worldX, int WorldZ, std::vector<Terrain*>& terrains, int halfSizeOfWorld) {
	float terX = worldX / terrains[0]->getSizeOfSide();
	float terZ = WorldZ / terrains[0]->getSizeOfSide();

	float fracX = terX - (int)terX;
	float fracZ = terZ - (int)terZ;

	if (fracX != 0) {
		if (terX > 0) {
			terX = terX - fracX;
		}
		else {
			terX = terX - (1 + fracX);
		}
	}

	if (fracZ != 0) {
		if (terZ > 0) {
			terZ = terZ - fracZ;
		}
		else {
			terZ = terZ - (1 + fracZ);
		}
	}

	return (halfSizeOfWorld + terX) * (halfSizeOfWorld * 2) + (halfSizeOfWorld + terZ);
}

inline float getTerrainY(float worldX, float worldZ, std::vector<Terrain*>& terrains, int HALF_SIZE_OF_WORLD) {
	return terrains[getTerrainIndexOutOfWorldXZ(worldX, worldZ, terrains, HALF_SIZE_OF_WORLD)]->getHeightOfTerrain(worldX, worldZ);
}


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Win32 Guided Tour"), NULL);

        return 1;
    }

    hInst = hInstance;


	main_hwnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WIN_W, WIN_H,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!main_hwnd)
    {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Win32 Guided Tour"), NULL);
        return 1;
    }
    else
    {
        RECT rc;
        GetClientRect(main_hwnd, &rc);

        //PIXELFORMATDESCRIPTOR pfd =
        //{
        //    sizeof(PIXELFORMATDESCRIPTOR),
        //    1,
        //    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        //    PFD_TYPE_RGBA,             //The kind of framebuffer. RGBA or palette.
        //    32,                        //Colordepth of the framebuffer.
        //    0, 0, 0, 0, 0, 0,
        //    0,
        //    0,
        //    0,
        //    0, 0, 0, 0,
        //    24,                        //Number of bits for the depthbuffer
        //    8,                         //Number of bits for the stencilbuffer
        //    0,                         //Number of Aux buffers in the framebuffer.
        //    PFD_MAIN_PLANE,
        //    0,
        //    0, 0, 0
        //};

        //HDC windowDC = GetDC(main_hwnd);
        //int closePixelFormat = ChoosePixelFormat(windowDC, &pfd);
        //SetPixelFormat(windowDC, closePixelFormat, &pfd);

        //HGLRC openGLContext = wglCreateContext(windowDC);
        //bool result = wglMakeCurrent(windowDC, openGLContext);

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 24;

		g_hDC = GetDC(main_hwnd);
		GLuint iPixelFormat = ChoosePixelFormat(g_hDC, &pfd);

		if (iPixelFormat != 0)
		{
			PIXELFORMATDESCRIPTOR bestMatch_pfd;
			DescribePixelFormat(g_hDC, iPixelFormat, sizeof(pfd), &bestMatch_pfd);

			if (bestMatch_pfd.cDepthBits < pfd.cDepthBits)
			{
				return 0;
			}

			if (SetPixelFormat(g_hDC, iPixelFormat, &pfd) == FALSE)
			{
				DWORD dwErrorCode = GetLastError();
				return 0;
			}
		}
		else
		{
			DWORD dwErrorCode = GetLastError();
			return 0;
		}

		HGLRC g_hRC = wglCreateContext(g_hDC);
		wglMakeCurrent(g_hDC, g_hRC);


        //MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
        
        // I hate it :)
        //glewExperimental = GL_TRUE;
        glewInit();

  //      GLuint framebuffer;
		//GL_ERROR(glGenFramebuffers(1, &framebuffer));
		//GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
		////GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  //      GLuint colorRenderbuffer;
		//GL_ERROR(glGenRenderbuffers(1, &colorRenderbuffer));
		//GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer));
		//GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, WIN_W, WIN_H));
		//GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer));

  //      GLuint depthRenderbuffer;
		//GL_ERROR(glGenRenderbuffers(1, &depthRenderbuffer));
		//GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer));
		//GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, WIN_W, WIN_H));
		//GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer));

  //      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  //      if (status != GL_FRAMEBUFFER_COMPLETE) 
		//{
  //          MessageBoxA(0, "failed to make complete framebuffer object", "OPENGL VERSION", 0);
  //      }
    }

    ShowWindow(main_hwnd, nCmdShow);
    UpdateWindow(main_hwnd);

    // Main message loop:
    MSG msg;

    ZeroMemory(&msg, sizeof(MSG));
	ShowCursor(true);


	loader = new Loader();

	// Models with sub meshes
	//
	//

	//std::vector<RawModel*> modelVector;
	//loader->loadFromOBJ("C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/tree.obj", modelVector);
	//ModelTexture* texture = new ModelTexture(loader->loadTexture("C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/dragon.png"));

	//std::vector<TexturedModel*> texturedModelVector;
	//for (auto model : modelVector) {
	//	texturedModelVector.push_back(new TexturedModel(model, texture));
	//}

	//typedef std::vector<Entity*> tree;

	//std::vector<tree> trees;
	//tree entityVector;
	//for (int i = 0; i < 50; i++)
	//{
	//	//randomize();
	//	float modelX = rand() % 400 - 200;
	//	float modelZ = rand() % 400 - 200;
	//	for (auto texturedModel : texturedModelVector) {
	//		entityVector.push_back(new Entity(texturedModel, glm::vec3(modelX, -11.5f, modelZ), glm::vec3(0.0f, 0.0f, 0.0f), 10.0f));
	//	}

	//	trees.push_back(entityVector);
	//}

	// Models with sub meshes
	//
	//



	//DWORD LastTickCount = GetTickCount();
	
	
	//DWORD CurrentTickCount = GetTickCount();
	//volatile int time = CurrentTickCount - LastTickCount;

	glm::vec3 entityPosition;

	// ************************ TERRAINS ************************
	const int SIZE_OF_WORLD = 4;
	const int HALF_SIZE_OF_WORLD = SIZE_OF_WORLD / 2;
	std::vector<Terrain*> terrains;
	terrains.reserve(SIZE_OF_WORLD * SIZE_OF_WORLD);

	TerrainTexture* backgroundTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "grassy.png"));
	TerrainTexture* rTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "mud.png"));
	TerrainTexture* gTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "grassFlowers.png"));
	TerrainTexture* bTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "path.png"));

	TerrainTexturePack* texturePack = new TerrainTexturePack(*backgroundTexture, *rTexture, *gTexture, *bTexture);
	TerrainTexture* blendMap = new TerrainTexture(loader->loadTexture(RES_FOLDER "blendMap.png"));
	std::string heightmap = RES_FOLDER "heightmap.png";

	for (int i = -HALF_SIZE_OF_WORLD; i < HALF_SIZE_OF_WORLD; i++) {
		for (int j = -HALF_SIZE_OF_WORLD; j < HALF_SIZE_OF_WORLD; j++) {
			terrains.push_back(new Terrain(i, j, *loader, *texturePack, *blendMap, heightmap));
		}
	}
	// ************************ TERRAINS ************************

	// ************************ TREES ************************

	RawModel* treeModel = loader->loadFromOBJ(RES_FOLDER "lowPolyTree.obj");
	ModelTexture* treeTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "lowPolyTree.png"));
	treeTexture->setShineDamper(10);
	treeTexture->setReflectivity(0);

	TexturedModel* treeTexturedModel = new TexturedModel(treeModel, treeTexture);
	
	srand(GetTickCount());

	const int MAX_RANDOM = 4500;
	const int NUMBER_OF_EACH_ENTITY = 500;

	std::vector<Entity*> trees;
	for (int i = 0; i < NUMBER_OF_EACH_ENTITY; i++)
	{
		float modelX = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;
		float modelZ = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;

		float noizeX = rand() % 100 - 50;
		float noizeZ = rand() % 100 - 50;

		modelX += noizeX;
		modelZ += noizeZ;

		entityPosition = { modelX, getTerrainY(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD) - 4.0f, modelZ };
		trees.push_back(new Entity(*loader, treeTexturedModel, entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ TREES ************************

	// ************************ FERNS ************************
	RawModel* fernModel = loader->loadFromOBJ(RES_FOLDER "fern.obj");
	ModelTexture* fernTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "fernAtlas.png"));
	fernTexture->setShineDamper(10);
	fernTexture->setReflectivity(0);
	fernTexture->setHasTransparency(true);
	fernTexture->setNumberOfRows(2);

	TexturedModel* fernTexturedModel = new TexturedModel(fernModel, fernTexture);

	srand(GetTickCount() + 10000);

	std::vector<Entity*> ferns;
	for (int i = 0; i < NUMBER_OF_EACH_ENTITY + 1000; i++)
	{
		float modelX = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;
		float modelZ = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;

		float noizeX = rand() % 100 - 50;
		float noizeZ = rand() % 100 - 50;

		modelX += noizeX;
		modelZ += noizeZ;

		entityPosition = { modelX, getTerrainY(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD) - 2.0f, modelZ };
		ferns.push_back(new Entity(*loader, fernTexturedModel, rand() % 4 , entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ FERNS ************************

	// ************************ GRASSES ************************
	RawModel* grassModel = loader->loadFromOBJ(RES_FOLDER "grass.obj");
	ModelTexture* grassTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "verticalGrass.png"));
	grassTexture->setShineDamper(10);
	grassTexture->setReflectivity(0);
	grassTexture->setHasTransparency(true);
	grassTexture->setUseFakeLighting(true);

	TexturedModel* grassTexturedModel = new TexturedModel(grassModel, grassTexture);

	srand(GetTickCount() + 20000);

	std::vector<Entity*> grasses;
	for (int i = 0; i < NUMBER_OF_EACH_ENTITY; i++)
	{
		float modelX = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;
		float modelZ = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;

		float noizeX = rand() % 100 - 50;
		float noizeZ = rand() % 100 - 50;

		modelX += noizeX;
		modelZ += noizeZ;

		entityPosition = { modelX, getTerrainY(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD) - 2.0f, modelZ };
		grasses.push_back(new Entity(*loader, grassTexturedModel, entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ GRASSES ************************

	// ************************ FLOWERS ************************
	ModelTexture* flowerTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "flower.png"));
	flowerTexture->setShineDamper(10);
	flowerTexture->setReflectivity(0);
	flowerTexture->setHasTransparency(true);
	flowerTexture->setUseFakeLighting(true);

	TexturedModel* flowerTexturedModel = new TexturedModel(grassModel, flowerTexture);

	srand(GetTickCount() + 30000);

	std::vector<Entity*> flowers;
	for (int i = 0; i < NUMBER_OF_EACH_ENTITY; i++)
	{
		float modelX = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;
		float modelZ = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;

		float noizeX = rand() % 100 - 50;
		float noizeZ = rand() % 100 - 50;

		modelX += noizeX;
		modelZ += noizeZ;

		entityPosition = { modelX, getTerrainY(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD) - 0.0f, modelZ };
		flowers.push_back(new Entity(*loader, flowerTexturedModel, entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ FLOWERS ************************

	// ************************ LIGHTS ************************

	const float firstLightX = 225.0f;
	const float firstLightZ = -350.0f;

	std::vector<Light*> lights;
	lights.push_back(new Light(glm::vec3(0.0f, 10000.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
	lights.push_back(new Light(glm::vec3(firstLightX, getTerrainY(firstLightX, firstLightZ, terrains, HALF_SIZE_OF_WORLD) + 14.7f * 1.5f, firstLightZ), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0009f, 0.0005f)));
	lights.push_back(new Light(glm::vec3(370.0f, getTerrainY(370.0f, -300.0f, terrains, HALF_SIZE_OF_WORLD) + 14.7f * 1.5f, -300.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0009f, 0.0005f)));
	lights.push_back(new Light(glm::vec3(280.0f, getTerrainY(280.0f, -405.0f, terrains, HALF_SIZE_OF_WORLD) + 14.7f * 1.5f, -405.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0009f, 0.0005f)));

	RawModel* lamp = loader->loadFromOBJ(RES_FOLDER "lamp.obj");
	ModelTexture* lampTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "lamp.png"));
	lampTexture->setUseFakeLighting(true);
	TexturedModel* texturedLamp = new TexturedModel(lamp, lampTexture);
	
	Entity* lampEntity = new Entity(*loader, texturedLamp, glm::vec3(firstLightX, getTerrainY(firstLightX, firstLightZ, terrains, HALF_SIZE_OF_WORLD), firstLightZ), glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);
	Entity* lampEntity1 = new Entity(*loader, texturedLamp, glm::vec3(370.0f, getTerrainY(370.0f, -300.0f, terrains, HALF_SIZE_OF_WORLD), -300.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);
	Entity* lampEntity2 = new Entity(*loader, texturedLamp, glm::vec3(280.0f, getTerrainY(280.0f, -405.0f, terrains, HALF_SIZE_OF_WORLD), -405.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);
	
	// ************************ LIGHTS ************************

	/*RawModel* plane = loader->loadToVAO(std::string("plane"));
	ModelTexture* texture1 = new ModelTexture(loader->loadTexture("C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/dragon.png"));
	TexturedModel* texturedPlane = new TexturedModel(plane, texture1);
	Entity* planeEntity = new Entity(texturedPlane, glm::vec3(0.0f, -11.5f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), 10.0f);*/

	/*RawModel* leaves = loader->loadFromOBJ(RES_FOLDER "untitled.obj");
	ModelTexture* leavesTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "DeadLeavesAtlas.png"));
	TexturedModel* texturedLeaves = new TexturedModel(leaves, leavesTexture);
	Entity* leavesEntity = new Entity(texturedLeaves, glm::vec3(0.0f, 10.5f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), 0.1f);

	RawModel* bogMyrtle = loader->loadFromOBJ(RES_FOLDER "untitled2.obj");
	ModelTexture* bogMyrtleTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "T_FieldScabious01_D.png"));
	bogMyrtleTexture->setShineDamper(10);
	bogMyrtleTexture->setReflectivity(0);
	bogMyrtleTexture->setHasTransparency(true);
	bogMyrtleTexture->setUseFakeLighting(true);

	TexturedModel* texturedBogMyrtle = new TexturedModel(bogMyrtle, bogMyrtleTexture);
	Entity* bogMyrtleEntity = new Entity(texturedBogMyrtle, glm::vec3(0.0f, 10.5f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), 0.1f);*/

	RawModel* rock = loader->loadFromOBJ(RES_FOLDER "rock2.obj");
	ModelTexture* rockTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "rock.png"));
	TexturedModel* texturedRock = new TexturedModel(rock, rockTexture);
	Entity* rockEntity = new Entity(*loader, texturedRock, glm::vec3(250.0f, getTerrainY(220.0f, -405.0f, terrains, HALF_SIZE_OF_WORLD), -405.0f), glm::vec3(0.0f, 90.0f, 0.0f), 0.05f);

	MasterRenderer* renderer = new MasterRenderer(glm::vec3(101.0f / 255.0f, 150.0f / 255.0f, 206.0f / 255.0f), *loader,
												  { RES_FOLDER "skyBox/right.png", RES_FOLDER "skyBox/left.png", RES_FOLDER "skyBox/top.png", RES_FOLDER "skyBox/bottom.png", RES_FOLDER "skyBox/back.png", RES_FOLDER "skyBox/front.png" },
												  { RES_FOLDER "skyBox/nightRight.png", RES_FOLDER "skyBox/nightLeft.png", RES_FOLDER "skyBox/nightTop.png", RES_FOLDER "skyBox/nightBottom.png", RES_FOLDER "skyBox/nightBack.png", RES_FOLDER "skyBox/nightFront.png" });

	// ************************ PLAYER ************************
	RawModel* playerModel = loader->loadFromOBJ(RES_FOLDER "person.obj");
	ModelTexture* playerTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "playerTexture.png"));
	playerTexture->setShineDamper(10);
	playerTexture->setReflectivity(0);

	TexturedModel* playerTexturedModel = new TexturedModel(playerModel, playerTexture);

	player = new Player(*loader, playerTexturedModel, glm::vec3(153.0f, 5.0f, -274.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);
	//camera = new ModelViewCamera(*player);
	camera = new FreeCamera(main_hwnd, glm::vec3(153.0f, 25.0f, -274.0f));
	//camera = new ModelViewCamera(*rockEntity);

	// ************************ PLAYER ************************

	// ************************ WATER ************************

	waterFrameBuffers = new WaterFrameBuffers();
	waterShader = new WaterShader();
	waterRenderer = new WaterRenderer(*loader, *waterShader, renderer->getProjectionMatrix(), *waterFrameBuffers, RES_FOLDER "waterDUDV.png", RES_FOLDER "normalMap.png");
	std::vector<WaterTile*> waters;
	waters.push_back(new WaterTile(153.0f, -274.0f, 20.0f));

	// ************************ WATER ************************

	// ************************ GUIS ************************

	std::vector<GuiTexture*> guis;
	// pivot is at 0.5f, 0.5f !
	//guis.push_back(new GuiTexture(loader->loadTexture(RES_FOLDER "healthGui.png"), glm::vec2(-0.75f, 0.9f), glm::vec2(0.25f, 0.25f)));
	//guis.push_back(new GuiTexture(waterFrameBuffers->getRefractionTexture(), glm::vec2(0.5f, 0.5f), glm::vec2(0.25f, 0.25f)));
	//guis.push_back(new GuiTexture(waterFrameBuffers->getReflectionTexture(), glm::vec2(-0.5f, 0.5f), glm::vec2(0.25f, 0.25f)));

	GuiRenderer* guiRenderer = new GuiRenderer(*loader);

	// ************************ GUIS ************************

	RECT rc;
	GetClientRect(main_hwnd, &rc);

	MousePicker* mousePicker = new MousePicker(*camera, renderer->getProjectionMatrix(), rc.right - rc.left, rc.bottom - rc.top, *terrains[getTerrainIndexOutOfWorldXZ(player->getPosition().x, player->getPosition().z, terrains, HALF_SIZE_OF_WORLD)]);

	auto compactRenderer = [&](glm::vec4 clipPlane) {
		//Sleep(5);
		//mousePicker->update();
		//Time::getInstance().startNewFrame();

		for (auto terrain : terrains) {
			renderer->processTerrain(*terrain);
		}

		for (auto tree : trees) {
			renderer->processEntity(*tree);
		}

		//lampEntity->setPosition(glm::vec3(mousePicker->getCurrentTerrainPoint().x, mousePicker->getCurrentTerrainPoint().y, mousePicker->getCurrentTerrainPoint().z));
		//lights[1]->setPosition(glm::vec3(mousePicker->getCurrentTerrainPoint().x, mousePicker->getCurrentTerrainPoint().y + 14.7f * 1.5f, mousePicker->getCurrentTerrainPoint().z));

		for (auto fern : ferns) {
			renderer->processEntity(*fern);
		}

		for (auto grass : grasses) {
			renderer->processEntity(*grass);
		}

		for (auto flower : flowers) {
			renderer->processEntity(*flower);
		}

		renderer->processEntity(*rockEntity);

		renderer->processEntity(*lampEntity);
		renderer->processEntity(*lampEntity1);
		renderer->processEntity(*lampEntity2);

		renderer->processEntity(*player);

		renderer->render(lights, camera, glm::vec3(200.0f / 255.0f, 200.0f / 255.0f, 220.0f / 255.0f), clipPlane);
	};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			Time::getInstance().startNewFrame();
			mousePicker->update();
			camera->move(Time::getInstance().getTimePassedFromLastCallMS() / 1000.0f);
			player->move(*terrains[getTerrainIndexOutOfWorldXZ(player->getPosition().x, player->getPosition().z, terrains, HALF_SIZE_OF_WORLD)]);

			GL_ERROR(glEnable(GL_CLIP_DISTANCE0));

			float distance = (camera->getPosition().y - waters[0]->getHeight()) * 2.0f;
			camera->setPitch(-camera->getPitch());
			camera->setPosition(glm::vec3(camera->getPosition().x, camera->getPosition().y - distance, camera->getPosition().z));

			waterFrameBuffers->bindReflectionFrameBuffer();
			compactRenderer(glm::vec4(0.0, 1.0, 0.0, -waters[0]->getHeight()));

			camera->setPitch(-camera->getPitch());
			camera->setPosition(glm::vec3(camera->getPosition().x, camera->getPosition().y + distance, camera->getPosition().z));

			waterFrameBuffers->bindRefractionFrameBuffer();
			compactRenderer(glm::vec4(0.0, -1.0, 0.0, waters[0]->getHeight()));

			waterFrameBuffers->unbindCurrentFrameBuffer();
			compactRenderer(glm::vec4(0.0, 0.0, 0.0, 0.0));
			waterRenderer->render(waters, *camera, lights[0]);
			guiRenderer->render(guis);

            SwapBuffers(g_hDC);
        }
    }

	delete guiRenderer;
	delete renderer;
	delete waterRenderer;
	delete waterFrameBuffers;

    return (int)msg.wParam;
}

//
// FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
// PURPOSE: Processes messages for the main window.//
// WM_PAINT - Paint the main window
// WM_DESTROY - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
		case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);


        EndPaint(hWnd, &ps);
        break;

		case WM_KEYDOWN:
		{
			if (wParam == 0x1B/*VK_ESCAPE*/) PostQuitMessage(0);

			if (wParam == 0x26/*VK_UP*/)
			{
				camera->setKey(1);
				player->setKey(1);
			}

			if (wParam == 0x28/*VK_DOWN*/)
			{
				camera->setKey(2);
				player->setKey(2);
			}

			if (wParam == 37/*VK_LEFT*/)
			{
				camera->setKey(3);
				player->setKey(3);
			}

			if (wParam == 39/*VK_RIGHT*/)
			{
				camera->setKey(4);
				player->setKey(4);
			}

			if (wParam == VK_SPACE) {
				player->jump();
			}
		}
		break;

		case WM_KEYUP:
		{
			camera->setKey(0);
			player->setKey(0);
		}
		break;

		case WM_MOUSEMOVE:
		{
			if (hWnd == GetActiveWindow()) {
				int mouseX = GET_X_LPARAM(lParam);
				int mouseY = GET_Y_LPARAM(lParam);
				//POINT mouse;
				//GetCursorPos(&mouse);

				RECT rc, screen_wr;
				GetClientRect(hWnd, &rc);
				GetWindowRect(hWnd, &screen_wr);

				//int current_mouse_x = screen_wr.left + mouseX + 8;
				//int current_mouse_y = screen_wr.top + mouseY + 31;

				//camera->setMouseCoordinates(mouse.x, mouse.y);

				//camera->setMouseCoordinates(mouseX, mouseY);
				//camera->setMouseCoordinates(current_mouse_x, current_mouse_y);
				Input::getInstance().setMouseCoordinates(mouseX, mouseY);
			}
		}
		break;

		case WM_MOUSEWHEEL:
		{
			camera->setMouseWheel(GET_Y_LPARAM(wParam));
		}
		break;

		case WM_LBUTTONUP:
		{
			camera->setMouseLeftButtonState(false);
		}
		break;

		case WM_LBUTTONDOWN:
		{
			camera->setMouseLeftButtonState(true);
		}
		break;

		case WM_RBUTTONUP:
		{
			camera->setMouseRightButtonState(false);
		}
		break;

		case WM_RBUTTONDOWN:
		{
			camera->setMouseRightButtonState(true);
		}
		break;

        return DefWindowProc(hWnd, message, wParam, lParam);

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;

    }

    return 0;
};