#pragma once

class Game;

struct GUIData
{
	OGLTexture2D				splash;
	//std::vector<OGLTexture2D>	invIcons;
};

class GUI
{
public:
	bool	mEnabled = true;

private:
	Game*	mG;
	GUIData mData;

public:
	GUI();
	GUI(Game* game);
	void	Tick();

private:
	void	Init();
	void	MainState();
	void	HUDState() const;
	void	MenuState();
	void	GameOverState();
};
