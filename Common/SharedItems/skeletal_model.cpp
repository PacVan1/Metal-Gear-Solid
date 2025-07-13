#include "common.h"
#include "skeletal_model.h"

#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

vec3 ToGLM(aiVector3D const& aiVec)
{
	return vec3(aiVec.x, aiVec.y, aiVec.z);
}

inline void ToGLM(aiVector3D const& aiVec, vec3& glmVec)
{
	glmVec = vec3(aiVec.x, aiVec.y, aiVec.z);
}

mat4 ToGLM(aiMatrix4x4 const& aiMat)
{
	mat4 mat; std::memcpy(glm::value_ptr(mat), &aiMat, sizeof(mat4));
	return glm::transpose(mat); // from row-major to column-major
}

inline void ToGLM(aiMatrix4x4 const& aiMat, mat4& glmMat)
{
	std::memcpy(glm::value_ptr(glmMat), &aiMat, sizeof(mat4));
	glmMat = glm::transpose(glmMat); // from row-major to column-major
}

quat ToGLM(aiQuaternion const& aiQuat)
{
	return quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
}

static void UploadMesh(SkeletalMesh& m)
{
	m.VAO = OGL::VAO();
	m.VBO = OGL::VBO();
	m.EBO = OGL::EBO();
	glBufferData(GL_ARRAY_BUFFER,		  m.vertices.size() * sizeof(SkeletalVertex), m.vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(u32),			  m.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer (0, 3,				  GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, position));
	glVertexAttribPointer (1, 3,				  GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, normal));
	glVertexAttribPointer (2, 2,				  GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, texCoord));
	glVertexAttribIPointer(3, BONE_INFLUENCE_COUNT, GL_INT,			  sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, boneIDs));
	glVertexAttribPointer (4, BONE_INFLUENCE_COUNT, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, weights));
	glEnableVertexAttribArray(0); 
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	CheckOGL();
}

struct ModelData 
{
	std::string modelPath;
	vector<std::string> animations;
};

static ModelData ParseModelFile(const std::string& filename) 
{
	ModelData modelData;
	std::ifstream file(filename);
	std::string line;

	if (!file.is_open()) 
	{
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return modelData;
	}

	while (std::getline(file, line)) 
	{
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

		if (line.compare(0, 6, "model:") == 0) 
		{
			modelData.modelPath = line.substr(7); // Extract model path
		}
		else if (line.compare(0, 5, "anim:") == 0)
		{
			modelData.animations.push_back(line.substr(6)); // Corrected extraction
		}
	}

	file.close();
	return modelData;
}

std::string dir;

SkeletalModel::SkeletalModel(char const* path) : 
	mBoneCounter(0)
{
	//return; // .dae is broken on Pi

	std::string p = path;
	dir = p.substr(0, p.find_last_of('/') + 1);

	ModelData loadData = ParseModelFile(path);

	static u32 constexpr PROCESS_FLAGS =
		aiProcess_Triangulate |
		aiProcess_FlipUVs | 
		aiProcess_CalcTangentSpace |
		aiProcess_GenNormals;

	Assimp::Importer importer;
	std::string fullPath = dir + loadData.modelPath;
	aiScene const* scene = importer.ReadFile((fullPath).c_str(), PROCESS_FLAGS);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		DebugLog(ASSET_FAILED, (dir + loadData.modelPath).c_str());
		DebugLog(WARNING, importer.GetErrorString());
		return;
	}
	TraverseScene(*scene, *scene->mRootNode);
	DebugLog(ASSET_LOADED, (dir + loadData.modelPath).c_str());

	for (int i = 0; i < loadData.animations.size(); i++)
	{
		mAnims.push_back(Animation((dir + loadData.animations[i]).c_str(), *this));
	}
}

void SkeletalModel::Render(mat4 const&) const
{
}

void SkeletalModel::LoadMesh(aiScene const& scene, aiMesh const& aiMesh)
{
	meshes.emplace_back();

	SkeletalMesh&				 mesh = meshes.back();
	std::vector<SkeletalVertex>& vtcs = mesh.vertices;
	std::vector<u32>&			 inds = mesh.indices;
	Material&					 mat  = mesh.mat;

	// loading positions, normals and texture coordinates
	vtcs.resize(aiMesh.mNumVertices);

	for (u32 i = 0; i < aiMesh.mNumVertices; i++)
	{
		for (int j = 0; j < BONE_INFLUENCE_COUNT; j++)
		{
			vtcs[i].boneIDs[j] = -1;
			vtcs[i].weights[j] = 0.0f;
		}
	}

	for (u32 i = 0; i < aiMesh.mNumVertices; ++i)
		vtcs[i].position = ToGLM(aiMesh.mVertices[i]);

	if (aiMesh.HasNormals()) for (u32 i = 0; i < aiMesh.mNumVertices; ++i)
		vtcs[i].normal = ToGLM(aiMesh.mNormals[i]);

	if (aiMesh.HasTextureCoords(0)) for (u32 i = 0; i < aiMesh.mNumVertices; ++i)
		vtcs[i].texCoord = ToGLM(aiMesh.mTextureCoords[0][i]);

	// loading indices
	inds.resize(aiMesh.mNumFaces * 3);
	for (u32 i = 0; i < aiMesh.mNumFaces; ++i)
	{
		aiFace face = aiMesh.mFaces[i];
		inds[i * 3 + 0] = face.mIndices[0];
		inds[i * 3 + 1] = face.mIndices[1];
		inds[i * 3 + 2] = face.mIndices[2];
	}

	// load bone information
	for (u32 b = 0; b < aiMesh.mNumBones; ++b)
	{
		aiBone& bone = *aiMesh.mBones[b];
	
		s32 boneID = -1;
		std::string boneName = bone.mName.C_Str();
		if (mBoneInfoMap.find(boneName) == mBoneInfoMap.end())
		{
			BoneInfo boneInfo;
			boneInfo.ID		= mBoneCounter;
			boneInfo.offset = ToGLM(bone.mOffsetMatrix);

			// register the bone
			mBoneInfoMap[boneName] = boneInfo;

			boneID = mBoneCounter; // boneID is equal to what number the bone was in line.
			mBoneCounter++;
		}
		else
		{
			boneID = mBoneInfoMap[boneName].ID;
		}
		assert(boneID != -1);
	
		aiVertexWeight* weights = bone.mWeights;
		for (u32 w = 0; w < bone.mNumWeights; ++w)
		{
			if (!weights) continue;
			s32 vertexID = weights[w].mVertexId;
			f32 weight = weights[w].mWeight;
			assert(vertexID <= vtcs.size());
			SkeletalVertex& vertex = vtcs[vertexID];
			for (s32 i = 0; i < BONE_INFLUENCE_COUNT; i++)
			{
				if (vertex.boneIDs[i] < 0)
				{
					vertex.weights[i] = weight;
					vertex.boneIDs[i] = boneID;
					break;
				}
			}
		}
	}

	UploadMesh(mesh);

	// load materials:
	if (aiMesh.mMaterialIndex >= 0)
	{
		aiMaterial& aiMat = *scene.mMaterials[aiMesh.mMaterialIndex];
		LoadMaterialTexture(aiMat, aiTextureType_DIFFUSE, TEXTURE_TYPES_ALBEDO, dir, mat);
		//LoadMaterial(mat, aiTextureType_NORMALS, texs);
		//LoadMaterial(mat, aiTextureType_SPECULAR, texs);
	}
}

void SkeletalModel::TraverseScene(aiScene const& scene, aiNode const& node)
{
	for (u32 i = 0; i < node.mNumMeshes; i++)
	{
		aiMesh const& mesh = *scene.mMeshes[node.mMeshes[i]];
		LoadMesh(scene, mesh);
	}
	for (u32 i = 0; i < node.mNumChildren; i++)
	{
		TraverseScene(scene, *node.mChildren[i]);
	}
}

static void ReadHierarchyData(AssimpNodeData& dest, aiNode const* src)
{
	assert(src);

	dest.name			= src->mName.data;
	dest.transformation = ToGLM(src->mTransformation);
	dest.childrenCount	= src->mNumChildren;

	for (u32 i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData data;
		ReadHierarchyData(data, src->mChildren[i]);
		dest.children.push_back(data);
	}
}

Animation::Animation(std::string const& path, SkeletalModel& model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
	//if (!scene || !scene->HasAnimations())
	//{
	//	DebugLog(ASSET_FAILED, importer.GetErrorString());
	//	return;
	//}
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0]; // load the first (and only) animation in the scene
	duration = static_cast<f32>(animation->mDuration);
	rate = static_cast<s32>(animation->mTicksPerSecond);
	ReadHierarchyData(rootNode, scene->mRootNode);
	ReadMissingBones(animation, model);
	DebugLog(ASSET_LOADED, path.c_str());
}

void Animation::ReadMissingBones(aiAnimation const* anim, SkeletalModel& model)
{
	// how many of the bones that I have, do animate?:
	s32 size = anim->mNumChannels;

	std::map<std::string, BoneInfo>& infoMap = model.mBoneInfoMap;
	s32& boneCount = model.mBoneCounter;

	for (s32 i = 0; i < size; i++)
	{
		auto channel = anim->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		// if the bone does not yet exist:
		if (infoMap.find(boneName) == infoMap.end())
		{
			infoMap[boneName].ID = boneCount;
			boneCount++;
		}

		bones.push_back(Bone(channel->mNodeName.data, infoMap[channel->mNodeName.data].ID, *channel));
	}
	this->boneInfoMap = infoMap;
}

Bone::Bone(std::string const& name, s32 const id, aiNodeAnim const& channel) :
	name(name),
	ID(id),
	local(1.0f)
{
	positionCount = channel.mNumPositionKeys;
	for (s32 i = 0; i < positionCount; i++)
	{
		aiVector3D aiPosition	= channel.mPositionKeys[i].mValue;
		float timeStamp			= static_cast<f32>(channel.mPositionKeys[i].mTime);
		KeyPosition data;
		data.position	= ToGLM(aiPosition);
		data.timeStamp	= timeStamp;
		positions.push_back(data);
	}

	rotationCount = channel.mNumRotationKeys;
	for (s32 i = 0; i < rotationCount; i++)
	{
		aiQuaternion aiRotation = channel.mRotationKeys[i].mValue;
		float timeStamp			= static_cast<f32>(channel.mRotationKeys[i].mTime);
		KeyRotation data;
		data.rotation	= ToGLM(aiRotation);
		data.timeStamp	= timeStamp;
		rotations.push_back(data);
	}

	scaleCount = channel.mNumScalingKeys;
	for (s32 i = 0; i < scaleCount; i++)
	{
		aiVector3D aiScaling	= channel.mScalingKeys[i].mValue;
		float timeStamp			= static_cast<f32>(channel.mScalingKeys[i].mTime);
		KeyScale data;
		data.scale		= ToGLM(aiScaling); 
		data.timeStamp	= timeStamp;
		scales.push_back(data);
	}
}

void Bone::Update(f32 const animTime)
{
	mat4 translation	= InterpolatePosition(animTime);
	mat4 rotation		= InterpolateRotation(animTime);
	mat4 scale			= InterpolateScaling(animTime);
	local				= translation * rotation * scale;
}

s32 Bone::GetPositionIndex(f32 animationTime)
{
	for (int i = 0; i < positionCount - 1; ++i)
	{
		if (animationTime < positions[i + 1].timeStamp)
			return i;
	}
	assert(0);
	return 0;
}

s32 Bone::GetRotationIndex(f32 animationTime)
{
	for (int index = 0; index < rotationCount - 1; ++index)
	{
		if (animationTime < rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

s32 Bone::GetScaleIndex(f32 animationTime)
{
	for (int i = 0; i < scaleCount - 1; ++i)
	{
		if (animationTime < scales[i + 1].timeStamp)
			return i;
	}
	assert(0);
	return 0;
}

f32 Bone::GetScaleFactor(f32 lastTimeStep, f32 nextTimeStep, f32 animationTime)
{
	f32 scaleFactor = 0.0f;
	f32 midWayLength = animationTime - lastTimeStep;
	f32 framesDiff = nextTimeStep - lastTimeStep;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

mat4 Bone::InterpolatePosition(f32 animationTime)
{
	if (1 == positionCount)
		return glm::translate(mat4(1.0f), positions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp,
		positions[p1Index].timeStamp, animationTime);
	vec3 finalPosition = glm::mix(positions[p0Index].position,
		positions[p1Index].position, scaleFactor);
	return glm::translate(mat4(1.0f), finalPosition);
}

mat4 Bone::InterpolateRotation(f32 animationTime)
{
	if (1 == rotationCount)
	{
		quat rotation = glm::normalize(rotations[0].rotation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp,
		rotations[p1Index].timeStamp, animationTime);
	quat finalRotation = glm::slerp(rotations[p0Index].rotation,
		rotations[p1Index].rotation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

mat4 Bone::InterpolateScaling(f32 animationTime)
{
	if (1 == scaleCount)
		return glm::scale(mat4(1.0f), scales[0].scale);

	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp,
		scales[p1Index].timeStamp, animationTime);
	vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale
		, scaleFactor);
	return glm::scale(mat4(1.0f), finalScale);
}

Bone* Animation::FindBone(std::string const& name)
{
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](Bone& bone)
		{
			return bone.name == name;
		}
	);
	if (iter == bones.end()) return nullptr;
	else return &(*iter);
}

Animator::Animator() : 
	mAnim(nullptr), 
	mPlaying(true)
{
	mTime = 0.0f;

	mFinalBoneMatrices.reserve(100);
	for (s32 i = 0; i < 100; i++)
	{
		mFinalBoneMatrices.push_back(mat4(1.0f));
	}
}

void Animator::Tick(f32 const dt)
{
	if (!mPlaying || !mAnim) return;

	mTime += mAnim->rate * dt;
	mTime = fmod(mTime, mAnim->duration);
	CalcBoneTransform(&mAnim->rootNode, mat4(1.0f));
}

void Animator::SetTime(f32 const time)
{
	if (!mAnim) return;

	mTime = mAnim->rate * time;
	mTime = fmod(mTime, mAnim->duration);
	CalcBoneTransform(&mAnim->rootNode, mat4(1.0f));
}

void Animator::SetAnim(Animation* anim)
{
	mAnim = anim;
	SetTime(0.0f);
}

void Animator::SetAnim2(Animation* anim)
{
	mTime = 0.0f;
	mAnim = anim;

	mFinalBoneMatrices.reserve(BONE_COUNT);

	for (int i = 0; i < BONE_COUNT; i++)
		mFinalBoneMatrices.push_back(mat4(1.0f));
}

void Animator::CalcBoneTransform(AssimpNodeData const* node, mat4 parent)
{
	std::string nodeName = node->name;
	mat4 nodeTransform = node->transformation;

	Bone* bone = mAnim->FindBone(nodeName);

	if (bone)
	{
		bone->Update(mTime);
		nodeTransform = bone->local;
	}

	mat4 const world = parent * nodeTransform;

	std::map<std::string, BoneInfo>& boneInfoMap = mAnim->boneInfoMap;
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		s32 index = boneInfoMap[nodeName].ID;
		mat4 offset = boneInfoMap[nodeName].offset;
		mFinalBoneMatrices[index] = world * offset;
	}

	for (s32 i = 0; i < node->childrenCount; i++)
		CalcBoneTransform(&node->children[i], world);
}

SkeletalModelInstance::SkeletalModelInstance(SkeletalModel* model)
{
	mModel = model;
	
	if (!mModel->mAnims.empty())
	{
		mAnimator.SetAnim(&mModel->mAnims[0]);
		mAnimator.Play();
	}
}

void SkeletalModelInstance::Tick(f32 const dt)
{
	mAnimator.Tick(dt);
}

void SkeletalModelInstance::Render() const
{
	static u32 const SHADER			= Shader::LoadAndBind("unified_shader", { BINDING_CAM, BINDING_POINT_LIGHT, BINDING_SPOTLIGHT }, { "SKELETAL" });
	static s32 const BONES_LOC		= Shader::GetLoc("finalBoneMatrices");

	if (!mModel) return;
	Shader::Bind(SHADER);

	Shader::SetMat4("model", mTransform.GetWorld());
	std::vector<mat4> const& transforms = mAnimator.GetBoneMatrices();
	glUniformMatrix4fv(Shader::GetLoc("finalBoneMatrices"), 100, GL_FALSE, value_ptr(transforms[0]));
	for (SkeletalMesh const& mesh : mModel->meshes)
	{
		Shader::SetTexture2D("diffuse", mesh.mat.texs[TEXTURE_TYPES_ALBEDO]);
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}
}

void SkeletalModelInstance::SetAnim(u32 const animIdx)
{
	if (animIdx < mModel->mAnims.size())
	{
		mAnimator.SetAnim(&mModel->mAnims[animIdx]);
	}
}

shared_ptr<SkeletalModelNode> CreateSkeletalModelNode(char const* name)
{
	return make_shared<SkeletalModelNode>(&RM().GetSkeletalModel(name));
}

SkeletalModelNode::SkeletalModelNode(SkeletalModel* model) :
	ModelNode(model)
{}

SkeletalModelNode::SkeletalModelNode(char const* name) :
	ModelNode(&RM().GetSkeletalModel(name))
{
	SetName("Skeletal Model");

	if (mModel && !mModel->mAnims.empty())
		SetAnim(0);
}

void SkeletalModelNode::Tick(float const dt)
{
	if (!mIsActive) return;
	mAnimator.Tick(dt);
	ChildrenTick(dt);
}

void SkeletalModelNode::Render() const
{
	static u32 const SHADER = Shader::LoadAndBind("unified_shader", 
		{ BINDING_CAM, BINDING_POINT_LIGHT, BINDING_SPOTLIGHT }, 
		{ "SKELETAL" });
	static s32 const BONES_LOC = Shader::GetLoc("finalBoneMatrices");

	if (!mModel || !mIsVisible) return;
	Shader::Bind(SHADER);

	Shader::SetMat4("model", GetWorldTransform());
	std::vector<mat4> const& transforms = mAnimator.GetBoneMatrices();
	glUniformMatrix4fv(Shader::GetLoc("finalBoneMatrices"), 100, GL_FALSE, value_ptr(transforms[0]));
	for (SkeletalMesh const& mesh : mModel->meshes)
	{
		Shader::SetTexture2D("diffuse", mesh.mat.texs[TEXTURE_TYPES_ALBEDO]);
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}

	ChildrenRender();
}

void SkeletalModelNode::SetAnim(u32 const animIdx)
{
	if (animIdx < mModel->mAnims.size())
		mAnimator.SetAnim(&mModel->mAnims[animIdx]);
}
