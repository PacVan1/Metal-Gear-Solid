#include "common.h"
#include "firearm.h"

Firearm::Firearm(shared_ptr<StaticModelNode> model) :
	Item(model),
	mFirePoint(0.0f), 
	mGroup(FILTERS_NONE),
	mMask(FILTERS_NONE), 
	mRecoil(false), 
	mRecoilTheta(0.0f)
{}

void Firearm::Tick(float const dt)
{
	if (!mIsActive) return;
	if (mRecoil)
	{
		mRecoilTheta += dt * 20.0f;
		if (mRecoilTheta >= PI)
		{
			mRecoil			= false;
			mRecoilTheta	= 0.0f;
			mDisplay->SetLocalPosition(vec3(mDisplay->GetLocalPosition().x, mDisplay->GetLocalPosition().y, 0.0f));
		}
		else
			mDisplay->SetLocalPosition(vec3(mDisplay->GetLocalPosition().x, mDisplay->GetLocalPosition().y, -sin(mRecoilTheta) * 0.3f));
	}
	ChildrenTick(dt);
}

void Firearm::Recoil()
{
	mRecoil = true;
	mRecoilTheta = 0.0f;
}

void Firearm::SetGroupAndMask(s32 const group, s32 const mask)
{
	mGroup	= group;
	mMask	= mask;
}

RCFirearm::RCFirearm(shared_ptr<StaticModelNode> model, float const range, float const cooldown, float const dmg) :
	Firearm(model),
	mRange(range),
	mCooldown(cooldown),
	mDmg(dmg)
{
	mDisplay->SetLocalRotation(vec3(0.0f, glm::radians(180.0f), 0.0f));
	SetGroupAndMask(FILTERS_PROJECTILE, FILTERS_ENEMY | FILTERS_TERRAIN);
}

void RCFirearm::Use()
{
	Recoil();
	using namespace RayCaster;
	HitResult result = Cast(GetWorldPosition(), GetWorldPosition() - GetWorldAhead() * mRange, mGroup, mMask, false);
	if (!result.hit) return;

	IDamageable* dmgable = dynamic_cast<IDamageable*>(result.actor);
	if (!dmgable || dmgable->IsDead()) return;

	dmgable->Damage(mDmg);
}
