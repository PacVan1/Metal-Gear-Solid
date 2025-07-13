#include "common.h"
#include "scene.h"

void Scene::Tick(float const dt)
{
	mDynamicWorld.Tick(dt);
	for (Actor* a : mActors) a->Tick(dt);
	//for (shared_ptr<Actor>& a : mActors2) a->Tick(dt);
	for (ModelInstanceBase* a : mToRender) a->Tick(dt);
}

void Scene::Render() const
{
	if (GS.mWireFrame) mDynamicWorld.WireFrame();
	for (Actor* a : mActors) a->Render();
	//for (shared_ptr<Actor> const& a : mActors2) a->Render();
	for (ModelInstanceBase* a : mToRender) a->Render();
}

void Scene::Add(Actor& actor)
{
	Add(&actor);
}

void Scene::Add(Actor*)
{
	//mDynamicWorld.Add(actor->mCollider);
	//mActors.push_back(actor);
}

void Scene::Add(shared_ptr<Actor> actor)
{
	mActors2.push_back(actor);
}

void Scene::Add(ModelInstanceBase* model)
{
	mToRender.push_back(model);
}

void Scene::Remove(Soldier const&)
{
	//Soldiers::Singleton().mPool.Release(make_shared<Soldier>(&soldier));
	//mActors2.erase(std::remove_if(mActors2.begin(), mActors2.end(),
	//	[&soldier](shared_ptr<Actor> const& ptr) { return ptr.get() == &soldier; }),
	//	mActors2.end());
	//mDynamicWorld.Remove(soldier.mCollider);
}

namespace SceneManager
{
	vector<Scene*> gScenes;
	Scene* gActive;

	void Init()
	{
		gScenes.push_back(new Scene());
		Scene& scene = *gScenes[0];

		//scene.Add(make_shared<Terrain>("MGS_Brian/MGS_Brian.obj"));
		//scene.Add(new Terrain("MGS_Brian/MGS_Brian.obj"));
		//scene.Add(make_shared<Soldier>());
		//scene.Add(make_shared<Soldier>());

		PerlinData p;
		p.width = 100;
		p.height = 100;
		p.depth = 10;
		p.octaves = 4;
		p.cellSize = 50;
		p.cellSizeZ = 5;
		p.seed = 1000;
		ComputeHelperValues(p);
		FogInstance* fog = new FogInstance(new FogModel(p));
		scene.Add(fog);

		SetActive(scene);
	}

	void Init(Scene& scene)
	{
		gActive = &scene;
	}

	void SetActive(Scene& scene)
	{
		gActive = &scene;
	}

	Scene& GetActive()
	{
		return *gActive;
	}

	void Tick(float const dt)
	{
		if (!GS.IsFreezed()) gActive->Tick(dt);
	}

	void Render()
	{
		gActive->Render();
	}

	void Cleanup()
	{
		// MYTODO delete scenes
	}
}
