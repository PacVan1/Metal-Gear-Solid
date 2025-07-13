#include "common.h"
#include "actors.h"

Character::Character() :
	Actor(&RM().GetColliderMesh("character_collider"), 30.0f)
{}

Character::Character(char const* name) :
	Actor(&RM().GetColliderMesh("character_collider"), 30.0f),
	mDisplay(CreateSGNode<SkeletalModelNode>(name))
{}

void Character::Init()
{
	Actor::Init();
	mRB.GetSGNode()->AddChild(mDisplay);
}

void Character::OnRevive()
{
	DebugLog(INFO, "Character revived!");
}

void Character::OnDeath()
{
	DebugLog(INFO, "Character died!");
	Engine::GetGame().GameOver();
}

void Character::Move(vec3 const& dir, float const dt)
{
	mRB.mRB->applyCentralImpulse(ToBT(dir * mMoveSpeed * dt));

	btVector3 velocity = mRB.mRB->getLinearVelocity();
	btScalar speed = velocity.length();
	btScalar maxSpeed = 135.0f;
	if (speed > maxSpeed) {
		velocity *= maxSpeed / speed;
		mRB.mRB->setLinearVelocity(velocity);
	}
}

void Character::LookAt(vec3 const& dir)
{
	quat test = glm::rotation(AHEAD, dir);
	mDisplay->SetLocalRotation(test);
}

Terrain::Terrain(char const* name) : 
	Actor(&RM().GetColliderMesh(name), RIGIDBODY_WEIGHTLESS),
	mDisplay(CreateSGNode<StaticModelNode>(name))
{
	SetName("Terrain");

	mRB.mGroup = FILTERS_TERRAIN;
	mRB.mMask = FILTERS_PLAYER | FILTERS_PROJECTILE | FILTERS_ENEMY | FILTERS_CAMERA;
}

void Terrain::Init()
{
	Actor::Init();
	mRB.GetSGNode()->AddChild(mDisplay);
}

void Terrain::Render() const
{
	mDisplay->Render();
}

Player::Player(shared_ptr<Inventory> inventory) :
	Character("Snake/SnakewAnims.wanim"), 
	mInventory(inventory)
{
	SetName("Player");

	mBehavior	= PLAYER_STATES_IDLING;
	mMoveSpeed	= 30000.0f;
	mDisplay->SetLocalScale(vec3(400.0f, 500.0f, 400.0f));
	mDisplay->TranslateLocal(vec3(0.0f, -40.0f, 0.0f));
	mRB.mRB->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	mRB.mRB->setActivationState(DISABLE_DEACTIVATION);

	mOrbitCam.SetPlayer(*this);
	mFPCam.SetPlayer(*this);
	CameraManager::SetActive(mOrbitCam);

	mRB.mGroup	= FILTERS_PLAYER;
	mRB.mMask	= FILTERS_TERRAIN | FILTERS_PROJECTILE;
}

void Player::Tick(float const dt)
{
	Character::Tick(dt);
	if (!GS.IsPlayerPossessed()) return;
	
	switch (mBehavior)
	{
	case PLAYER_STATES_IDLING:	IdlingState(dt);	break;
	case PLAYER_STATES_RUNNING:	RunningState(dt);	break;
	case PLAYER_STATES_AIMING:	AimingState(dt);	break;
	default: break;
	}
}

void Player::OnHeal(float const amount)
{
	DebugLog(INFO, "Player healed!");
}

void Player::OnDamage(float const dmg)
{
	DebugLog(INFO, "Player damaged!");
}

static float constexpr STAND_STILL_TOLERANCE = 0.0001f;

void Player::IdlingState(float const dt)
{
	// get horizontal direction vectors of the camera
	vec3 right = glm::normalize(glm::cross(mOrbitCam.GetAhead(), UP));
	vec3 ahead = mOrbitCam.GetAhead(); ahead.y = 0.0f;

	vec3 input = vec3(0.0f);
	if (Inputs::MoveAhead())	input += ahead;
	if (Inputs::MoveLeft())		input += -right;
	if (Inputs::MoveBack())		input += -ahead;
	if (Inputs::MoveRight())	input += right;

	// move in chosen direction
	if (glm::length(input) > STAND_STILL_TOLERANCE)
	{
		mBehavior = PLAYER_STATES_RUNNING;
		mDisplay->SetAnim(PLAYER_STATES_RUNNING);
		Move(glm::normalize(input), dt);
	}

	if (Inputs::Aim())
	{
		CM::Transition(mFPCam);
		mBehavior = PLAYER_STATES_AIMING;
		//mInventory->ShowEquiped();
	}
}

void Player::RunningState(float const dt)
{
	// get horizontal direction vectors of the camera
	vec3 right = glm::normalize(glm::cross(mOrbitCam.GetAhead(), UP));
	vec3 ahead = mOrbitCam.GetAhead(); ahead.y = 0.0f;

	vec3 vel = vec3(0.0f);
	if (Inputs::MoveAhead())	vel += ahead;
	if (Inputs::MoveLeft())		vel -= right;
	if (Inputs::MoveBack())		vel -= ahead;
	if (Inputs::MoveRight())	vel += right;

	// move in chosen direction
	if (glm::length(vel) > STAND_STILL_TOLERANCE)
		Move(glm::normalize(vel), dt);

	//if (glm::length(vel) > 0.0f) Move(glm::normalize(vel), dt);
	//else mRB.mRB->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));

	// what direction is the rigidbody moving in
	vec3 pos = GetWorldPosition();
	vel = ToGLM(mRB.mRB->getLinearVelocity());
	vel.y = 0.0f;

	// rotate character toward move direction
	if (glm::length(vel) > STAND_STILL_TOLERANCE)
	{
		LIGHTS.SetSpotPosition(vec3(pos.x, pos.y + 20.0f, pos.z), 0);
		LIGHTS.SetSpotDirection(vel, 0);
		LookAt(glm::normalize(vel)); 
	}
	else
	{
		mDisplay->SetAnim(PLAYER_STATES_IDLING);
		mBehavior = PLAYER_STATES_IDLING;
	}

	if (Inputs::Aim())
	{
		CM::Transition(mFPCam);
		mBehavior = PLAYER_STATES_AIMING;
		//mInventory->ShowEquiped();
	}
}

void Player::AimingState(float const dt)
{
	if (!Inputs::Aim())
	{
		CM::Transition(mOrbitCam);
		mBehavior = PLAYER_STATES_IDLING;
		//mInventory->HideEquiped();
	}

	// get horizontal direction vectors of the camera
	vec3 right = glm::normalize(glm::cross(mFPCam.GetAhead(), UP));
	vec3 ahead = mFPCam.GetAhead(); ahead.y = 0.0f;

	//if (Inputs::Fire()) mInventory->Use();

	vec3 vel = vec3(0.0f);
	if (Inputs::MoveAhead())	vel += ahead;
	if (Inputs::MoveLeft())		vel -= right;
	if (Inputs::MoveBack())		vel -= ahead;
	if (Inputs::MoveRight())	vel += right;

	// move in chosen direction
	if (glm::length(vel) > STAND_STILL_TOLERANCE)
		Move(glm::normalize(vel), dt);

	// what direction is the rigidbody moving in
	vec3 pos = GetWorldPosition();
	vel = ToGLM(mRB.mRB->getLinearVelocity());
	vel.y = 0.0f;

	LIGHTS.SetSpotPosition(vec3(pos.x, pos.y + 20.0f, pos.z), 0);
	LIGHTS.SetSpotDirection(ahead, 0);
	LookAt(normalize(ahead));
}

Actor::Actor(ColliderMesh const* mesh, float const mass) : 
	mRB(mesh, mass)
{
	mRB.mRB->setUserPointer(this);
}

void Actor::Init()
{
	//mRB.GetSGNode()->AddChild(shared_from_this());
	AddChild(mRB.GetSGNode());
}

void Actor::SetLocalPosition(vec3 const& position)
{
	btTransform transform = mRB.mRB->getWorldTransform();
	transform.setOrigin(ToBT(position));
	mRB.mRB->setWorldTransform(transform);
	mRB.mRB->getMotionState()->setWorldTransform(transform);
}

void Actor::SetLocalRotation(quat const& rotation)
{
	btTransform transform = mRB.mRB->getWorldTransform();
	transform.setRotation(ToBT(rotation)); // Convert glm::quat to btQuaternion
	mRB.mRB->setWorldTransform(transform);
	mRB.mRB->getMotionState()->setWorldTransform(transform);
}

Soldier::Soldier() : 
	Character("Walking/WalkingwAnims.wanim")
{
	SetName("Soldier");

	mDisplay->TranslateLocal(vec3(0.0f, -40.0f, 0.0f));
	mDisplay->SetLocalScale(vec3(50.0f));

	mRB.mRB->setFriction(12.0f);
	mMoveSpeed = 20000.0f;

	mRB.mGroup	= FILTERS_ENEMY;
	mRB.mMask	= FILTERS_TERRAIN | FILTERS_PROJECTILE;
}

void Soldier::Tick(float const dt)
{
	if (!IsDead()) Character::Tick(dt);

	RunningState(dt);
}

void Soldier::Render() const
{
	if (!IsDead()) Character::Render();
}

void Soldier::OnHeal(float const amount)
{
	DebugLog(INFO, "Soldier healed!");
}

void Soldier::OnDamage(float const dmg)
{
	DebugLog(INFO, ("Soldier hit for: " + std::to_string(dmg) + "dmg").c_str());
}

void Soldier::OnDeath()
{
	DebugLog(INFO, "Soldier died!");
	//SM::GetActive().Remove(*this);
}

void Soldier::RunningState(float const dt)
{
	auto& node = mDisplay;
	vec3 rayDir1 = normalize(normalize(node->GetWorldAhead()) + normalize(-node->GetWorldRight()));
	vec3 rayDir2 = normalize(node->GetWorldAhead());
	vec3 rayDir3 = normalize(normalize(node->GetWorldAhead()) + normalize(node->GetWorldRight()));
	vec3 from = node->GetWorldPosition();

	using namespace RayCaster;
	HitResult res1 = Cast(from, from + rayDir1 * 150.0f);
	HitResult res2 = Cast(from, from + rayDir2 * 150.0f);
	HitResult res3 = Cast(from, from + rayDir3 * 150.0f);

	float w1 = !res1.hit ? 1.0f : glm::length(res1.point - from) / 150.0f;
	float w2 = !res2.hit ? 1.0f : glm::length(res2.point - from) / 150.0f;
	float w3 = !res3.hit ? 1.0f : glm::length(res3.point - from) / 150.0f;

	vec3 playerDir = glm::normalize(Engine::GetGame().GetPlayerPosition() - from);

	vec3 finalDir = rayDir1 * w1 + rayDir2 * w2 + rayDir3 * w3 + playerDir * 12.0f;
	finalDir.y = 0.0f;
	finalDir = glm::normalize(finalDir);

	// move in chosen direction
	if (glm::length(finalDir) > STAND_STILL_TOLERANCE)
		Move(glm::normalize(finalDir), dt);

	// what direction is the rigidbody moving in
	vec3 pos = ToGLM(mRB.mRB->getWorldTransform().getOrigin());
	vec3 vel = ToGLM(mRB.mRB->getLinearVelocity());
	vel.y = 0.0f;

	// rotate character toward move direction
	if (glm::length(vel) > STAND_STILL_TOLERANCE)
	{
		LookAt(glm::normalize(vel));
	}
}
