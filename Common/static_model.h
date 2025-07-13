#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>  
#include <assimp/postprocess.h> 

#include "model.h"

struct StaticVertex
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
};

using StaticMesh = Mesh<StaticVertex>;
struct StaticModel : public Model<StaticMesh>
{
public:
	StaticModel() = default;
	StaticModel(char const* path);
	virtual void Render(mat4 const& transform) const override;

private:
	void LoadMesh(aiScene const& scene, aiMesh const& mesh);
	void TraverseScene(aiScene const& scene, aiNode const& node);
};

class StaticModelInstance : public ModelInstance<StaticModel>
{
public:
	StaticModelInstance() = default;
	StaticModelInstance(StaticModel* model);
	virtual void Render() const override;
};

class StaticModelNode : public ModelNode<StaticModel>
{
public:
	StaticModelNode(char const* name);
};
