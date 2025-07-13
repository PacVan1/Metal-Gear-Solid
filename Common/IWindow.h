#pragma once

#ifdef WINDOWS_BUILD
#include <GLFW/glfw3.h>

#else
#include <GLES2/gl2.h>

#endif

#ifdef Raspberry_BUILD
#include <GLES3/gl3.h>
#endif

#ifdef WINDOWS_BUILD
#include "glad/glad.h"
#endif

auto constexpr WINDOW_TITLE		= "Year 1, Block B Retake - Ayser Hartgring";
auto constexpr RES_WIDTH		= 320; 
auto constexpr RES_HEIGHT		= 180; 
auto constexpr WINDOW_WIDTH		= 1280; 
auto constexpr WINDOW_HEIGHT	= 720; 
auto constexpr ASPECT_RATIO		= static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);

class IWindow
{
public:
	void ClearColor()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void Clear()
	{
		ClearColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	virtual void	EnableCursor() = 0;
	virtual void	DisableCursor() = 0;
	virtual void	SwapBuffer() = 0;
	virtual void	Quit() = 0;
};

// factory-singleton design pattern
IWindow& GetWindow();
float GetFPS();
void Quit();