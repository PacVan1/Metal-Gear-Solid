#pragma once

class Scene
{
public:
	DynaWorld					mDynamicWorld;
	vector<Actor*>				mActors;
	vector<shared_ptr<Actor>>	mActors2;
	vector<ModelInstanceBase*>	mToRender;

public:
			Scene() = default;
	void	Tick(float const dt);
	void	Render() const;
	void	Add(Actor& actor);
	void	Add(Actor* actor);
	void	Add(shared_ptr<Actor> actor);
	void	Add(ModelInstanceBase* model);
	void	Remove(Soldier const& soldier);

	inline btDiscreteDynamicsWorld const* GetSimulation() const { return mDynamicWorld.mSimulation; }
};

namespace SceneManager
{
	void	Init(); 
	void	Init(Scene& scene);
	void	SetActive(Scene& scene);
	Scene&	GetActive();
	void	Tick(float const dt);
	void	Render();
	void	Cleanup();
}