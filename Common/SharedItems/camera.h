#pragma once

class Scene;
class Editor;
class Player;

class Camera
{	
friend class Editor;
public:
	float			mYaw;
	float			mPitch;

protected:
	vec3			mPosition;
	vec3			mAhead;
	vec3			mUp;
	vec3			mRight;
	float			mSpeed;
	float			mSens;
	float			mFOV; 

public:
						Camera();
	mat4 				CalcView() const;
	mat4 				CalcProjection() const;
	mat4				CalcOrtho() const;

	inline vec3 const&	GetPosition() const	{ return mPosition; }
	inline vec3 const&	GetAhead() const	{ return mAhead; }
	inline vec3 const&	GetUp() const		{ return mUp; }
	inline vec3 const&	GetRight() const	{ return mRight; }

	virtual void		Tick(float const dt) = 0;
	virtual void		Activate() {}

protected:
	void				Refresh();
};

class FreeCamera : public Camera
{
friend class Editor;
public:
	virtual void Tick(float const dt) override;
};

class SpinningCamera : public Camera
{
friend class Editor;
public:
	vec3 mLookAt;

public:
	virtual void Tick(float const dt) override;
	virtual void Activate() override;
};

class OrbitalCamera : public Camera
{
friend class Editor;
public:
	Player const*	mPlayer;				// to orbit around
	vec3			mOffset;
	float			mDist		= 130.0f;	// distance from subject
	float			mLerpSpeed	= 0.3f;
	bool			mLerp;

public:
					OrbitalCamera() = default; 
	virtual void	Tick(float const dt) override;
	void			SetPlayer(Player const& player);
};

class FirstPersonCamera : public Camera
{
friend class Editor;
private:
	Player const*	mPlayer;
	float			mAngle = 0.0f; // for sine motion

public:
	virtual void	Tick(float const dt) override;
	virtual void	Activate() override;
	void			SetPlayer(Player const& player);
};

namespace CameraManager
{
	void			Init(Camera& cam);
	void			Tick(float const dt);
	void			Cleanup();
	void			SetActive(Camera& cam);
	void			Transition(Camera& cam);
	bool			IsTransitioning();
	Camera const*	GetActive();
}

namespace CM = CameraManager;
