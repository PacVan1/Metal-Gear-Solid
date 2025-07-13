#include "common.h"
#include "scene_graph.h"

SceneGraph::SceneGraph()
{
	SetName("Root");
}

void SceneGraph::Tick(float const dt)
{
	if (GS.IsFreezed()) return;

	mDynaWorld.Tick(dt);
    RefreshTransforms();
    SGNode::Tick(dt);
}

void SceneGraph::Render() const
{
	SGNode::Render();
	if (GS.mWireFrame) mDynaWorld.WireFrame();
}

void SceneGraph::AddActor(shared_ptr<Actor> actor)
{
	AddChild(actor);
	mDynaWorld.Add(actor->mRB); 
}

void SceneGraph::RemoveActor(Actor* actor)
{
	mDynaWorld.Remove(actor->mRB);
	//RemoveChild(actor);
	actor->mIsActive = false;
	actor->mIsVisible = false;
}

shared_ptr<SGNode> CreateSGNode()
{
	return make_shared<SGNode>();
}

SceneManager2& SceneManager2::Singleton()
{
	static SceneManager2 singleton = SceneManager2();
	return singleton;
}

shared_ptr<SceneGraph> SceneManager2::GetActive()
{
	return SM().mActive;
}

DynaWorld const& SceneManager2::GetDynaWorld()
{
	return SM().mActive->mDynaWorld;
}

SceneManager2::SceneManager2()	
{
	mScenes.push_back(CreateSGNode<SceneGraph>());
	mActive = mScenes[0];
}
