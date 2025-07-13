#pragma once

class Actor : public SGNode
{
public:
	RigidBody mRB;

public:
						Actor(ColliderMesh const* mesh, float const mass);
	virtual void		Init() override;
	virtual void		SetLocalPosition(vec3 const& position) override;
	virtual void		SetLocalRotation(quat const& rotation) override;

	inline virtual vec3			GetLocalPosition() const override	{ return mRB.GetSGNode()->GetLocalPosition(); }
	inline virtual quat			GetLocalRotation() const override	{ return mRB.GetSGNode()->GetLocalRotation(); }
	inline virtual mat4			GetLocalTransform() const override	{ return mRB.GetSGNode()->GetLocalTransform(); }
	inline virtual vec3			GetLocalRight() const override		{ return mRB.GetSGNode()->GetLocalRight(); }
	inline virtual vec3			GetLocalUp() const override			{ return mRB.GetSGNode()->GetLocalUp(); }
	inline virtual vec3			GetLocalAhead() const override		{ return mRB.GetSGNode()->GetLocalAhead(); }
	inline virtual vec3			GetWorldPosition() const override	{ return mRB.GetSGNode()->GetWorldPosition(); }
	inline virtual mat4			GetWorldTransform() const override	{ return mRB.GetSGNode()->GetWorldTransform(); }
	inline virtual vec3			GetWorldRight() const override		{ return mRB.GetSGNode()->GetWorldRight(); }
	inline virtual vec3			GetWorldUp() const override			{ return mRB.GetSGNode()->GetWorldUp(); }
	inline virtual vec3			GetWorldAhead() const override		{ return mRB.GetSGNode()->GetWorldAhead(); }
	inline vec3					GetVelocity() const					{ return ToGLM(mRB.mRB->getLinearVelocity()); }
	inline shared_ptr<SGNode>	GetMotionStateNode() const			{ return mRB.GetSGNode(); }
};

class Character : public Actor, public IDamageable
{
protected:
	shared_ptr<SkeletalModelNode>	mDisplay;		// holds model render data
	vec3							mFacing;
	float							mMoveForce;		// speed at which the character tries to move
	float							mMaxMoveSpeed;	
	u32								mBehavior;		// behavior state in which the character is
	shared_ptr<SGNode>				mFirearmPoint;

public:
					Character();
					Character(char const* name);
	virtual void	Init() override;
	virtual void	OnRevive() override;
	virtual void	OnDeath() override;

	inline u32		GetBehavior() const { return mBehavior; }

protected:
	void			Move(vec3 const& direction, float const dt);
	virtual void	LookAt(vec3 const& direction);
};

class Terrain : public Actor
{
public:
	shared_ptr<StaticModelNode> mDisplay;

public:
	Terrain(char const* name);
	virtual void Init() override;
	virtual void Render() const override;
};

enum playerStates
{
	PLAYER_STATES_IDLING, 
	PLAYER_STATES_RUNNING,
	PLAYER_STATES_AIMING,
	PLAYER_STATES_COUNT,
};

class Editor;

class Player : public Character
{
friend class Editor;
public:
	OrbitalCamera			mOrbitCam;
	FirstPersonCamera		mFPCam;

	shared_ptr<Inventory>	mInventory;

public:
					Player(shared_ptr<Inventory> inventory);
	virtual void	Tick(float const dt) override;

protected:
	virtual void	OnHeal(float const amount) override;
	virtual void	OnDamage(float const dmg) override;
	virtual void	OnDeath() override;
	virtual void	LookAt(vec3 const& direction) override;

private:
	void			IdlingState(float const dt);
	void			RunningState(float const dt);
	void			AimingState(float const dt);
};

enum soldierStates : u32
{
	SOLDIER_STATES_PATROLLING, 
	SOLDIER_STATES_PURSUING, 
	SOLDIER_STATES_FIRING,
};

class Soldier : public Character
{
public:
	PathFollower			mPathFollower;
	shared_ptr<RCFirearm>	mFirearm;
	float					mTargetDesire; // how much does it want to follow the player vs avoiding obstacles
	float					mVisionRange;
	float					mTargetVisionRange;

	// firing
	float					mFireTimer;
	float					mFireTime;
	float					mFireStandStillTimer;
	float					mFireStandStillTime;

public:
					Soldier();
	virtual void	Tick(float const dt) override;
	virtual void	Render() const override;
	void			ToStartPosition();
	void			Detected();

protected:
	void			FollowTarget(float const dt, vec3 const& target);

	virtual void	OnHeal(float const amount) override;
	virtual void	OnDamage(float const dmg) override;
	virtual void	OnDeath() override;

private:
	void			PatrollingState(float const dt);
	void			PursuingState(float const dt);
	void			FiringState(float const dt);
};

enum alertLevels : u8
{
	ALERT_LEVELS_OFF, 
	ALERT_LEVELS_DETECTED, 
};

class Soldiers
{
public:
	vector<shared_ptr<Soldier>> mUnits;
	u8							mAlertLevel;

public:
	static Soldiers& Singleton();

public:
			Soldiers();
	void	ToStartPosition();
	void	Detect();

	inline u8 GetAlertLevel() const { return mAlertLevel; }
};