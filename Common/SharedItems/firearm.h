#pragma once

class Firearm : public Item
{
protected:
	vec3	mFirePoint;
	s32		mGroup;
	s32		mMask;

	bool	mRecoil;
	float	mRecoilTheta;

public:
	void SetGroupAndMask(s32 const group, s32 const mask);

protected:
	Firearm(shared_ptr<StaticModelNode> model);
	virtual void Tick(float const dt) override;
	void Recoil();
};

// ray casting firearm
class RCFirearm : public Firearm
{
protected:
	float	mRange;		// how far it can fire
	float	mCooldown;	// how long it times for next fire
	float	mDmg;		// damage dealt

public:
	RCFirearm(shared_ptr<StaticModelNode> model, float const range, float const cooldown, float const dmg);
	virtual void Use() override;
};