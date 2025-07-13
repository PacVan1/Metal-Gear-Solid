#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>  
#include <assimp/postprocess.h> 
#include <assimp/quaternion.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

#include "model.h"

struct KeyPosition
{
	vec3	position;
	f32		timeStamp;
};

struct KeyRotation
{
	quat	rotation;
	f32		timeStamp;
};

struct KeyScale
{
	vec3	scale;
	f32		timeStamp;
};

struct Bone
{
	vector<KeyPosition>	positions;
	vector<KeyRotation>	rotations;
	vector<KeyScale>	scales;
	s32					positionCount;
	s32					rotationCount;
	s32					scaleCount;
	mat4				local;
	std::string			name;
	s32					ID; 

	Bone(std::string const& name, s32 const id, aiNodeAnim const& channel); 

	void Update(f32 const animTime);
	s32 GetPositionIndex(f32 animationTime);
	s32 GetRotationIndex(f32 animationTime);
	s32 GetScaleIndex(f32 animationTime);
	f32 GetScaleFactor(f32 lastTimeStep, f32 nextTimeStep, f32 animationTime);
	mat4 InterpolatePosition(f32 animationTime);
	mat4 InterpolateRotation(f32 animationTime);
	mat4 InterpolateScaling(f32 animationTime);
};

struct BoneInfo
{
	s32		ID;
	mat4	offset;
};

struct AssimpNodeData
{
	mat4					transformation;
	std::string				name;
	s32						childrenCount;
	vector<AssimpNodeData>	children;
};

struct SkeletalModel;

struct Animation
{
	f32								duration;
	s32								rate;
	vector<Bone>					bones;
	AssimpNodeData					rootNode;
	std::map<std::string, BoneInfo> boneInfoMap;

	Animation() = default;
	~Animation() {}
	Animation(std::string const& path, SkeletalModel& model);
	Bone* FindBone(std::string const& name);

	void ReadMissingBones(aiAnimation const* anim, SkeletalModel& model);
};

using AnimationSet = vector<Animation>;

struct AnimationState
{
	Animation*	currentAnim = nullptr;
	Animation*	nextAnim	= nullptr;
	float		blendFactor = 0.0f;
};

class Animator
{
private:
	vector<mat4>	mFinalBoneMatrices;
	Animation*		mAnim;
	f32				mTime;
	bool			mPlaying;

public:
	Animator();
	inline void Play() { mPlaying = true; }
	inline void Stop() { mPlaying = false; }
	void Tick(f32 const dt);
	void SetTime(f32 const time);
	void SetAnim(Animation* anim);
	void SetAnim2(Animation* anim);

	inline std::vector<mat4> const& GetBoneMatrices() const { return mFinalBoneMatrices; }

private:
	void CalcBoneTransform(AssimpNodeData const* node, mat4 parent);
};

auto constexpr BONE_INFLUENCE_COUNT = 4;
auto constexpr BONE_COUNT			= 100;

struct SkeletalVertex
{
	vec3	position;
	vec3	normal;
	vec2	texCoord;
	s32		boneIDs[BONE_INFLUENCE_COUNT];
	f32		weights[BONE_INFLUENCE_COUNT];
};

using SkeletalMesh = Mesh<SkeletalVertex>;
struct SkeletalModel : public Model<SkeletalMesh>
{
public:
	s32								mBoneCounter;
	std::map<std::string, BoneInfo> mBoneInfoMap;
	AnimationSet					mAnims;

public:
	SkeletalModel() = default;
	SkeletalModel(char const* path);
	virtual void Render(mat4 const& transform) const override;

private:
	void LoadMesh(aiScene const& scene, aiMesh const& mesh);
	void TraverseScene(aiScene const& scene, aiNode const& node);
};

class SkeletalModelInstance : public ModelInstance<SkeletalModel>
{
private:
	Animator		mAnimator;

public:
					SkeletalModelInstance() = default;
					SkeletalModelInstance(SkeletalModel* model);
	virtual void	Tick(f32 const dt) override;
	virtual void	Render() const override;
	void			SetAnim(u32 const animIdx); 
};

class SkeletalModelNode : public ModelNode<SkeletalModel>
{
public:
	Animator mAnimator;

public:
	SkeletalModelNode() = default;
	SkeletalModelNode(SkeletalModel* model);
	SkeletalModelNode(char const* name);
	virtual void	Tick(float const dt) override;
	virtual void	Render() const override;
	void			SetAnim(u32 const animIdx);
};

shared_ptr<SkeletalModelNode> CreateSkeletalModelNode(char const* name);
