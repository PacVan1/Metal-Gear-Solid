#pragma once

enum mouse : s32
{
	MOUSE_LEFT,	
	MOUSE_RIGHT,
	MOUSE_MIDDLE
};

enum keys : s32
{
	KEYS_A, KEYS_B, KEYS_C, KEYS_D,
	KEYS_E, KEYS_F, KEYS_G, KEYS_H,
	KEYS_I, KEYS_J, KEYS_K, KEYS_L,
	KEYS_M, KEYS_N, KEYS_O, KEYS_P,
	KEYS_Q, KEYS_R, KEYS_S, KEYS_T,
	KEYS_U, KEYS_V, KEYS_W, KEYS_X,
	KEYS_Y, KEYS_Z, KEYS_0, KEYS_1,
	KEYS_2, KEYS_3, KEYS_4, KEYS_5, 
	KEYS_6, KEYS_7, KEYS_8, KEYS_9,
	KEYS_TAB,
	KEYS_CAPS_LOCK,
	KEYS_LEFT_SHIFT,
	KEYS_LEFT_CTRL,
	KEYS_LEFT_ALT,
	KEYS_ESCAPE,
	KEYS_RIGHT_SHIFT,
	KEYS_ENTER,
	KEYS_UP,
	KEYS_RIGHT,
	KEYS_DOWN,
	KEYS_LEFT,
	KEYS_SPACE,
	KEYS_COUNT
};

auto constexpr SYSTEM_KEY_COUNT = 256;

class IInput
{
public:
	s32			mKeyMapping[KEYS_COUNT];
	bool		mKeyStates[SYSTEM_KEY_COUNT];
	bool		mPrevKeyStates[SYSTEM_KEY_COUNT];
	bool		mButtonStates[3];
	bool		mPrevButtonStates[3];
	bool		mShouldUpdate;
	ivec2		mPrevMousePos;
	ivec2		mMousePos;
	ivec2		mMouseDeltaPos;

public:
	IInput() :
		mKeyStates{ false }, 
		mPrevKeyStates{ false }, 
		mShouldUpdate(false)
	{}
	void UpdatePrevious()
	{
		if (mShouldUpdate)
		{
			memcpy(mPrevKeyStates, mKeyStates, SYSTEM_KEY_COUNT * sizeof(bool));
			mShouldUpdate = false;
		}
		memcpy(mPrevButtonStates, mButtonStates, 3 * sizeof(bool));
	}
	virtual void InitKeyMapping() = 0;
	bool IsKeyUp(u8 const key) const
	{
		return !mKeyStates[mKeyMapping[key] & (SYSTEM_KEY_COUNT - 1)];
	}
	bool IsKeyDown(u8 const key) const
	{
		return mKeyStates[mKeyMapping[key] & (SYSTEM_KEY_COUNT - 1)];
	}
	bool IsKeyReleased(u8 const key) const
	{
		return IsKeyUp(key) && mPrevKeyStates[mKeyMapping[key] & (SYSTEM_KEY_COUNT - 1)];
	}
	bool IsMouseButtonUp(u8 const button) const
	{
		return !mButtonStates[button];
	}

	bool IsMouseButtonDown(u8 const button) const
	{
		return mButtonStates[button];
	}

	bool IsMouseButtonReleased(u8 const button) const
	{
		return IsMouseButtonUp(button) && mPrevButtonStates[button];
	}
	virtual float		GetDeltaScroll() const = 0;
	virtual ivec2		GetMousePos() const = 0; 
	virtual ivec2		GetMouseDeltaPos() const = 0; 
};

IInput& GetInput();



