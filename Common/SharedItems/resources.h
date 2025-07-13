#pragma once

class ResourceManager
{
private:
	unordered_map<string_view, StaticModel*>	mStaticModels;
	unordered_map<string_view, SkeletalModel*>	mSkeletalModels;
	unordered_map<string_view, ColliderMesh*>	mColliderMeshes;
	unordered_map<string_view, Skybox*>			mSkyboxes;

public:
	static ResourceManager& Singleton();

public:
	ResourceManager();
	~ResourceManager();
	StaticModel&	GetStaticModel(string_view const name);
	SkeletalModel&	GetSkeletalModel(string_view const name);
	ColliderMesh&	GetColliderMesh(string_view const name);
	Skybox&			GetSkybox(string_view const name);
};

// alias as shorthand
inline ResourceManager& RM() { return ResourceManager::Singleton(); }