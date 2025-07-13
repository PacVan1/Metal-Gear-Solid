#include "common.h"
#include "camera.h"

float constexpr INIT_YAW	= -90.0f;
float constexpr INIT_PITCH	=   0.0f;
float constexpr INIT_SPEED	= 100.0f;
float constexpr INIT_SENS	=   0.1f;
float constexpr INIT_FOV	=  45.0f;

float constexpr CAMERA_NEAR_PLANE	=	 0.1f;
float constexpr CAMERA_FAR_PLANE	= 2000.0f;
float constexpr CAMERA_ASPECT		= 4.0f / 3.0f;

Camera::Camera() : 
	mPosition(0.0f, 0.0f, 0.0f),
	mYaw(INIT_YAW),
	mPitch(INIT_PITCH),
	mSpeed(INIT_SPEED),
	mSens(INIT_SENS),
	mFOV(INIT_FOV)
{
	Refresh(); 
}

void Camera::Tick(float const dt)
{
}

mat4 Camera::CalcView() const
{
	return glm::lookAt(mPosition, mPosition + mAhead, UP);
}

mat4 Camera::CalcProjection() const
{
	return glm::perspective(mFOV, CAMERA_ASPECT, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);
}

void Camera::Refresh()
{
	mAhead.x	= cos(radians(mYaw)) * cos(radians(mPitch));
	mAhead.y	= sin(radians(mPitch));
	mAhead.z	= sin(radians(mYaw)) * cos(radians(mPitch));
	mAhead		= glm::normalize(mAhead);
	mRight		= glm::normalize(glm::cross(mAhead, UP));
	mUp			= glm::normalize(glm::cross(mRight, mAhead));
}

void OrbitalCamera::Tick(float const dt)
{
	if (!mPlayer) return;

	mOffset = vec3(0, 0, -mDist);

	// getting input
	mYaw	-= GetInput().GetMouseDeltaPos().x * mSens;
	mPitch	+= GetInput().GetMouseDeltaPos().y * mSens;
	mPitch	= mPitch > 85.0f ? 85.0f : mPitch;
	mPitch	= mPitch < -85.0f ? -85.0f : mPitch;

	// finding position and angle
	mat4 rotMat = glm::rotate(mat4(1.0f), glm::radians(mYaw), UP);
	rotMat		= glm::rotate(rotMat, glm::radians(mPitch), RIGHT);
	vec3 rotOff = vec3(rotMat * vec4(mOffset, 1.0f));
	vec3 target = mPlayer->GetWorldPosition() + rotOff;
	mPosition	= mLerp ? glm::mix(mPosition, target, mLerpSpeed) : target;
	mAhead		= glm::normalize(mPlayer->GetWorldPosition() - mPosition);

	// resolve occluding camera
	btVector3 from	= ToBT(mPlayer->GetWorldPosition());
	btVector3 to	= ToBT(mPosition);
	btCollisionWorld::ClosestRayResultCallback ray(from, to);

	ray.m_collisionFilterGroup	= FILTERS_CAMERA;
	ray.m_collisionFilterMask	= FILTERS_TERRAIN;

	shared_ptr<SceneGraph> scene = SM().GetActive(); 
	btDiscreteDynamicsWorld* world = scene->mDynaWorld.mSimulation;

	world->rayTest(from, to, ray);
	if (ray.hasHit()) mPosition = ToGLM(ray.m_hitPointWorld) - glm::normalize(ToGLM(to - from)) * 0.5f;

	mRight	= glm::normalize(glm::cross(mAhead, UP));
	mUp		= glm::normalize(glm::cross(mRight, mAhead));
}

void OrbitalCamera::SetPlayer(Player const& player)
{
	mPlayer = &player;
}

namespace CameraManager
{
	struct alignas(16) CamBlock
	{
		mat4 view;
		mat4 projection;
		mat4 VP;
		vec4 camPos;
	};

	Camera*		gCam;
	Camera*		gNextCam = nullptr;
	CamBlock	gBlock;
	GLuint		gUBO;
	float		gTransitionSpeed = 5.0f;
	float		gTime = 0.0f;

	void Init(Camera& cam)
	{
		SetActive(cam);
		gUBO = OGL::UBO();
		glBufferData(GL_UNIFORM_BUFFER, sizeof(CamBlock), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_CAM, gUBO);
		CheckOGL();
	}

	void Tick(float const dt)
	{
		// no active cam
		if (!gCam) return;

		// refresh data on CPU
		if (gNextCam)
		{
			gTime += gTransitionSpeed * dt;
			if (gTime >= 1.0f)
			{
				// reached goal
				SetActive(*gNextCam);
				gTime				= 0.0f;
				gNextCam			= nullptr;
			}
			else
			{
				gCam->Tick(dt);
				gNextCam->Tick(dt);
				mat4 const from = gCam->CalcView();
				mat4 const to	= gNextCam->CalcView();
				vec3 posA = vec3(from[3]);
				vec3 posB = vec3(to[3]);
				vec3 lerpPos = glm::mix(posA, posB, gTime);
				quat quatA = glm::quat_cast(from);
				quat quatB = glm::quat_cast(to);
				quat lerpQuat = glm::slerp(quatA, quatB, gTime);
				mat4 rotationMatrix = glm::mat4_cast(lerpQuat);
				mat4 interp = glm::translate(mat4(1.0f), lerpPos) * rotationMatrix;
				gBlock.view = interp;
				gBlock.camPos = vec4(lerpPos, 0.0f);
			}
		}
		else
		{
			gCam->Tick(dt);
			gBlock.view	= gCam->CalcView();
			gBlock.camPos = vec4(gCam->GetPosition(), 0.0f);
		}
		gBlock.projection = gCam->CalcProjection();
		gBlock.VP = gBlock.projection * gBlock.view;

		// upload data to GPU
		glBindBuffer(GL_UNIFORM_BUFFER, gUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CamBlock), &gBlock);
		CheckOGL();
	}

	void Transition(Camera& cam)
	{
		// already active
		if (gCam == &cam) return;

		gTime				= 0.0f;
		gNextCam			= &cam;
		gNextCam->mPitch	= -gCam->mPitch;
		gNextCam->mYaw		= -gCam->mYaw + 90.0f;
		gNextCam->Activate();
	}

	bool IsTransitioning()
	{
		return gNextCam;
	}

	void SetActive(Camera& cam)
	{
		gCam = &cam;
		gCam->Activate();
	}

	Camera const* GetActive()
	{
		return gCam;
	}

	void Cleanup()
	{
		glDeleteBuffers(1, &gUBO);
	}
}

void FreeCamera::Tick(float const dt)
{
	float const vel = mSpeed * dt;
	if (Inputs::MoveRight())	mPosition += mRight * vel;
	if (Inputs::MoveAhead())	mPosition += mAhead * vel;
	if (Inputs::MoveLeft())		mPosition -= mRight * vel;
	if (Inputs::MoveBack())		mPosition -= mAhead * vel;
	if (Inputs::MoveUp())		mPosition += mUp * vel;
	if (Inputs::MoveDown())		mPosition -= mUp * vel;
	if (GetInput().IsMouseButtonDown(MOUSE_RIGHT))
	{
		mYaw	+= GetInput().GetMouseDeltaPos().x * mSens;
		mPitch	-= GetInput().GetMouseDeltaPos().y * mSens;
		Refresh();
	}
}

void FirstPersonCamera::Tick(float const dt)
{
	if (GetInput().IsMouseButtonDown(MOUSE_RIGHT))
	{
		mYaw += GetInput().GetMouseDeltaPos().x * mSens;
		mPitch -= GetInput().GetMouseDeltaPos().y * mSens;
		Refresh();
	}

	if (!mPlayer) return;

	mPosition = mPlayer->GetWorldPosition();
	mPosition.y += 20.0f;

	if (Inputs::MoveAhead() || Inputs::MoveRight() ||
		Inputs::MoveLeft() || Inputs::MoveBack())
	{
		mPosition.y += cos(radians(mAngle += dt * 1000.0f)) * 0.5f;
	}
}

void FirstPersonCamera::Activate()
{
	if (!mPlayer) return;
	mPosition = mPlayer->GetWorldPosition();
	mPosition.y += 20.0f;
	Refresh();
}

void FirstPersonCamera::SetPlayer(Player const& player)
{
	mPlayer = &player;
}

void SpinningCamera::Tick(float const dt)
{
	static float angle = 0.0f;

	angle += dt;
	float const cosa = cos(angle); 
	float const sina = sin(angle);

	mPosition = mLookAt + vec3(cosa * 250.0f, 325.0f, sina * 250.0f);
	mAhead = mLookAt - mPosition;
}

void SpinningCamera::Activate()
{
	Refresh();
}
