#include "common.h"

//#include <GLES3/gl3.h>
//#include <GLES2/gl2ext.h>

// OpenGL ES 3.1
#include <GLES3/gl31.h> 

// context creation for OpenGL ES
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <EGL/eglext.h>

// Linux windowing
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h> // for default cursor pixmap

// input
#include <linux/input.h>

struct EGLState
{
	EGLDisplay				display;
	EGLSurface				surface;
	EGLContext				context;
	EGLConfig				config;
	EGLNativeWindowType		nativeWindow;
};

struct XWindowData
{
	Display*	display;
	Window		root;
	Window		window;
	EGLState	state;
};

static EGLint constexpr CONTEXT_ATTRIBUTES[] =
{
    EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
    EGL_CONTEXT_MINOR_VERSION_KHR, 1,
    EGL_NONE,
    EGL_NONE
};

static EGLint constexpr ATTRIBUTE_LIST[] =
{
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 16,
    EGL_SURFACE_TYPE,
    EGL_WINDOW_BIT,
    EGL_CONFORMANT,
    EGL_OPENGL_ES3_BIT_KHR,
    EGL_NONE
};

static void CreateWindow(XWindowData& data)
{
	data.display	= XOpenDisplay(nullptr);
	data.root		= RootWindowOfScreen(DefaultScreenOfDisplay(data.display));

	XSetWindowAttributes swa, xattr;
	Atom wm_state;
	XWMHints hints;

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;

	swa.event_mask			= ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
	swa.background_pixmap	= None;
	swa.background_pixel	= 0;
	swa.border_pixel		= 0;
	swa.override_redirect	= true;

	//data.window = XCreateWindow(
	//	data.display, data.root, 0, 0,
	//	WINDOW_WIDTH, WINDOW_HEIGHT, 0,
	//	CopyFromParent, InputOutput,
	//	CopyFromParent, CWEventMask,
	//	&swa);

	data.window = XCreateWindow(
		data.display, data.root, 0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0,
		CopyFromParent, InputOutput,
		CopyFromParent, CWBackPixel | CWBorderPixel | CWEventMask,
		&swa);

	XSelectInput(data.display, data.window, KeyPressMask | KeyReleaseMask);
	xattr.override_redirect = false;
	XChangeWindowAttributes(data.display, data.window, CWOverrideRedirect, &xattr);

	hints.input = true;
	hints.flags = InputHint;
	XSetWMHints(data.display, data.window, &hints);

	// make the window visible on the screen
	XMapWindow(data.display, data.window);
	XStoreName(data.display, data.window, WINDOW_TITLE);

	XClearWindow(data.display, data.window);
	XMapRaised(data.display, data.window);
	XSelectInput(data.display, data.window, FocusChangeMask | 
		KeyPressMask | KeyReleaseMask | 
		ButtonPressMask | ButtonReleaseMask | 
		PointerMotionMask);

	wm_state = XInternAtom(data.display, "_NET_WM_STATE", false);
	Atom wm_fullscreen = XInternAtom(data.display, "_NET_WM_STATE_FULLSCREEN", false);

	XChangeProperty(data.display, data.window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)&wm_fullscreen, 1);

	//XEvent xev;
	//memset(&xev, 0, sizeof(xev));
	//xev.type					= ClientMessage;
	//xev.xclient.window			= data.window;
	//xev.xclient.message_type	= wm_state;
	//xev.xclient.format			= 32;
	//xev.xclient.data.l[0]		= 1;
	//xev.xclient.data.l[1]		= false;
	//XSendEvent(
	//data.display,
	//DefaultRootWindow(data.display),
	//false, SubstructureNotifyMask,
	//&xev);

	data.state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(data.state.display, &majorVersion, &minorVersion);
	eglGetConfigs(data.state.display, nullptr, 0, &numConfigs);
	eglChooseConfig(data.state.display, ATTRIBUTE_LIST, &data.state.config, 1, &numConfigs);

	data.state.context = eglCreateContext(data.state.display, data.state.config, nullptr, CONTEXT_ATTRIBUTES);
	data.state.surface = eglCreateWindowSurface(data.state.display, data.state.config, data.window, nullptr);

	eglMakeCurrent(data.state.display, data.state.surface, data.state.surface, data.state.context);
	eglSurfaceAttrib(data.display, data.state.surface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
	eglSwapInterval(data.state.display, 1);

#ifdef NO_CURSOR
	Cursor invisibleCursor;
	Pixmap bitmapNoData;
	XColor black;
	static char noData[] = {0, 0, 0, 0, 0, 0, 0, 0};
	black.red = black.green = black.blue = 0;
	bitmapNoData = XCreateBitmapFromData(data.display, data.window, noData, 8, 8);
	invisibleCursor = XCreatePixmapCursor(data.display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
	XDefineCursor(data.display, data.window, invisibleCursor);
	XFreeCursor(data.display, invisibleCursor);
	XFreePixmap(data.display, bitmapNoData);
#endif
}
static void HandleEvents(XWindowData const& data);

class RaspWindow final : public IWindow
{
public:
	XWindowData mWindow;

public:
	RaspWindow() { CreateWindow(mWindow); }
	virtual void EnableCursor() override;
	virtual void DisableCursor() override;
	virtual void Quit() override;
	virtual void SwapBuffer() override;
};

Cursor CreateInvisibleCursor(Display* display, Window window)
{
	Pixmap blank;
	XColor dummy;
	char data[1] = { 0 };
	blank = XCreateBitmapFromData(display, window, data, 1, 1);
	Cursor cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
	XFreePixmap(display, blank);
	return cursor;
}

void RaspWindow::Quit()
{
	XDestroyWindow(mWindow.display, mWindow.window);
}

void RaspWindow::SwapBuffer()
{
	eglSwapBuffers(mWindow.state.display, mWindow.state.surface);
}

RaspWindow gWindow;
ivec2	   gMousePos;
ivec2	   gMouseDeltaPos;
bool	   gWarpCursor;

static void SetCursorVisibility(bool const visible)
{
	static bool initialized = false;
	static Cursor defaultCursor;
	static Cursor invisibleCursor;

	if (!initialized)
	{
		initialized = true;

		// default
		defaultCursor = XCreateFontCursor(gWindow.mWindow.display, XC_left_ptr);

		// invisible
		static char constexpr NO_DATA[] = { 0,0,0,0,0,0,0,0 };
		Pixmap BLANK_PIXMAP = XCreateBitmapFromData(gWindow.mWindow.display, gWindow.mWindow.window, NO_DATA, 8, 8);
		XColor dummyColor;
		invisibleCursor = XCreatePixmapCursor(gWindow.mWindow.display, BLANK_PIXMAP, BLANK_PIXMAP, &dummyColor, &dummyColor, 0, 0);
	}

	if (visible)
		XDefineCursor(gWindow.mWindow.display, gWindow.mWindow.window, defaultCursor);
	else
		XDefineCursor(gWindow.mWindow.display, gWindow.mWindow.window, invisibleCursor);
}

void RaspWindow::EnableCursor()
{
	SetCursorVisibility(true);
	gWarpCursor = false;
}

void RaspWindow::DisableCursor()
{
	SetCursorVisibility(false);
	gWarpCursor = true;
}

IWindow& GetWindow()
{
	static IWindow* window = &gWindow;
	return *window;
}

static void* ProcessKeyboardThread(void* arg);

class Input : public IInput
{
public:
	XWindowData*	mWindow;
	pthread_t		mKeyboardThread;
	std::string		mKeyboardLocation;
	bool			mCursorDisabled; 

public:
	Input(XWindowData* window) : 
		mWindow(window)
	{ 
		InitKeyMapping(); 
		FindKeyboardLocation();
		pthread_create(&mKeyboardThread, nullptr, &ProcessKeyboardThread, this);
	}
	void			FindKeyboardLocation();
	std::string		FindActiveKeyboardEv();
	virtual	void	InitKeyMapping() override;
	virtual float	GetDeltaScroll() const override;
	virtual ivec2	GetMousePos() const override;
	virtual ivec2	GetMouseDeltaPos() const override;
};

void HandleEvents(XWindowData const& data)
{
	XEvent event;
	while (XPending(data.display)) 
	{
		XNextEvent(data.display, &event);
		ImGuiIO& io = ImGui::GetIO();
		switch (event.type)
		{
		case ButtonPress:	io.MouseDown[event.xbutton.button] = true; break;
		case ButtonRelease:	io.MouseDown[event.xbutton.button] = false; break;
		case MotionNotify:	io.MousePos = ImVec2(event.xmotion.x, event.xmotion.y); break;
		case FocusIn:		XSetInputFocus(data.display, data.window, RevertToNone, CurrentTime); break;
		}
	}
}

void* ProcessKeyboardThread(void* arg)
{
	Input* input = static_cast<Input*>(arg);
	FILE* fKeyboard;
	fKeyboard = fopen(((Input*)arg)->mKeyboardLocation.c_str(), "r");
	if (fKeyboard == nullptr)
	{
		DebugLog(ERROR, "Keyboard file not found.");
		pthread_exit(nullptr);
		return nullptr;
	}
	input_event event;
	while (true)
	{
		fread(&event, sizeof(input_event), 1, fKeyboard);
		if (event.type == (__u16)EV_KEY)
		{
			GetInput().mShouldUpdate = true;
			input->mKeyStates[event.code] = event.value > 0;
		}
	}
	fclose(fKeyboard);
	pthread_exit(nullptr);
}
void Input::FindKeyboardLocation()
{
	std::string ev{ FindActiveKeyboardEv() };
	mKeyboardLocation = "/dev/input/" + ev;
}
std::string Input::FindActiveKeyboardEv()
{
	std::ifstream devicesFile;
	std::stringstream devicesStream;
	devicesFile.open("/proc/bus/input/devices");
	devicesStream << devicesFile.rdbuf();
	devicesFile.close();
	std::string devices = devicesStream.str();
	std::size_t newLinePos{ 0 };
	std::string ev{};
	do
	{
		if (devices.substr(newLinePos + 1, 11) == "H: Handlers")
		{
			std::size_t nextNewLine{ devices.find('\n', newLinePos + 1) - 1 };
			std::string line{ devices.substr(newLinePos + 1, nextNewLine - (newLinePos + 1)) };
			std::size_t lastSpace{ line.rfind(' ') };
			ev = line.substr(lastSpace + 1, line.length() - lastSpace + 1);
		}
		if (devices.substr(newLinePos + 1, 5) == "B: EV")
		{
			std::size_t start{ newLinePos + 7 };
			newLinePos = devices.find('\n', newLinePos + 1);
			std::string stringValue{ devices.substr(start, newLinePos - start) };
			unsigned int value;
			std::stringstream ss;
			ss << std::hex << stringValue;
			ss >> value;
			constexpr unsigned int keyboardMask{ 0x120013 };
			if ((value & keyboardMask) == keyboardMask) break;
			continue;
		}
		newLinePos = devices.find('\n', newLinePos + 1);
	} while (newLinePos != std::string::npos);
	return ev;
}
void Input::InitKeyMapping()
{
	mKeyMapping[KEYS_A]				= KEY_A;
	mKeyMapping[KEYS_B]				= KEY_B;
	mKeyMapping[KEYS_C]				= KEY_C;
	mKeyMapping[KEYS_D]				= KEY_D;
	mKeyMapping[KEYS_E]				= KEY_E;
	mKeyMapping[KEYS_F]				= KEY_F;
	mKeyMapping[KEYS_G]				= KEY_G;
	mKeyMapping[KEYS_H]				= KEY_H;
	mKeyMapping[KEYS_I]				= KEY_I;
	mKeyMapping[KEYS_J]				= KEY_J;
	mKeyMapping[KEYS_K]				= KEY_K;
	mKeyMapping[KEYS_L]				= KEY_L;
	mKeyMapping[KEYS_M]				= KEY_M;
	mKeyMapping[KEYS_N]				= KEY_N;
	mKeyMapping[KEYS_O]				= KEY_O;
	mKeyMapping[KEYS_P]				= KEY_P;
	mKeyMapping[KEYS_Q]				= KEY_Q;
	mKeyMapping[KEYS_R]				= KEY_R;
	mKeyMapping[KEYS_S]				= KEY_S;
	mKeyMapping[KEYS_T]				= KEY_T;
	mKeyMapping[KEYS_U]				= KEY_U;
	mKeyMapping[KEYS_V]				= KEY_V;
	mKeyMapping[KEYS_W]				= KEY_W;
	mKeyMapping[KEYS_X]				= KEY_X;
	mKeyMapping[KEYS_Y]				= KEY_Y;
	mKeyMapping[KEYS_Z]				= KEY_Z;
	mKeyMapping[KEYS_0]				= KEY_0;
	mKeyMapping[KEYS_1]				= KEY_1;
	mKeyMapping[KEYS_2]				= KEY_2;
	mKeyMapping[KEYS_3]				= KEY_3;
	mKeyMapping[KEYS_4]				= KEY_4;
	mKeyMapping[KEYS_5]				= KEY_5;
	mKeyMapping[KEYS_6]				= KEY_6;
	mKeyMapping[KEYS_7]				= KEY_7;
	mKeyMapping[KEYS_8]				= KEY_8;
	mKeyMapping[KEYS_9]				= KEY_9;
	mKeyMapping[KEYS_TAB]			= KEY_TAB;
	mKeyMapping[KEYS_CAPS_LOCK]		= KEY_CAPSLOCK;
	mKeyMapping[KEYS_LEFT_SHIFT]	= KEY_LEFTSHIFT;
	mKeyMapping[KEYS_LEFT_CTRL]		= KEY_LEFTCTRL;
	mKeyMapping[KEYS_ESCAPE]		= KEY_HOMEPAGE;
	mKeyMapping[KEYS_RIGHT_SHIFT]	= KEY_RIGHTSHIFT;
	mKeyMapping[KEYS_ENTER]			= KEY_ENTER;
	mKeyMapping[KEYS_UP]			= KEY_UP;
	mKeyMapping[KEYS_RIGHT]			= KEY_RIGHT;
	mKeyMapping[KEYS_DOWN]			= KEY_DOWN;
	mKeyMapping[KEYS_LEFT]			= KEY_LEFT;
	mKeyMapping[KEYS_SPACE]			= KEY_SPACE;
}

float Input::GetDeltaScroll() const
{
	return 0.0f;
}

ivec2 Input::GetMousePos() const
{
	//int rootX, rootY;
	//int winX, winY;
	//uint maskReturned;
	//Window windowReturned;
	//
	//XQueryPointer(
	//	mWindow->display, mWindow->window,
	//	&windowReturned, &windowReturned,
	//	&rootX, &rootY,
	//	&winX, &winY,
	//	&maskReturned);
	//
	//return { winX, winY };

	return gMousePos;
}

ivec2 Input::GetMouseDeltaPos() const
{
	return gMouseDeltaPos;
}

IInput& GetInput()
{
	static IInput* input = new Input(&gWindow.mWindow);
	return *input;
}

static float gDT = 0.0f;
static float gFPS = 0.0f;
inline static float GetDeltaTime()
{
	static Timer timer;
	gDT = std::min(500.0f, 1000.0f * timer.Elapsed()); timer.Reset();
	return gDT;
}

float GetFPS()
{
	return gFPS;
}

static void WarpCursorToCenter(Display* display, Window window, int width, int height)
{
	//XWarpPointer(display, None, window, 0, 0, 0, 0, width / 2, height / 2);
	//XFlush(display); // Ensure the command is sent immediately
}

static void HandleXEvents()
{
	ImGuiIO& io = ImGui::GetIO();

	XEvent event;
	while (XPending(gWindow.mWindow.display))
	{
		XNextEvent(gWindow.mWindow.display, &event);

		if (event.type == MotionNotify)
		{
			io.AddMousePosEvent(event.xmotion.x, event.xmotion.y);
			gMouseDeltaPos.x = event.xmotion.x - gMousePos.x;
			gMouseDeltaPos.y = event.xmotion.y - gMousePos.y;
			gMousePos.x = event.xmotion.x;
			gMousePos.y = event.xmotion.y;
		}
		else if (event.type == ButtonPress || event.type == ButtonRelease)
		{
			bool pressed = (event.type == ButtonPress);
			switch (event.xbutton.button)
			{
			case Button1: // left
			{
				io.AddMouseButtonEvent(0, pressed);
				GetInput().mButtonStates[MOUSE_LEFT] = pressed;
				break;
			}
			case Button2: // middle
			{
				io.AddMouseButtonEvent(2, pressed);
				GetInput().mButtonStates[MOUSE_MIDDLE] = pressed;
				break;
			}
			case Button3:
			{
				io.AddMouseButtonEvent(1, pressed);
				GetInput().mButtonStates[MOUSE_RIGHT] = pressed;
				break;
			}
			case Button4: io.AddMouseWheelEvent(0.0f, 1.0f); break; // scroll up
			case Button5: io.AddMouseWheelEvent(0.0f, 1.0f); break; // scroll down
			}
		}
	}

	if (gWarpCursor)
	{
		int const centerX = Engine::GetWindowWidth() / 2;
		int const centerY = Engine::GetWindowHeight() / 2;
		XWarpPointer(gWindow.mWindow.display, None, gWindow.mWindow.window, 0, 0, 0, 0, centerX, centerY);
		XFlush(gWindow.mWindow.display);
	}
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RES_WIDTH, RES_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gTex2, 0);

	// Create depth renderbuffer
	gRBO2 = OGL::RBO();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RES_WIDTH, RES_HEIGHT);
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
		gPlatformSets.windowTitle	= "Linux - BUas - Year 1, Block B Retake - Ayser Hartgring";
	}
	PlatformSettings const& GetPlatformSettings()
	{
		return gPlatformSets;
	}
	Game& GetGame()
	{
		return *gGame;
	}
	void Quit()
	{
		GetWindow().Quit();
	}
}

int main()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init(IMGUI_GLSL_VERSION);
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	OGL::Init();

	Engine::gGame	= new Game();
	Editor editor	= Editor(Engine::gGame);
	IInput& input	= GetInput();
	IWindow& window	= GetWindow();

	LowResInit();
	PrepQuad();

	while (true)
	{
		float const dt = GetDeltaTime() / 1000.0f;
		gFPS = 1.0f / dt;

		// low res
		glBindFramebuffer(GL_FRAMEBUFFER, gFBO2);
		glViewport(0, 0, RES_WIDTH, RES_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		Engine::gGame->Tick(dt);

		// up scale
		static GLuint const PROGRAM = Shader::Load("main");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(PROGRAM);
		Shader::SetTexture2D("c", gTex2);
		glBindVertexArray(gVAO2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		CheckOGL();

		if (input.IsKeyReleased(KEYS_E)) editor.Toggle();
		input.UpdatePrevious();
		HandleXEvents();
		ImGui_ImplOpenGL3_NewFrame(); ImGui::NewFrame();
		editor.Tick();
		ImGui::Render(); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.SwapBuffer();
	}
	
	delete Engine::gGame;

    return 0;
}
