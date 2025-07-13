#include "common.h"
#include "model.h"

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>  
//#include <assimp/postprocess.h> 
//#include "skeletal_model.h"
//
//std::string directory;
//
//Mesh::Mesh() : 
//	VAO(0), 
//	VBO(0), 
//	EBO(0)
//{}
//
//Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<u32>& indices, std::vector<OGLTexture2D>& textures) :
//	vertices(vertices), 
//	indices(indices), 
//	textures(textures)
//{
//	VAO = OGL::VAO();
//	VBO = OGL::VBO();
//	EBO = OGL::EBO();
//
//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glEnableVertexAttribArray(2);
//}
//
//std::vector<OGLTexture2D> LoadMaterialTextures(aiMaterial& material, aiTextureType type)
//{
//	std::vector<OGLTexture2D> textures;
//	for (u32 i = 0; i < material.GetTextureCount(type); i++)
//	{
//		aiString str;
//		material.GetTexture(type, i, &str);
//		OGLTexture2D texture = OGLTexture2D((directory + str.C_Str()).c_str());
//		//texture.type = typeName;
//		textures.push_back(texture);
//	}
//	return textures;
//}
//
//static void ConvertAIMesh(aiScene const& scene, aiMesh const& aiMesh, Model& _model)
//{
//	std::vector<Mesh::Vertex>			vertices;
//	std::vector<u32>			indices;
//	std::vector<OGLTexture2D>	textures; 
//	
//	for (u32 i = 0; i < aiMesh.mNumVertices; i++)
//	{ 
//		Mesh::Vertex vtx;
//		vtx.position.x = aiMesh.mVertices[i].x;
//		vtx.position.y = aiMesh.mVertices[i].y;
//		vtx.position.z = aiMesh.mVertices[i].z;
//
//		if (aiMesh.HasNormals())
//		{
//			vtx.normal.x = aiMesh.mNormals[i].x;
//			vtx.normal.y = aiMesh.mNormals[i].y;
//			vtx.normal.z = aiMesh.mNormals[i].z;
//		}
//
//		if (aiMesh.HasTextureCoords(0))
//		{
//			vtx.texCoord.x = aiMesh.mTextureCoords[0][i].x;
//			vtx.texCoord.y = aiMesh.mTextureCoords[0][i].y;
//		}
//		vertices.push_back(vtx); 
//	}
//
//	for (u32 i = 0; i < aiMesh.mNumFaces; i++)
//	{
//		aiFace face = aiMesh.mFaces[i];
//		for (u32 j = 0; j < face.mNumIndices; j++)
//		{
//			indices.push_back(face.mIndices[j]);
//		}
//	}
//
//	// load materials:
//	if (aiMesh.mMaterialIndex >= 0)
//	{
//		aiMaterial& material = *scene.mMaterials[aiMesh.mMaterialIndex];
//		std::vector<OGLTexture2D> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
//		//std::vector<OGLTexture2D> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
//		//std::vector<OGLTexture2D> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS);
//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//		//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//	}
//
//	//mesh = Mesh(vertices, indices); 
//	_model.meshes.push_back(Mesh(vertices, indices, textures));
//}
//
//static void TraverseAINode(aiNode const& node, aiScene const& scene, Model& model)
//{
//	for (int i = 0; i < node.mNumMeshes; i++)
//	{
//		aiMesh const& aiMesh = *scene.mMeshes[node.mMeshes[i]];
//		//model.meshes.emplace_back(); 
//		ConvertAIMesh(scene, aiMesh, model);
//	}
//	for (int i = 0; i < node.mNumChildren; i++)
//	{
//		TraverseAINode(*node.mChildren[i], scene, model);
//	}
//}
//
//static void Load(char const* path, Model& model)
//{
//	static uint constexpr PROCESS_FLAGS =
//		aiProcess_Triangulate |
//		//aiProcess_DropNormals |  
//		//aiProcess_FixInfacingNormals |
//		//aiProcess_GenNormals |
//		//aiProcess_GenSmoothNormals |  
//		//aiProcess_MakeLeftHanded	|
//		aiProcess_FlipUVs;
//		//aiProcess_CalcTangentSpace;
//
//	Assimp::Importer importer;
//	aiScene const* scene = importer.ReadFile(path, PROCESS_FLAGS);
//
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		printf(importer.GetErrorString());
//		return;
//	}
//	std::string p = path;
//	directory = p.substr(0, p.find_last_of('/') + 1);
//	TraverseAINode(*scene->mRootNode, *scene, model);
//	printf("Loaded succesfully\n");
//}
//
//Model::Model(char const* path)
//{
//	Load(path, *this);
//}

void LoadMaterialTexture(aiMaterial const& aiMat, aiTextureType const aiType, u8 const type, std::string const& dir, Material& mat)
{
	for (u32 i = 0; i < aiMat.GetTextureCount(aiType); ++i)
	{
		aiString str; aiMat.GetTexture(aiType, i, &str);
		mat.texs[type] = OGLTexture2D(str.C_Str(), dir);
	}
}

void Material::Cleanup()
{
	glDeleteTextures(1, &texs[0].ID);
}
