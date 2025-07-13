#include "common.h"
#include "physics.h"

MotionState::MotionState() : 
    mTransform(Transform())
{}

MotionState::MotionState(Transform const& transform) :
	mTransform(transform)
{}

void MotionState::getWorldTransform(btTransform& worldTransform) const
{
    vec3 position, scale, skew;
    vec4 perspective;
    quat rotation;

    glm::decompose(mTransform.mWorld, scale, rotation, position, skew, perspective);
    worldTransform.setOrigin(ToBT(position));
    worldTransform.setRotation(ToBT(rotation));
}

void MotionState::setWorldTransform(const btTransform& worldTransform)
{
    worldTransform.getOpenGLMatrix(glm::value_ptr(mTransform.mWorld));
    for (Transform* child : mTransform.mChildren) child->RefreshWorld();
}

Transform const& MotionState::GetTransform() const
{
    return mTransform;
}

void MotionState::MakeChild(Transform& child)
{
    mTransform.MakeChild(child);
}

void MotionState::RemoveChild(Transform& child)
{
	mTransform.RemoveChild(child);
}

DynaWorld::DynaWorld()
{
	mConfig		= new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mConfig);
	mBroadphase = new btDbvtBroadphase();
	mSolver		= new btSequentialImpulseConstraintSolver();
	mSimulation = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mConfig);

	mSimulation->setGravity(btVector3(0.0f, -60.0f, 0.0f));

	// floor (reason is in the header file)
	mPlaneShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 130.0f);
	mPlaneRB	= new btRigidBody(0.0f, nullptr, mPlaneShape);
	mPlaneRB->setFriction(55.0f);
	mSimulation->addRigidBody(mPlaneRB, 
		FILTERS_TERRAIN, 
		FILTERS_PLAYER | 
		FILTERS_ENEMY | 
		FILTERS_PROJECTILE | 
		FILTERS_CAMERA);
}

DynaWorld::~DynaWorld()
{
	delete mSimulation;
	delete mSolver;
	delete mBroadphase;
	delete mDispatcher;
	delete mConfig;

	// using plane instead of terrain (reason in header file)
	delete mPlaneRB;
	delete mPlaneShape;
}

void DynaWorld::Tick(float const dt)
{
	mSimulation->stepSimulation(dt, 5, 1.0f / 120.0f);
}

void DynaWorld::WireFrame() const
{
#ifdef DEBUG_MODE
	for (NewCollider const* c : mNewColliders) c->WireFrame();
#endif
}

void DynaWorld::Add(DynaCollider& collider)
{
	mSimulation->addRigidBody(collider.mRB, collider.mGroup, collider.mMask);
	mColliders.push_back(&collider);
}

void DynaWorld::Add(RigidBody& rb)
{
	mSimulation->addRigidBody(rb.mRB, rb.mGroup, rb.mMask);
	mNewColliders.push_back(&rb);
}

void DynaWorld::Remove(DynaCollider const& collider)
{
	mSimulation->removeRigidBody(collider.mRB);
	mColliders.erase(std::remove(mColliders.begin(), mColliders.end(), &collider), mColliders.end());
}

void DynaWorld::Remove(RigidBody& rb)
{
	mSimulation->removeRigidBody(rb.mRB);
	mNewColliders.erase(std::remove(mNewColliders.begin(), mNewColliders.end(), &rb), mNewColliders.end());
}

MotionStateNode::MotionStateNode() : 
	SGNode()
{
	SetName("Motion State");

	RefreshTransforms();
}

void MotionStateNode::getWorldTransform(btTransform& worldTransform) const
{
	worldTransform.setOrigin(ToBT(mLocalPosition));
	worldTransform.setRotation(ToBT(mLocalRotation));
}

void MotionStateNode::setWorldTransform(const btTransform& worldTransform)
{
	mLocalPosition = ToGLM(worldTransform.getOrigin());
	mLocalRotation = ToGLM(worldTransform.getRotation());
	mIsDirty = true;
}
