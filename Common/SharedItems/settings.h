#pragma once

//#define FULL_SCREEN		// activates full screen for windows
#define DEBUG_MODE			// gives access to the editor, wireframes, etc
#define LEAK_DETECTION_MODE	// activates leak detector for Windows

class Game;
class Editor;

class GameSettings
{
friend class Editor;
public:
	bool mWireFrame			= false;
	bool mFog				= true;

private:
	Game* mG				= nullptr;
	bool mPlayerPossessed	= true;
	bool mFreezed			= true;

public:
	static GameSettings& Instance()
	{
		static GameSettings instance = GameSettings();
		return instance;
	}

	GameSettings() = default;
	void SetPlayerPossessed(bool const value);
	void SetFreeze(bool const value);
	void SetGame(Game& game);

	inline bool IsPlayerPossessed() const	{ return mPlayerPossessed; }
	inline bool IsFreezed() const			{ return mFreezed; }

private:
	void SetPlayerPossessed();
};

#include "types.h"

namespace Engine
{
struct PlatformSettings
{
								// default settings
	u32			windowWidth		= 1280; // in pixels
	u32			windowHeight	= 720;	// in pixels
	u32			resWidth		= 320;	// in pixels
	u32			resHeight		= 180;	// in pixels
	f32			aspectRatio		= (f32)windowWidth / (f32)windowHeight;
	char const* windowTitle		= "BUas - Year 1, Block B Retake - Ayser Hartgring";
};

void					InitPlatformSettings();
PlatformSettings const& GetPlatformSettings();
Game&					GetGame();
void					Quit();

u32						GetWindowWidth();
u32						GetWindowHeight();
u32						GetResWidth();
u32						GetResHeight();
char const*				GetWindowTitle();
}

#define GS GameSettings::Instance()