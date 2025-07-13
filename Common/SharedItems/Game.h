#pragma once

enum gameStates : u8
{
	GAME_STATES_OPENING,
	GAME_STATES_PLAYING,
	GAME_STATES_MENU,
	GAME_STATES_PAUSED,
	GAME_STATES_GAME_OVER,
	GAME_STATES_COUNT,
};

class GameOpeningState : public IState<Game>
{
public:
	virtual void Enter(Game& g) override;
	virtual void Exit(Game& g) override;
	virtual void RenderGUI(Game& g) override;
};

class GamePlayingState : public IState<Game>
{
public:
	virtual void Enter(Game& g) override;
	virtual void Tick(Game& g, float const dt) override;
	virtual void RenderGUI(Game& g) override;
};

class GameMenuState : public IState<Game>
{
public:
	virtual void Enter(Game& g) override;
	virtual void Exit(Game& g) override;
	virtual void Tick(Game& g, float const dt) override;
	virtual void RenderGUI(Game& g) override;
};

class GamePauseState : public IState<Game>
{
};

class GameOverState : public IState<Game>
{
public:
	virtual void Enter(Game& g) override;
};

class Game
{
friend class GameSettings;
friend class Editor;
friend class GUI;
friend class GameOpeningState;
friend class GamePlayingState;
friend class GameMenuState;
friend class GamePauseState;
friend class GameOverState;
public:
	GUI						mGUI;

private:
	shared_ptr<Player> 		mPlayer;
	shared_ptr<Soldier>		mSoldier;
	shared_ptr<Inventory>	mInventory;
	shared_ptr<FogNode>		mFog; // for the editor

	u8						mPrevState;
	u8						mState;

	Skybox const*			mSkybox;

	FreeCamera				mFreeCam;
	SpinningCamera			mGameOverCam;

	StateMachine<Game>		mSM;
	GameOpeningState		mOpeningState;
	GamePlayingState		mPlayingState;
	GameMenuState			mMenuState;
	GamePauseState			mPauseState;
	GameOverState			mOverState;

public:
			Game();
			~Game();
	void	Tick(float const dt);
	void	RenderGUI();
	void	Restart();
	void	GameOver();

	inline vec3 GetPlayerPosition() { return mPlayer->GetWorldPosition(); }
	inline u8	GetGameState()		{ return mState; }
	inline void ToggleMenu()		{ mState = mState == GAME_STATES_MENU ? GAME_STATES_PLAYING : GAME_STATES_MENU; }
};