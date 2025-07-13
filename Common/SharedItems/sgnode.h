#pragma once

// forward declaring Editor
class Editor;

// Copilot helped me with utilizing smart pointers in my structure.
// also partially inspired by Jacco's second lecture about scene graph.
class SGNode : public std::enable_shared_from_this<SGNode>
{
friend class Editor;
public:
	bool						mIsActive;
	bool						mIsVisible;

protected:
	vec3						mLocalPosition;
	quat						mLocalRotation;
	vec3						mLocalScale;
	mat4						mLocalTransform;
	mat4						mWorldTransform;
	weak_ptr<SGNode>			mParent;
	vector<shared_ptr<SGNode>>	mChildren;
	string_view					mName;
	bool						mIsDirty;

public:
						SGNode();
						SGNode(SGNode const& other);
	virtual				~SGNode();
	virtual void		SetLocalPosition(vec3 const& position);
	virtual void		SetLocalRotation(quat const& rotation);
	void				SetLocalScale(vec3 const& scale);
	void				AddToLocalPosition(vec3 const& offset);
	void				ResetLocalTransform();
	void				AddChild(shared_ptr<SGNode> node);
	void				RemoveChild(SGNode* node);
	void				TranslateLocal(vec3 const& offset);
	void				RotateLocal(quat const& offset);
	void				LookAt(vec3 const& target, vec3 const& up);
	void				SetName(string_view const& name);
	void				RefreshTransforms();

	// debugging
	void				PrintUseCount() const;
	void				PrintDestroyed() const;

	quat				GetWorldRotation() const;
	vec3				GetWorldScale() const;

	inline virtual vec3	GetLocalPosition() const	{ return mLocalPosition; }
	inline virtual quat	GetLocalRotation() const	{ return mLocalRotation; }
	inline vec3			GetLocalScale() const		{ return mLocalScale; }
	inline virtual mat4	GetLocalTransform() const	{ return mLocalTransform; }
	inline virtual vec3	GetLocalRight() const		{ return glm::normalize(vec3(mWorldTransform[0])); }
	inline virtual vec3	GetLocalUp() const			{ return glm::normalize(vec3(mWorldTransform[1])); }
	inline virtual vec3	GetLocalAhead() const		{ return glm::normalize(-vec3(mWorldTransform[2])); }
	inline virtual vec3	GetWorldPosition() const	{ return vec3(mWorldTransform[3]); }
	inline virtual mat4	GetWorldTransform() const	{ return mWorldTransform; }
	inline virtual vec3	GetWorldRight() const		{ return glm::normalize(vec3(mWorldTransform[0])); }
	inline virtual vec3	GetWorldUp() const			{ return glm::normalize(vec3(mWorldTransform[1])); }
	inline virtual vec3	GetWorldAhead() const		{ return glm::normalize(-vec3(mWorldTransform[2])); }
	inline string_view	GetName() const						{ return mName; }

	virtual void		Init() {}
	virtual void		Tick(float const dt);
	virtual void		Render() const;

	template <typename T>
	shared_ptr<T>		GetPtr()					{ return std::static_pointer_cast<T>(shared_from_this()); }

protected:
	void				ChildrenTick(float const dt);
	void				ChildrenRender() const;
};

// for construction and adding children at children at once
template <typename Node, typename... Args>
shared_ptr<Node> CreateSGNode(Args&&... args) 
{
	shared_ptr<Node> node = make_shared<Node>(std::forward<Args>(args)...);
	node->Init();
	return node;
}
