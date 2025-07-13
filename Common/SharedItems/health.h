#pragma once

class IDamageable
{
private:
	float			mHealth;
	float			mMaxHealth;
	bool			mIsDead;

public:
	void			Reset();
	void			Heal(float const amount);
	void			Damage(float const dmg);
	void			Revive();

	inline float	GetHealth() const		{ return mHealth; }
	inline float	GetMaxHealth() const	{ return mMaxHealth; }
	inline bool		IsDead() const			{ return mIsDead; }

protected:
					IDamageable();
	virtual void	OnRevive() = 0;
	virtual void	OnHeal(float const amount) = 0;
	virtual void	OnDamage(float const dmg) = 0;
	virtual void	OnDeath() = 0;
};
