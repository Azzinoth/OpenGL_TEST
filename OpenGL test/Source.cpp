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

#define WIN_W 1920
#define WIN_H 1080

#define RES_FOLDER "C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/resources/"

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
        GLenum err = glGetError();

        RECT rc;
        GetClientRect(main_hwnd, &rc);

        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
            PFD_TYPE_RGBA,             //The kind of framebuffer. RGBA or palette.
            32,                        //Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                        //Number of bits for the depthbuffer
            8,                         //Number of bits for the stencilbuffer
            0,                         //Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };
        err = glGetError();

        HDC windowDC = GetDC(main_hwnd);
        int closePixelFormat = ChoosePixelFormat(windowDC, &pfd);
        SetPixelFormat(windowDC, closePixelFormat, &pfd);

        err = glGetError();

        HGLRC openGLContext = wglCreateContext(windowDC);
        bool result = wglMakeCurrent(windowDC, openGLContext);

        err = glGetError();

        //MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
        
        // I hate it :)
        //glewExperimental = GL_TRUE;
        glewInit();

        GLuint framebuffer = 0;
        err = glGetError();
        glGenFramebuffers(1, &framebuffer);
        err = glGetError();
        glBindFramebuffer(GL_FRAMEBUFFER, 0/*framebuffer*/); // OpenGL framebuffer = 0 by default in OpenGL ES it is not 0

        GLuint colorRenderbuffer;
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, WIN_W, WIN_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

        GLuint depthRenderbuffer;
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, WIN_W, WIN_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) 
		{
            MessageBoxA(0, "failed to make complete framebuffer object", "OPENGL VERSION", 0);
        }

        err = glGetError();

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        err = glGetError();
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

		trees.push_back(new Entity(treeTexturedModel, glm::vec3(modelX, -5.0f, modelZ), glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ TREES ************************

	// ************************ FERNS ************************
	RawModel* fernModel = loader->loadFromOBJ(RES_FOLDER "fern.obj");
	ModelTexture* fernTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "fern.png"));
	fernTexture->setShineDamper(10);
	fernTexture->setReflectivity(0);
	fernTexture->setHasTransparency(true);

	TexturedModel* fernTexturedModel = new TexturedModel(fernModel, fernTexture);

	srand(GetTickCount() + 10000);

	std::vector<Entity*> ferns;
	for (int i = 0; i < NUMBER_OF_EACH_ENTITY; i++)
	{
		float modelX = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;
		float modelZ = rand() % MAX_RANDOM - MAX_RANDOM / 2.0;

		float noizeX = rand() % 100 - 50;
		float noizeZ = rand() % 100 - 50;

		modelX += noizeX;
		modelZ += noizeZ;

		ferns.push_back(new Entity(fernTexturedModel, glm::vec3(modelX, -3.0f, modelZ), glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
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

		grasses.push_back(new Entity(grassTexturedModel, glm::vec3(modelX, 0.0f, modelZ), glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
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

		flowers.push_back(new Entity(flowerTexturedModel, glm::vec3(modelX, 0.0f, modelZ), glm::vec3(0.0f, 0.0f, 0.0f), 3.0f));
	}
	// ************************ FLOWERS ************************

	Light* light = new Light(glm::vec3(1000.0f, 200.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	const int SIZE_OF_WORLD = 4;
	std::vector<Terrain*> terrains;
	terrains.reserve(SIZE_OF_WORLD * SIZE_OF_WORLD);

	TerrainTexture* backgroundTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "grassy.png"));
	TerrainTexture* rTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "mud.png"));
	TerrainTexture* gTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "grassFlowers.png"));
	TerrainTexture* bTexture = new TerrainTexture(loader->loadTexture(RES_FOLDER "path.png"));

	TerrainTexturePack* texturePack = new TerrainTexturePack(*backgroundTexture, *rTexture, *gTexture, *bTexture);
	TerrainTexture* blendMap = new TerrainTexture(loader->loadTexture(RES_FOLDER "blendMap.png"));
	std::string heightmap = RES_FOLDER "heightmap.png";
 
	for (int i = -SIZE_OF_WORLD / 2; i < SIZE_OF_WORLD / 2; i++) {
		for (int j = -SIZE_OF_WORLD / 2; j < SIZE_OF_WORLD / 2; j++) {
			terrains.push_back(new Terrain(i, j, *loader, *texturePack, *blendMap, heightmap));
		}
	}

	RawModel* plane = loader->loadToVAO(std::string("plane"));
	ModelTexture* texture1 = new ModelTexture(loader->loadTexture("C:/Users/Кондрат/Downloads/OpenGL test/OpenGL test/dragon.png"));
	TexturedModel* texturedPlane = new TexturedModel(plane, texture1);
	Entity* planeEntity = new Entity(texturedPlane, glm::vec3(0.0f, -11.5f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), 10.0f);

	MasterRenderer* renderer = new MasterRenderer();

	// ************************ PLAYER ************************
	RawModel* playerModel = loader->loadFromOBJ(RES_FOLDER "person.obj");
	ModelTexture* playerTexture = new ModelTexture(loader->loadTexture(RES_FOLDER "playerTexture.png"));
	flowerTexture->setShineDamper(10);
	flowerTexture->setReflectivity(0);

	TexturedModel* playerTexturedModel = new TexturedModel(playerModel, playerTexture);

	player = new Player(playerTexturedModel, glm::vec3(0.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);
	camera = new ModelViewCamera(*player);

	// ************************ PLAYER ************************

	bool inc = false;
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
				light->setPosition(glm::vec3(light->getPosition().x + 2, light->getPosition().y, light->getPosition().z));
				if (light->getPosition().x > 3200) inc = false;
			}

			if (!inc) {
				light->setPosition(glm::vec3(light->getPosition().x - 2, light->getPosition().y, light->getPosition().z));
				if (light->getPosition().x < 200) inc = true;
			}

				 
			//camera->setYaw(camera->getYaw() + 1);
			
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

			renderer->processEntity(*planeEntity);

			player->move(delta / 1000.0f);
			renderer->processEntity(*player);

			renderer->render(light, camera);
            SwapBuffers(GetDC(main_hwnd));
        }
    }

	delete renderer;

    GLenum err = glGetError();
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
//int last_mouse_x = 0;
//int last_mouse_y = 0;
//
//float current_mouse_x_angle = 0;
//float current_mouse_y_angle = 0;

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