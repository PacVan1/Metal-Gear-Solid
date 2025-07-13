#include "common.h"
#include "settings.h"

void GameSettings::SetPlayerPossessed(bool const value)
{
	mPlayerPossessed = value;
	if (mPlayerPossessed)
	{
		CM::Transition(mG->mPlayer->mOrbitCam);
	}
	else
	{
		CM::Transition(mG->mFreeCam);
	}
}

void GameSettings::SetFreeze(bool const value)
{
	mFreezed = value;
	if (mFreezed)
	{
		SetPlayerPossessed(false);
	}
}

void GameSettings::SetGame(Game& game)
{
	mG = &game;
}

void GameSettings::SetPlayerPossessed()
{
	SetPlayerPossessed(mPlayerPossessed);
}

namespace Engine
{
u32 GetWindowWidth()
{
	return GetPlatformSettings().windowWidth;
}
u32 GetWindowHeight()
{
	return GetPlatformSettings().windowHeight;
}
u32 GetResWidth()
{
	return GetPlatformSettings().resWidth;
}
u32 GetResHeight()
{
	return GetPlatformSettings().resHeight;
}
char const* GetWindowTitle()
{
	return GetPlatformSettings().windowTitle;
}
}