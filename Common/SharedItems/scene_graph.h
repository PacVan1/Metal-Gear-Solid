#pragma once

class SceneGraph : public SGNode
{
friend class Editor;
public:
	DynaWorld mDynaWorld;

public:
	SceneGraph();
	virtual void Tick(float const dt) override;
	virtual void Render() const override;
	void AddActor(shared_ptr<Actor>);
	void RemoveActor(Actor* actor);
};

class SceneManager2
{
public:
	vector<shared_ptr<SceneGraph>>	mScenes;
	shared_ptr<SceneGraph>			mActive;

public:
	static SceneManager2& Singleton();
	static shared_ptr<SceneGraph> GetActive();
	static DynaWorld const& GetDynaWorld();

public:
	SceneManager2();
	inline void Tick(float const dt) const				{ if (mActive) mActive->Tick(dt); }
	inline void Render() const							{ if (mActive) mActive->Render(); }
	inline void AddActor(shared_ptr<Actor> actor) const { if (mActive) mActive->AddActor(actor); }
	inline void AddChild(shared_ptr<SGNode> node) const { if (mActive) mActive->AddChild(node); }
};

inline SceneManager2& SM() { return SceneManager2::Singleton(); }