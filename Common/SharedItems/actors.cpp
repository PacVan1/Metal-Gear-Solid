#include "common.h"
#include "actors.h"

Character::Character() :
	Actor(&RM().GetColliderMesh("character_collider"), 30.0f)
{
	mRB.mRB->setActivationState(DISABLE_DEACTIVATION);
}

Character::Character(char const* name) :
	Actor(&RM().GetColliderMesh("character_collider"), 30.0f),
	mDisplay(CreateSGNode<SkeletalModelNode>(name)), 
	mFirearmPoint(CreateSGNode<SGNode>())
{
	mRB.mRB->setActivationState(DISABLE_DEACTIVATION);
}

void Character::Init()
{
	Actor::Init();
	mRB.GetSGNode()->AddChild(mDisplay);
	mRB.GetSGNode()->AddChild(mFirearmPoint);
	mFirearmPoint->SetName("Firearm Point");
}

void Character::OnRevive()
{
	DebugLog(INFO, "Character revived!");
}

void Character::OnDeath()
{
	DebugLog(INFO, "Character died!");
}

void Character::Move(vec3 const& dir, float const dt)
{
	mRB.mRB->applyCentralImpulse(ToBT(dir * mMoveForce * dt));

	btVector3 velocity = mRB.mRB->getLinearVelocity();
	btScalar speed = velocity.length();
	if (speed > mMaxMoveSpeed) 
	{
		velocity *= mMaxMoveSpeed / speed;
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

	mBehavior		= PLAYER_STATES_IDLING;
	mMoveForce		= 30000.0f;
	mMaxMoveSpeed	= 135.0f;
	mDisplay->SetLocalScale(vec3(400.0f, 500.0f, 400.0f));
	mDisplay->TranslateLocal(vec3(0.0f, -40.0f, 0.0f));
	mRB.mRB->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	mFirearmPoint->SetLocalPosition(vec3(0.0f, 19.0f, 0.0f));

	mRB.mRB->setActivationState(DISABLE_DEACTIVATION);

	mOrbitCam.SetPlayer(*this);
	mFPCam.SetPlayer(*this);
	CameraManager::SetActive(mOrbitCam);

	mRB.mGroup	= FILTERS_PLAYER;
	mRB.mMask	= FILTERS_TERRAIN | FILTERS_PROJECTILE | FILTERS_ENEMY;

	mInventory->Equip(0);
	mFirearmPoint->AddChild(mInventory->mEquiped);
}

void Player::Tick(float const dt)
{
	if (!mIsActive || IsDead()) return;

	if (!GS.IsPlayerPossessed()) return;
	
	switch (mBehavior)
	{
	case PLAYER_STATES_IDLING:	IdlingState(dt);	break;
	case PLAYER_STATES_RUNNING:	RunningState(dt);	break;
	case PLAYER_STATES_AIMING:	AimingState(dt);	break;
	default: break;
	}

	ChildrenTick(dt);
}

void Player::OnHeal(float const)
{
	DebugLog(INFO, "Player healed!");
}

void Player::OnDamage(float const)
{
	DebugLog(INFO, "Player damaged!");
}

void Player::OnDeath()
{
	Engine::GetGame().GameOver();
	SM().GetActive()->RemoveActor(this);
	mIsActive = false;
}

void Player::LookAt(vec3 const& dir)
{
	if (mBehavior != PLAYER_STATES_AIMING)
		Character::LookAt(dir);
	else
	{
		quat test = glm::rotation(AHEAD, dir);
		mInventory->mEquiped->SetLocalRotation(test);
	}
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
		mInventory->ShowEquiped();
		mDisplay->mIsVisible = false;
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
		//LIGHTS.SetSpotPosition(vec3(pos.x, pos.y + 20.0f, pos.z), 0);
		//LIGHTS.SetSpotDirection(vel, 0);
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
		mInventory->ShowEquiped();
		mDisplay->mIsVisible = false;
	}
}

void Player::AimingState(float const dt)
{
	if (!Inputs::Aim())
	{
		CM::Transition(mOrbitCam);
		mBehavior = PLAYER_STATES_IDLING;
		mInventory->HideEquiped();
		mDisplay->mIsVisible = true;
	}

	// get horizontal direction vectors of the camera
	vec3 right = glm::normalize(glm::cross(mFPCam.GetAhead(), UP));
	vec3 ahead = mFPCam.GetAhead(); ahead.y = 0.0f;

	if (Inputs::Fire()) mInventory->Use();

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

	//LIGHTS.SetSpotPosition(vec3(pos.x, pos.y + 20.0f, pos.z), 0);
	//LIGHTS.SetSpotDirection(ahead, 0);
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

static u8 constexpr		DESIRE_DIR_COUNT = 8;
static vec3 constexpr	DESIRE_DIRS[DESIRE_DIR_COUNT] =
{
	{ 1.000f, 0.000f, 0.000f },
	{ 0.707f, 0.000f, 0.707f },
	{ 0.000f, 0.000f, 1.000f },
	{-0.707f, 0.000f, 0.707f },
	{-1.000f, 0.000f, 0.000f },
	{-0.707f, 0.000f,-0.707f },
	{ 0.000f, 0.000f,-1.000f },
	{ 0.707f, 0.000f,-0.707f }
};

Soldier::Soldier() :
	Character("camo_scout/camo_scout.wanim"), 
	mPathFollower(""), 
	mFireStandStillTimer(1.5f), 
	mFireStandStillTime(0.0f), 
	mFireTime(0.0f)
{
	SetName("Soldier");

	mFirearm = CreateSGNode<RCFirearm>(CreateSGNode<StaticModelNode>("Vepr_Hunter3/vepr_hunter.dae"), 1300.0f, 600.0f, 600.0f);
	mFirearm->SetGroupAndMask(FILTERS_PROJECTILE, FILTERS_PLAYER | FILTERS_TERRAIN);
	mFirearm->SetName("RC Firearm");
	mDisplay->AddChild(mFirearm);
	mFirearm->SetLocalScale(vec3(6.0f / 4500.0f, 6.0f / 5300.0f, 6.0f / 4500.0f));
	mFirearm->SetLocalPosition(vec3(-5.2f / 4500.0f, 69.0f / 5300.0f, 12.0f / 4500.0f));

	mDisplay->SetLocalScale(vec3(4500.0f, 5300.0f, 4500.0f));
	mDisplay->TranslateLocal(vec3(0.0f, -40.0f, 0.0f));
	mRB.mRB->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	mBehavior = SOLDIER_STATES_PATROLLING;

	mMoveForce			= 22500.0f;
	mMaxMoveSpeed		= 90.0f;
	mTargetDesire		= 5.0f;
	mVisionRange		= 70.0f; 
	mTargetVisionRange	= 150.0f;

	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(3.0f, 5.0f);
	mFireTimer = dist(rng);

	mRB.mGroup	= FILTERS_ENEMY;
	mRB.mMask	= FILTERS_TERRAIN | FILTERS_PROJECTILE | FILTERS_PLAYER;
}

void Soldier::Tick(float const dt)
{
	if (!mIsActive || IsDead()) return;

	Character::Tick(dt);
	switch (mBehavior)
	{
	case SOLDIER_STATES_PATROLLING: PatrollingState(dt);	break;
	case SOLDIER_STATES_PURSUING:	PursuingState(dt);		break;
	case SOLDIER_STATES_FIRING:		FiringState(dt);		break;
	}

	ChildrenTick(dt);
}

void Soldier::Render() const
{
	if (!mIsVisible) return;
	if (!IsDead())
	{
		Character::Render();
		ChildrenRender();
	}
}

void Soldier::ToStartPosition()
{
	mPathFollower.Reset();
	vec2 const checkpoint = mPathFollower.GetCurrent();
	SetLocalPosition(vec3(checkpoint.x, 200.0f, checkpoint.y));
}

void Soldier::OnHeal(float const)
{
	DebugLog(INFO, "Soldier healed!");
}

void Soldier::OnDamage(float const dmg)
{
	DebugLog(INFO, ("Soldier hit for: " + std::to_string(dmg) + "dmg").c_str());
	Soldiers::Singleton().Detect();
}

void Soldier::OnDeath()
{
	DebugLog(INFO, "Soldier died!");
	SM().GetActive()->RemoveActor(this);
}

void Soldier::FollowTarget(float const dt, vec3 const& target)
{
	using namespace RayCaster;

	if (glm::length(target - mDisplay->GetWorldPosition()) < STAND_STILL_TOLERANCE) return;

	vec3 from = mDisplay->GetWorldPosition();
	vec3 targetDir = glm::normalize(target - from);

	vec3 finalDir = vec3(0.0f);

	// add obstacle desire
	for (u8 i = 0; i < DESIRE_DIR_COUNT; ++i)
	{
		HitResult const res = Cast(from, from + DESIRE_DIRS[i] * mVisionRange, FILTERS_ENEMY, FILTERS_TERRAIN);
		float const w = !res.hit ? 1.0f : glm::length(res.point - from) / mVisionRange;
		finalDir += DESIRE_DIRS[i] * w;
	}

	// add target desire
	HitResult targetRes = Cast(from, from + targetDir * mTargetVisionRange, FILTERS_ENEMY, FILTERS_TERRAIN);
	float playerWeight = !targetRes.hit ? 1.0f : glm::length(targetRes.point - from) / mTargetVisionRange;
	finalDir += targetDir * playerWeight * mTargetDesire;

	// compute final direction
	finalDir.y = 0.0f;
	finalDir = glm::normalize(finalDir);

	// move in final direction
	if (glm::length(finalDir) > STAND_STILL_TOLERANCE)
		Move(glm::normalize(finalDir), dt);

	// rotate character toward move direction
	vec3 vel = GetVelocity();
	vel.y = 0.0f;
	if (glm::length(vel) > STAND_STILL_TOLERANCE)
		LookAt(glm::normalize(vel));
}

void Soldier::Detected()
{
	mBehavior = SOLDIER_STATES_PURSUING;
}

void Soldier::PursuingState(float const dt)
{
	FollowTarget(dt, Engine::GetGame().GetPlayerPosition());

	mFireTime += dt;
	if (mFireTime >= mFireTimer)
	{
		static std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(3.0f, 5.0f);
		mBehavior = SOLDIER_STATES_FIRING;
		mFireTimer = dist(rng);
		mFireTime = 0.0f;
	}
}

void Soldier::FiringState(float const dt)
{
	mFireStandStillTime += dt;
	if (mFireStandStillTime >= mFireStandStillTimer)
	{
		mFirearm->Use();
		mBehavior = SOLDIER_STATES_PURSUING;
		mFireStandStillTime = 0.0f;
	}
}

void Soldier::PatrollingState(float const dt)
{
	// random ray direction to detect the player
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	vec3 const dir = normalize(mDisplay->GetLocalAhead() + mDisplay->GetLocalRight() * dist(rng));
	
	using namespace RayCaster;
	HitResult const res = Cast(mDisplay->GetWorldPosition() + vec3(0.0f, 70.0f, 0.0f), mDisplay->GetWorldPosition() + vec3(0.0f, 70.0f, 0.0f) - dir * mTargetVisionRange, FILTERS_ENEMY, FILTERS_PLAYER, false);
	if (res.hit && dynamic_cast<Player*>(res.actor))
	{
		// saw player
		mBehavior = SOLDIER_STATES_PURSUING;
		Soldiers::Singleton().Detect();
	}
	else
	{
		vec2 const checkpoint = mPathFollower.GetCurrent();
		FollowTarget(dt, vec3(checkpoint.x, mDisplay->GetWorldPosition().y, checkpoint.y));
		mPathFollower.TryNext(vec2(mDisplay->GetWorldPosition().x, mDisplay->GetWorldPosition().z));
	}
}

Soldiers& Soldiers::Singleton()
{
	static Soldiers singleton;
	return singleton;
}

Soldiers::Soldiers()
{
	mUnits.resize(3);
	
	for (shared_ptr<Soldier>& unit : mUnits)
		unit = CreateSGNode<Soldier>();
	
	// initialize paths
	mUnits[0]->mPathFollower.mCheckpoints.resize(4);
	mUnits[0]->mPathFollower.mCheckpoints[0] = vec2(-468.0f, -358.0f);
	mUnits[0]->mPathFollower.mCheckpoints[1] = vec2(-468.0f, -134.0f);
	mUnits[0]->mPathFollower.mCheckpoints[2] = vec2(-175.0f, -134.0f);
	mUnits[0]->mPathFollower.mCheckpoints[3] = vec2(-175.0f, -358.0f);
	
	mUnits[1]->mPathFollower.mCheckpoints.resize(4);
	mUnits[1]->mPathFollower.mCheckpoints[0] = vec2(-468.0f, -75.0f);
	mUnits[1]->mPathFollower.mCheckpoints[1] = vec2(-468.0f, 165.0f);
	mUnits[1]->mPathFollower.mCheckpoints[2] = vec2(-175.0f, 165.0f);
	mUnits[1]->mPathFollower.mCheckpoints[3] = vec2(-175.0f, -75.0f);

	mUnits[2]->mPathFollower.mCheckpoints.resize(9);
	mUnits[2]->mPathFollower.mCheckpoints[0] = vec2(-0775.0f,-140.0f);
	mUnits[2]->mPathFollower.mCheckpoints[1] = vec2(-1000.0f,-130.0f);
	mUnits[2]->mPathFollower.mCheckpoints[2] = vec2(-1175.0f,-125.0f);
	mUnits[2]->mPathFollower.mCheckpoints[3] = vec2(-1175.0f,-350.0f);
	mUnits[2]->mPathFollower.mCheckpoints[4] = vec2(-0880.0f,-350.0f);
	mUnits[2]->mPathFollower.mCheckpoints[5] = vec2(-0880.0f,-220.0f);
	mUnits[2]->mPathFollower.mCheckpoints[6] = vec2(-0990.0f,-220.0f);
	mUnits[2]->mPathFollower.mCheckpoints[7] = vec2(-1000.0f, 160.0f);
	mUnits[2]->mPathFollower.mCheckpoints[8] = vec2(-0775.0f, 160.0f);

	// add to scene
	for (shared_ptr<Soldier> unit : mUnits)
	{
		unit->ToStartPosition();
		SM().GetActive()->AddActor(unit);
	}
}

void Soldiers::ToStartPosition()
{
	// assuming the paths are already set
	for (shared_ptr<Soldier> unit : mUnits)
		unit->ToStartPosition();
}

void Soldiers::Detect()
{
	LIGHTS.ActivateAlarm();
	for (auto unit : Soldiers::Singleton().mUnits)
		if (!unit->IsDead()) unit->Detected();
}
