#include "common.h"
#include "transform.h"

Transform::Transform() : 
	mPosition(0.0f), 
	mScale(1.0f), 
	mWorld(1.0f), 
	mLocal(1.0f), 
	mParent(nullptr)
{
	RefreshAll();
}

void Transform::TransformLocal(mat4 const& mat)
{
	mLocal *= mat;
	RefreshWorld();
}

void Transform::TransformWorld(mat4 const& mat)
{
	mWorld *= mat;
	for (Transform* child : mChildren) child->RefreshWorld();
}

void Transform::SetLocal(mat4 const& mat)
{
	mLocal = mat;
	RefreshWorld();
}

void Transform::Set(btTransform const& transform)
{
	transform.getOpenGLMatrix(glm::value_ptr(mWorld));
	RefreshWorld();
}

void Transform::Translate(vec3 const& position)
{
	mPosition = position;
	RefreshAll();
}

void Transform::Rotate(vec3 const& rotation)
{
	mRotation = quat(radians(rotation));
	RefreshAll();
}

void Transform::RotateLocal(quat const& rotation)
{
	mRotation = rotation;
	RefreshAll();
}

void Transform::Scale(vec3 const& scale)
{
	mScale = scale;
	RefreshAll();
}

void Transform::Reset()
{
	mPosition	= vec3(0.0f);
	mRotation	= vec3(0.0f);
	mRotation	= quat();
	mScale		= vec3(1.0f);
	RefreshAll();
}

void Transform::RefreshAll()
{
	RefreshLocal();
	RefreshWorld();
}

void Transform::RefreshLocal()
{
	mat4 const rotMat	= glm::toMat4(mRotation);
	mat4 const transMat = glm::translate(mat4(1.0f), mPosition);
	mat4 const scaleMat = glm::scale(mat4(1.0f), mScale);
	mLocal = transMat * rotMat * scaleMat;
}

void Transform::RefreshWorld()
{
	mWorld = mParent ? mParent->GetWorld() * mLocal : mLocal;
	for (Transform* child : mChildren) child->RefreshWorld();
}

void Transform::MakeChild(Transform& child)
{
	mChildren.push_back(&child);
	child.mParent = this;
}

void Transform::RemoveChild(Transform& child)
{
	auto it = std::find(mChildren.begin(), mChildren.end(), &child);
	if (it != mChildren.end()) {
		mChildren.erase(it);
		child.mParent = nullptr;
	}
}
