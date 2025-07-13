#pragma once

#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "physics.h"

// btCollisionShape wrapper for drawing debug lines
class ColliderMesh
{
public:
	btCollisionShape* mShape;
	btTriangleMesh* mTriangleMesh;

#ifdef DEBUG_MODE
private:
	GLuint	mVAO;
	GLuint	mVBO;
	GLuint	mEBO;
	u32		mVertexCount;
	u32		mIndexCount;
#endif

public:
	ColliderMesh() = default;
	ColliderMesh(ColliderMesh const& other);
	ColliderMesh(StaticModel const& model);
	ColliderMesh(btBvhTriangleMeshShape* shape);
	ColliderMesh(btCapsuleShape* shape);
	~ColliderMesh();
	void WireFrame(Transform const& transform, vec3 const& color) const;
	void WireFrame(mat4 const& transform, vec3 const& color) const;
	btBvhTriangleMeshShape* CreateTriangleCollisionShape(StaticModel const& model);
};

/// Bullet wrapper for a default collision object
class Collider
{
public:
	vec3				mDebugColor;
	ColliderMesh const* mMesh;
	MotionState*		mMotionState;

	s32	mGroup;
	s32	mMask;

public:
	void WireFrame() const;
	void MakeChild(Transform& transform);
	void RemoveChild(Transform& transform);

protected:
	Collider(ColliderMesh const* mesh);
	~Collider();
};

class NewCollider
{
public:
	ColliderMesh const*			mColliderMesh;
	shared_ptr<MotionStateNode> mMotionState;
	s32							mGroup;
	s32							mMask;

public:
	NewCollider(ColliderMesh const* mesh);
	~NewCollider();
	void WireFrame() const;

	inline shared_ptr<SGNode> GetSGNode() const { return mMotionState; }
};

float constexpr DYNA_COLLIDER_WEIGHTLESS = 0.0f;

/// Bullet wrapper for btRigidBody
class DynaCollider : public Collider
{
public:
	btRigidBody* mRB;

public:
	DynaCollider();
	DynaCollider(ColliderMesh const* mesh, float const mass = DYNA_COLLIDER_WEIGHTLESS);
	void Disable();
	void Enable();
	~DynaCollider();
};

float constexpr RIGIDBODY_WEIGHTLESS = 0.0f;

class RigidBody : public NewCollider
{
public:
	btRigidBody* mRB;

public:
	RigidBody();
	RigidBody(ColliderMesh const* mesh, float const mass = RIGIDBODY_WEIGHTLESS);
	~RigidBody();
};