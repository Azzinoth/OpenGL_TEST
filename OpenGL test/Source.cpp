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

Loader* loader;
EntityRenderer* renderer;
ModelViewCamera* camera;
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

int getTerrainIndexOutOfWorldXZ(int worldX, int WorldZ, std::vector<Terrain*>& terrains, int halfSizeOfWorld) {
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

		entityPosition = { modelX, terrains[getTerrainIndexOutOfWorldXZ(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD)]->getHeightOfTerrain(modelX, modelZ) - 4.0f, modelZ };
		trees.push_back(new Entity(treeTexturedModel, entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
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

		entityPosition = { modelX, terrains[getTerrainIndexOutOfWorldXZ(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD)]->getHeightOfTerrain(modelX, modelZ) - 2.0f, modelZ };
		ferns.push_back(new Entity(fernTexturedModel, rand() % 4 , entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
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

		entityPosition = { modelX, terrains[getTerrainIndexOutOfWorldXZ(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD)]->getHeightOfTerrain(modelX, modelZ) - 2.0f, modelZ };
		grasses.push_back(new Entity(grassTexturedModel, entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
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

		entityPosition = { modelX, terrains[getTerrainIndexOutOfWorldXZ(modelX, modelZ, terrains, HALF_SIZE_OF_WORLD)]->getHeightOfTerrain(modelX, modelZ) - 0.0f, modelZ };
		flowers.push_back(new Entity(flowerTexturedModel, entityPosition, glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ FLOWERS ************************

	Light* light = new Light(glm::vec3(1000.0f, 200.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	/*RawModel* plane = loader->loadToVAO(std::string("plane"));
	ModelTexture* texture1 = new ModelTexture(loader->loadTexture("C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/dragon.png"));
	TexturedModel* texturedPlane = new TexturedModel(plane, texture1);
	Entity* planeEntity = new Entity(texturedPlane, glm::vec3(0.0f, -11.5f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), 10.0f);*/

	MasterRenderer* renderer = new MasterRenderer(glm::vec3(101.0f / 255.0f, 150.0f / 255.0f, 206.0f / 255.0f));

	// ************************ PLAYER ************************
	RawModel* playerModel = loader->loadFromOBJ(RES_FOLDER "person.obj");
	ModelTexture* playerTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "playerTexture.png"));
	playerTexture->setShineDamper(10);
	playerTexture->setReflectivity(0);

	TexturedModel* playerTexturedModel = new TexturedModel(playerModel, playerTexture);

	player = new Player(playerTexturedModel, glm::vec3(10.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);
	camera = new ModelViewCamera(*player);

	// ************************ PLAYER ************************

	bool inc = true;
	GLenum error;
	float RED = 1.0;
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			Sleep(10);

			if (inc) {
				light->setPosition(glm::vec3(light->getPosition().x + 3, light->getPosition().y, light->getPosition().z));
				if (light->getPosition().x > 1100) inc = false;
			}

			if (!inc) {
				light->setPosition(glm::vec3(light->getPosition().x - 3, light->getPosition().y, light->getPosition().z));
				if (light->getPosition().x < -1100) inc = true;
			}

			Time time = Time::getInstance();
			float delta = time.getTimePassedFromLastCallMS();

			for (auto terrain : terrains) {
				renderer->processTerrain(*terrain);
			}

			for (auto tree : trees) {
				renderer->processEntity(*tree);
			}

			for (auto fern : ferns) {
				renderer->processEntity(*fern);
			}

			for (auto grass : grasses) {
				renderer->processEntity(*grass);
			}

			for (auto flower : flowers) {
				renderer->processEntity(*flower);
			}

			//renderer->processEntity(*planeEntity);

			player->move(delta / 1000.0f, *terrains[getTerrainIndexOutOfWorldXZ(player->getPosition().x, player->getPosition().z, terrains, HALF_SIZE_OF_WORLD)]);
			renderer->processEntity(*player);


			renderer->render(light, camera, glm::vec3(101.0f / 255.0f, 150.0f / 255.0f, 206.0f / 255.0f));

			//glClearColor(RED, 150.0f / 255.0f, 0.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/);

            SwapBuffers(g_hDC);
        }
    }

	delete renderer;

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


        // End application-specific layout section.EndPaint(hWnd, &ps);
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
			POINT mouse;
			GetCursorPos(&mouse);

			camera->setMouseCoordinates(mouse.x, mouse.y);
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