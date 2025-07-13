#pragma once

class MotionState;
class Editor;

class Transform
{
friend class MotionState;
friend class Editor;
public:
	vec3					mPosition;
	quat					mRotation;
	vec3					mScale;

private:
	mat4					mWorld;
	mat4					mLocal;

	Transform const*		mParent;
	std::vector<Transform*> mChildren;

public:
	Transform();

	void TransformLocal(mat4 const& mat);
	void TransformWorld(mat4 const& mat);
	void SetLocal(mat4 const& mat);
	void Set(btTransform const& transform);
	void Translate(vec3 const& position);
	void Rotate(vec3 const& rotation);
	void RotateLocal(quat const& rotation);
	void Scale(vec3 const& scale);
	void MakeChild(Transform& child);
	void RemoveChild(Transform& child);
	void RefreshAll();
	void RefreshLocal();
	void RefreshWorld();

	inline vec3 const&	GetPosition() const			{ return mPosition; }
	inline quat const&	GetRotation() const			{ return mRotation; }
	inline vec3 const&	GetScale() const			{ return mScale; }
	inline mat4 const&	GetWorld() const			{ return mWorld; }
	inline mat4 const&	GetLocal() const			{ return mLocal; }
	inline vec3			GetRight() const			{ return vec3(mWorld[0]); }
	inline vec3			GetUp() const				{ return vec3(mWorld[1]); }
	inline vec3			GetAhead() const			{ return vec3(mWorld[2]); }
	inline vec3			GetWorldPosition() const	{ return vec3(mWorld[3]);}

private:
	void Reset(); 
};
