#pragma once

namespace Inputs
{
	// game
	inline bool MoveRight()		{ return GetInput().IsKeyDown(KEYS_D); }
	inline bool MoveAhead()		{ return GetInput().IsKeyDown(KEYS_W); }
	inline bool MoveLeft()		{ return GetInput().IsKeyDown(KEYS_A); }
	inline bool MoveBack()		{ return GetInput().IsKeyDown(KEYS_S); }
	inline bool MoveUp()		{ return GetInput().IsKeyDown(KEYS_R); }
	inline bool MoveDown()		{ return GetInput().IsKeyDown(KEYS_F); }
	inline bool Fire()			{ return GetInput().IsMouseButtonReleased(MOUSE_LEFT); }
	inline bool Aim()			{ return GetInput().IsMouseButtonDown(MOUSE_RIGHT); }
	inline bool ToggleMenu()	{ return GetInput().IsKeyReleased(KEYS_TAB); }

	// engine
	inline bool QuitApp()		{ return GetInput().IsKeyReleased(KEYS_ESCAPE); }
	inline bool ToggleEditor()	{ return GetInput().IsKeyReleased(KEYS_E); }
}