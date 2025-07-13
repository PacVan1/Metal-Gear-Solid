#include "common.h"
#include "static_model.h"

std::string directory;

static void UploadMesh(StaticMesh& m)
{
	m.VAO = OGL::VAO();
	m.VBO = OGL::VBO();
	m.EBO = OGL::EBO();
	glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(StaticVertex), m.vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(u32), m.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, texCoord));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	CheckOGL();
}

static void LoadMaterial(aiMaterial const& aiMat, aiTextureType const aiType, u8 const type, Material& mat)
{
	for (u32 i = 0; i < aiMat.GetTextureCount(aiType); ++i)
	{
		aiString str; aiMat.GetTexture(aiType, i, &str);
		mat.texs[type] = OGLTexture2D(str.C_Str(), directory);
	}
}

void StaticModel::LoadMesh(aiScene const& scene, aiMesh const& aiMesh)
{
	meshes.emplace_back();

	StaticMesh&					mesh = meshes.back();
	std::vector<StaticVertex>&	vtcs = mesh.vertices;
	std::vector<u32>&			inds = mesh.indices;
	Material&					mat	 = mesh.mat;

	vtcs.resize(aiMesh.mNumVertices);
	vtcs.reserve(aiMesh.mNumVertices);
	for (u32 i = 0; i < aiMesh.mNumVertices; ++i)
	{
		vtcs[i].position.x = aiMesh.mVertices[i].x;
		vtcs[i].position.y = aiMesh.mVertices[i].y;
		vtcs[i].position.z = aiMesh.mVertices[i].z;

		if (aiMesh.HasNormals())
		{
			vtcs[i].normal.x = aiMesh.mNormals[i].x;
			vtcs[i].normal.y = aiMesh.mNormals[i].y;
			vtcs[i].normal.z = aiMesh.mNormals[i].z;
		}
		if (aiMesh.HasTextureCoords(0))
		{
			vtcs[i].texCoord.x = aiMesh.mTextureCoords[0][i].x;
			vtcs[i].texCoord.y = aiMesh.mTextureCoords[0][i].y;
		}
	}

	inds.resize(aiMesh.mNumFaces * 3);
	inds.reserve(aiMesh.mNumFaces * 3);
	for (u32 i = 0; i < aiMesh.mNumFaces; ++i)
	{
		aiFace face = aiMesh.mFaces[i];
		inds[i * 3 + 0] = face.mIndices[0];
		inds[i * 3 + 1] = face.mIndices[1];
		inds[i * 3 + 2] = face.mIndices[2];
	}

	if (aiMesh.mMaterialIndex >= 0)
	{
		aiMaterial& aiMat = *scene.mMaterials[aiMesh.mMaterialIndex];
		LoadMaterial(aiMat, aiTextureType_DIFFUSE, TEXTURE_TYPES_ALBEDO, mat);
		//LoadMaterial(mat, aiTextureType_NORMALS, texs);
		//LoadMaterial(mat, aiTextureType_SPECULAR, texs);
	}

	UploadMesh(mesh);
}

void StaticModel::TraverseScene(aiScene const& scene, aiNode const& node)
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

StaticModel::StaticModel(char const* path)
{
	static u32 constexpr PROCESS_FLAGS =
		aiProcess_Triangulate |
		aiProcess_FlipUVs | 
		aiProcess_PreTransformVertices |
		aiProcess_GenSmoothNormals;

	Assimp::Importer importer;
	aiScene const* scene = importer.ReadFile(path, PROCESS_FLAGS);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		DebugLog(ASSET_FAILED, path);
		DebugLog(WARNING, importer.GetErrorString());
		return;
	}
	std::string p = path;
	directory = p.substr(0, p.find_last_of('/') + 1);
	TraverseScene(*scene, *scene->mRootNode);
	DebugLog(ASSET_LOADED, path);
}

void StaticModel::Render(mat4 const& transform) const
{
	static u32 const PROGRAM = Shader::LoadAndBind("unified_shader",
		{ BINDING_CAM, BINDING_POINT_LIGHT, BINDING_SPOTLIGHT },
		{ "STATIC" });

	Shader::Bind(PROGRAM);
	Shader::SetMat4("model", transform);
	for (StaticMesh const& mesh : meshes)
	{
		Shader::SetTexture2D("diffuse", mesh.mat.texs[TEXTURE_TYPES_ALBEDO]);
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}
}

StaticModelInstance::StaticModelInstance(StaticModel* model)
{
	mModel = model;
}

void StaticModelInstance::Render() const
{
	static u32 const PROGRAM = Shader::LoadAndBind("unified_shader", 
		{ BINDING_CAM, BINDING_POINT_LIGHT, BINDING_SPOTLIGHT }, 
		{ "STATIC" });

	if (!mModel) return;
	Shader::Bind(PROGRAM);
	Shader::SetMat4("model", mTransform.GetWorld());
	for (StaticMesh const& mesh : mModel->meshes)
	{
		Shader::SetTexture2D("diffuse", mesh.mat.texs[TEXTURE_TYPES_ALBEDO]);
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}
}

//void StaticModelInstance::Render(Camera const& camera) const
//{
//	static u32 const PROGRAM = Shader::LoadAndBind("textured", { BINDING_CAM, BINDING_POINT_LIGHT, BINDING_SPOTLIGHT }, { "STATIC" });
//	static u32 const MODEL = Shader::GetLoc("model");
//
//	if (!mModel) return;
//
//	Shader::Bind(PROGRAM);
//	Shader::SetMat4("model", mTransform.GetWorld());
//	for (StaticMesh const& mesh : mModel->meshes)
//	{
//		Shader::SetTexture2D("diffuse", mesh.mat.texs[TEXTURE_TYPES_ALBEDO]);
//		glBindVertexArray(mesh.VAO);
//		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
//	}
//}

StaticModelNode::StaticModelNode() : 
	ModelNode(nullptr)
{
	SetName("Static Model");
}

StaticModelNode::StaticModelNode(char const* name) :
	ModelNode(&RM().GetStaticModel(name))
{
	SetName("Static Model");
}
