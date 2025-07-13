#include "common.h"
#include "health.h"

void IDamageable::Reset()
{
	mHealth = mMaxHealth;
	mIsDead = false;
}

void IDamageable::Heal(float const amount)
{
	mHealth += amount;
	if (mHealth >= mMaxHealth)
		mHealth = mMaxHealth;
	OnHeal(amount);
}

void IDamageable::Damage(float const dmg)
{
	mHealth -= dmg;
	OnDamage(dmg);
	if (mHealth <= 0.0f)
	{
		mHealth = 0.0f;
		mIsDead = true;
		OnDeath();
	}
}

void IDamageable::Revive()
{
	Reset();
	OnRevive();
}

IDamageable::IDamageable() :
	mMaxHealth(10.0f)
{
	Reset();
}
