#pragma once

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"

class Collider;
class NewCollider;
class DynaCollider;
class RigidBody;
class Trigger;
class Camera;

enum filters : s32
{
	FILTERS_NONE		= 1 << 0,
	FILTERS_TERRAIN		= 1 << 1,
	FILTERS_PLAYER		= 1 << 2,
	FILTERS_ENEMY		= 1 << 3,
	FILTERS_PROJECTILE	= 1 << 4,
	FILTERS_CAMERA		= 1 << 5,
};

class MotionState : public btMotionState
{
private:
	Transform mTransform;

public:
						MotionState();
						MotionState(Transform const& transform);
	virtual void		getWorldTransform(btTransform& worldTransform) const override;
	virtual void		setWorldTransform(const btTransform& worldTransform) override;
	Transform const&	GetTransform() const;
	void				MakeChild(Transform& child);
	void				RemoveChild(Transform& child);
};

class MotionStateNode : public SGNode, public btMotionState
{
public:
						MotionStateNode();
	virtual void		getWorldTransform(btTransform& worldTransform) const override;
	virtual void		setWorldTransform(const btTransform& worldTransform) override;
};

class DynaWorld
{
public:
	btDefaultCollisionConfiguration*		mConfig;
	btCollisionDispatcher*					mDispatcher;
	btBroadphaseInterface*					mBroadphase;
	btSequentialImpulseConstraintSolver*	mSolver;
	btDiscreteDynamicsWorld*				mSimulation;
	vector<Collider*>						mColliders;
	vector<NewCollider*>					mNewColliders; // debug draw

	// couldn't remove jittery movement on my 
	// terrain so replaced the floor a plane
	btStaticPlaneShape*						mPlaneShape; 
	btRigidBody*							mPlaneRB;

public:
			DynaWorld();
			~DynaWorld();
	void	Tick(float const dt);
	void	WireFrame() const;
	void	Add(DynaCollider& collider);
	void	Add(RigidBody& rb);
	void	Remove(DynaCollider const& collider);
	void	Remove(RigidBody& rb);
};