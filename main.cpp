#include "settings.h"
#include "common.h"
#include "Game.h"
#include "ImGui-master/imgui_impl_glfw.h"

#ifdef _DEBUG
#ifdef LEAK_DETECTION_MODE
#include <vld/vld.h>
#endif
#endif

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

GLFWwindow* gGLFWWindow = nullptr;
ivec2		gMousePos;
ivec2		gMouseDeltaPos;
float		gMouseScrollDelta;
float		gFPS; // frames per second
Editor*		gEditor;

class Window final : public IWindow
{
public:
	virtual void EnableCursor() override;
	virtual void DisableCursor() override;
	virtual void Quit() override;
	virtual void SwapBuffer() override;
};

class Input final : public IInput
{
public:
	Input() { InitKeyMapping(); }
	virtual void	InitKeyMapping() override;
	virtual float	GetDeltaScroll() const override;
	virtual ivec2	GetMousePos() const override;
	virtual ivec2	GetMouseDeltaPos() const override;
};

static void KeyCallback(GLFWwindow*, int key, int, int action, int)
{
	GetInput().mShouldUpdate = true;
	if (action == GLFW_RELEASE) { if (key >= 0) GetInput().mKeyStates[key & (SYSTEM_KEY_COUNT - 1)] = false; return; }
	if (action == GLFW_PRESS) { if (key >= 0) GetInput().mKeyStates[key & (SYSTEM_KEY_COUNT - 1)] = true; return; }
}

static void ButtonCallback(GLFWwindow*, int button, int action, int)
{
	if (action == GLFW_RELEASE) { if (button >= 0) GetInput().mButtonStates[button] = false; return; }
	if (action == GLFW_PRESS) { if (button >= 0) GetInput().mButtonStates[button] = true; return; }
}

static void MousePosCallback(GLFWwindow*, double x, double y)
{
	glm::ivec2 nextMousePos = { static_cast<int>(x), static_cast<int>(y) };
	gMouseDeltaPos = nextMousePos - gMousePos;
	gMousePos = nextMousePos;
}

static void MouseScrollCallback(GLFWwindow*, double, double yOffset)
{
	gMouseScrollDelta = static_cast<float>(yOffset);
}

inline static float GetDeltaTime()
{
	static float deltaTime = 0.0f; static Timer timer;
	deltaTime = std::min(500.0f, 1000.0f * timer.Elapsed()); timer.Reset();
	return deltaTime;
}

inline void Window::SwapBuffer()
{
	glfwSwapBuffers(gGLFWWindow);
	glfwPollEvents();
}

void Window::EnableCursor()
{
	glfwSetInputMode(gGLFWWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::DisableCursor()
{
	glfwSetInputMode(gGLFWWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

inline void Window::Quit()
{
	glfwSetWindowShouldClose(gGLFWWindow, GLFW_TRUE);
}

inline IWindow& GetWindow()
{
	static unique_ptr<IWindow> window = make_unique<Window>();
	return *window;
}

float GetFPS()
{
	return gFPS;
}

inline IInput& GetInput()
{
	static unique_ptr<IInput> input = make_unique<Input>();
	return *input;
}

void Input::InitKeyMapping()
{
	mKeyMapping[KEYS_A] = GLFW_KEY_A;
	mKeyMapping[KEYS_B] = GLFW_KEY_B;
	mKeyMapping[KEYS_C] = GLFW_KEY_C;
	mKeyMapping[KEYS_D] = GLFW_KEY_D;
	mKeyMapping[KEYS_E] = GLFW_KEY_E;
	mKeyMapping[KEYS_F] = GLFW_KEY_F;
	mKeyMapping[KEYS_G] = GLFW_KEY_G;
	mKeyMapping[KEYS_H] = GLFW_KEY_H;
	mKeyMapping[KEYS_I] = GLFW_KEY_I;
	mKeyMapping[KEYS_J] = GLFW_KEY_J;
	mKeyMapping[KEYS_K] = GLFW_KEY_K;
	mKeyMapping[KEYS_L] = GLFW_KEY_L;
	mKeyMapping[KEYS_M] = GLFW_KEY_M;
	mKeyMapping[KEYS_N] = GLFW_KEY_N;
	mKeyMapping[KEYS_O] = GLFW_KEY_O;
	mKeyMapping[KEYS_P] = GLFW_KEY_P;
	mKeyMapping[KEYS_Q] = GLFW_KEY_Q;
	mKeyMapping[KEYS_R] = GLFW_KEY_R;
	mKeyMapping[KEYS_S] = GLFW_KEY_S;
	mKeyMapping[KEYS_T] = GLFW_KEY_T;
	mKeyMapping[KEYS_U] = GLFW_KEY_U;
	mKeyMapping[KEYS_V] = GLFW_KEY_V;
	mKeyMapping[KEYS_W] = GLFW_KEY_W;
	mKeyMapping[KEYS_X] = GLFW_KEY_X;
	mKeyMapping[KEYS_Y] = GLFW_KEY_Y;
	mKeyMapping[KEYS_Z] = GLFW_KEY_Z;
	mKeyMapping[KEYS_0] = GLFW_KEY_0;
	mKeyMapping[KEYS_1] = GLFW_KEY_1;
	mKeyMapping[KEYS_2] = GLFW_KEY_2;
	mKeyMapping[KEYS_3] = GLFW_KEY_3;
	mKeyMapping[KEYS_4] = GLFW_KEY_4;
	mKeyMapping[KEYS_5] = GLFW_KEY_5;
	mKeyMapping[KEYS_6] = GLFW_KEY_6;
	mKeyMapping[KEYS_7] = GLFW_KEY_7;
	mKeyMapping[KEYS_8] = GLFW_KEY_8;
	mKeyMapping[KEYS_9] = GLFW_KEY_9;
	mKeyMapping[KEYS_TAB] = GLFW_KEY_TAB;
	mKeyMapping[KEYS_CAPS_LOCK] = GLFW_KEY_CAPS_LOCK;
	mKeyMapping[KEYS_LEFT_SHIFT] = GLFW_KEY_LEFT_SHIFT;
	mKeyMapping[KEYS_LEFT_CTRL] = GLFW_KEY_LEFT_CONTROL;
	mKeyMapping[KEYS_LEFT_CTRL] = GLFW_KEY_LEFT_ALT;
	mKeyMapping[KEYS_ESCAPE] = GLFW_KEY_ESCAPE;
	mKeyMapping[KEYS_RIGHT_SHIFT] = GLFW_KEY_RIGHT_SHIFT;
	mKeyMapping[KEYS_ENTER] = GLFW_KEY_END;
	mKeyMapping[KEYS_UP] = GLFW_KEY_UP;
	mKeyMapping[KEYS_RIGHT] = GLFW_KEY_RIGHT;
	mKeyMapping[KEYS_DOWN] = GLFW_KEY_DOWN;
	mKeyMapping[KEYS_LEFT] = GLFW_KEY_LEFT;
	mKeyMapping[KEYS_SPACE] = GLFW_KEY_SPACE;
}

float Input::GetDeltaScroll() const
{
	return gMouseScrollDelta;
}

inline ivec2 Input::GetMousePos() const
{
	return gMousePos;
}

inline ivec2 Input::GetMouseDeltaPos() const
{
	return gMouseDeltaPos;
}

static void GUITick()
{
	ImGui_ImplOpenGL3_NewFrame(); 
	ImGui_ImplGlfw_NewFrame(); 
	ImGui::NewFrame();
	Engine::GetGame().RenderGUI();
#ifdef DEBUG_MODE
	gEditor->Tick();
#endif
	ImGui::Render(); 
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GLuint gVAO2 = 0;
GLuint gVBO2 = 0;
GLuint gFBO2 = 0;
GLuint gRBO2 = 0;
GLuint gTex2 = 0;

void LowResInit()
{
	gFBO2 = OGL::FBO();
	gTex2 = OGL::Texture2D();

	// Create and bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO2);

	// Create and bind color texture
	glBindTexture(GL_TEXTURE_2D, gTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Engine::GetResWidth(), Engine::GetResHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gTex2, 0);

	// Create depth renderbuffer
	gRBO2 = OGL::RBO();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Engine::GetResWidth(), Engine::GetResHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRBO2);

	// Unbind for safety
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PrepQuad()
{
	static constexpr float VERTS[] =
	{
		-1, 1,
		 1, 1,
		-1,-1,
		 1, 1,
		-1,-1,
		 1,-1
	};
	glGenVertexArrays(1, &gVAO2);
	glBindVertexArray(gVAO2);
	glGenBuffers(1, &gVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTS), VERTS, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	CheckOGL();
}

namespace Engine
{
	PlatformSettings	gPlatformSets;
	Game*				gGame;

	void InitPlatformSettings()
	{
		gPlatformSets.windowWidth	= 1280;
		gPlatformSets.windowHeight	= 720;
		gPlatformSets.resWidth		= 640;
		gPlatformSets.resHeight		= 360;
		gPlatformSets.windowTitle	= "Windows - BUas - Year 1, Block B Retake - Ayser Hartgring";
	}
	PlatformSettings const& GetPlatformSettings()
	{
		return gPlatformSets;
	}
	Game& Engine::GetGame()
	{
		return *gGame;
	}
	void Engine::Quit()
	{
		GetWindow().Quit();
	}
}

int main()
{
	Engine::InitPlatformSettings();

	glfwInit();
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#ifdef FULL_SCREEN
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	gGLFWWindow = glfwCreateWindow(Engine::GetWindowWidth(), Engine::GetWindowHeight(), Engine::GetWindowTitle(), monitor, nullptr);
#else
	gGLFWWindow = glfwCreateWindow(Engine::GetWindowWidth(), Engine::GetWindowHeight(), Engine::GetWindowTitle(), nullptr, nullptr);
#endif
	if (!gGLFWWindow)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(gGLFWWindow);
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		glfwTerminate();
	}
	glfwSwapInterval(1);
	glfwSetKeyCallback(gGLFWWindow, KeyCallback);
	glfwSetMouseButtonCallback(gGLFWWindow, ButtonCallback);
	glfwSetCursorPosCallback(gGLFWWindow, MousePosCallback);
	glfwSetScrollCallback(gGLFWWindow, MouseScrollCallback);
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(gGLFWWindow, true);
	ImGui_ImplOpenGL3_Init(IMGUI_GLSL_VERSION);
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)Engine::GetWindowWidth(), (float)Engine::GetWindowHeight());

	Engine::gGame	= new Game();
	gEditor			= new Editor(Engine::gGame);
	IInput& input	= GetInput();
	IWindow& window = GetWindow();

	OGL::Init();

	LowResInit();
	PrepQuad();

	while (!glfwWindowShouldClose(gGLFWWindow))
	{
		// low res
		glBindFramebuffer(GL_FRAMEBUFFER, gFBO2);
		glViewport(0, 0, Engine::GetResWidth(), Engine::GetResHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		
		float const dt = GetDeltaTime() / 1000.0f;
		gFPS = 1.0f / dt;
		
		Engine::GetGame().Tick(dt);

		// up scale
		static GLuint const PROGRAM = Shader::Load("main");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Engine::GetWindowWidth(), Engine::GetWindowHeight());
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(PROGRAM);
		Shader::SetTexture2D("c", gTex2);
		glBindVertexArray(gVAO2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		CheckOGL(); 

		if (Inputs::ToggleEditor()) gEditor->Toggle();
		if (Inputs::QuitApp()) Engine::Quit();
		input.UpdatePrevious(); gMouseDeltaPos = ivec2(0, 0);
		GUITick();
		window.SwapBuffer();
	}

	delete gEditor;
	delete Engine::gGame;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(gGLFWWindow);
	glfwTerminate();

	return 0;
}
