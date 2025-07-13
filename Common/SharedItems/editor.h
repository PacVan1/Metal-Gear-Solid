#pragma once

class Transform;
class Camera;
class OrbitalCamera;
class Game;
class FogInstance;
class Player;
class GameSettings;
class Lights;
class SGNode;

class Editor
{
private:
	Game*	mG;
	bool	mShow;

public:
			Editor(); 
			Editor(Game* game);
	void	Toggle();
	void	Tick() const;

private:
	void	Edit(Camera& camera) const;
	void	Edit(OrbitalCamera& camera) const;
	void	Edit(Transform& transform) const;
	void	Edit(FogInstance& fog) const;
	void	Edit(FogNode& fog) const;
	void	Edit(Player& player) const;
	void	Edit(Lights& lights) const;
	void	EditPointLight(Lights& lights, u8 const idx) const;
	void	EditSpotlight(Lights& lights, u8 const idx) const;
	void	Edit(shared_ptr<SGNode> const& node) const;

	void	Globals() const;
};
