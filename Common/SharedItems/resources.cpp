#include "common.h"
#include "resources.h"

static string_view constexpr MODEL_PATH     = "../Common/Assets/Models/";
static string_view constexpr SKYBOX_PATH    = "../Common/Assets/Skyboxes/";

ResourceManager& ResourceManager::Singleton()
{
    static ResourceManager singleton;
    return singleton;
}

ResourceManager::ResourceManager()
{
    // MYTODO temporary to load the character collider
    ColliderMesh* loaded = new ColliderMesh(new btCapsuleShape(10, 60));
    mColliderMeshes["character_collider"] = loaded;
}

ResourceManager::~ResourceManager()
{
    // asked Copilot how to delete my containers
    for (auto& [_, model]   : mStaticModels)    delete model;
    for (auto& [_, model]   : mSkeletalModels)  delete model;
    for (auto& [_, mesh]    : mColliderMeshes)  delete mesh;
    for (auto& [_, skybox]  : mSkyboxes)        delete skybox;
}

StaticModel& ResourceManager::GetStaticModel(std::string_view const name)
{
    std::string const path = std::string(MODEL_PATH) + std::string(name);
    auto it = mStaticModels.find(name);
    if (it != mStaticModels.end()) return *it->second;
    StaticModel* loaded = new StaticModel(path.c_str());
    mStaticModels[name] = loaded;
    return *loaded;
}

SkeletalModel& ResourceManager::GetSkeletalModel(string_view const name)
{
    std::string const path = std::string(MODEL_PATH) + std::string(name);
    auto it = mSkeletalModels.find(name);
    if (it != mSkeletalModels.end()) return *it->second;
    SkeletalModel* loaded = new SkeletalModel(path.c_str());
    mSkeletalModels[name] = loaded;
    return *loaded;
}

ColliderMesh& ResourceManager::GetColliderMesh(std::string_view const name)
{
    auto it = mColliderMeshes.find(name);
    if (it != mColliderMeshes.end()) return *it->second;
    ColliderMesh* loaded = new ColliderMesh(GetStaticModel(name));
    mColliderMeshes[name] = loaded;
    return *loaded;
}

Skybox& ResourceManager::GetSkybox(string_view const name)
{
    std::string const path = std::string(SKYBOX_PATH) + std::string(name);
    auto it = mSkyboxes.find(name);
    if (it != mSkyboxes.end()) return *it->second;
    Skybox* loaded = new Skybox(path.c_str());
    mSkyboxes[name] = loaded;
    return *loaded;
}
