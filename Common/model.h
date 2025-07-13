#pragma once

enum textureTypes : u8
{
	TEXTURE_TYPES_ALBEDO, 
	TEXTURE_TYPES_COUNT, 
};

struct Material
{
	OGLTexture2D texs[TEXTURE_TYPES_COUNT];

	void Cleanup();
};

template <typename VertexType>
struct Mesh
{
	vector<VertexType>	vertices;
	vector<u32>			indices;
	// MYTODO make indices pointing to materials instead of having them
	Material			mat;
	GLuint				VAO, VBO, EBO;

	void Cleanup();
};

template <typename MeshType>
struct Model
{
	vector<MeshType> meshes;

	void Cleanup();
	virtual void Render(mat4 const& transform) const {}
};

class ModelInstanceBase
{
public:
	virtual void Tick(f32 const) {}
	virtual void Render() const = 0;
};

template <typename ModelType>
class ModelInstance : public ModelInstanceBase
{
public:
	Transform	mTransform;

protected:
	ModelType*	mModel;

public:
	ModelInstance() : mModel(nullptr) {}

	inline vec3 GetRight() const		{ return mTransform.GetRight(); }
	inline vec3 GetUp() const			{ return mTransform.GetUp(); }
	inline vec3 GetAhead() const		{ return mTransform.GetAhead(); }
	inline vec3 GetPosition() const		{ return mTransform.GetWorldPosition(); }
	inline Transform& GetTransform() 	{ return mTransform; }
};

void LoadMaterialTexture(aiMaterial const& aiMat, aiTextureType const aiType, u8 const type, std::string const& dir, Material& mat);

template <typename MeshType>
class NewModel
{
private:
	vector<MeshType> mMeshes;

public:
	virtual void Tick(float const dt) {}
	virtual void Render(mat4 const& transform) const = 0;
};

template <typename ModelType>
class ModelNode : public SGNode
{
protected:
	ModelType* mModel;

public:
	ModelNode() : mModel(nullptr) {}
	ModelNode(ModelType* model) : mModel(model) {}
	virtual void Render() const override;
};

template<typename ModelType>
inline void ModelNode<ModelType>::Render() const
{
	if (!mIsVisible) return;
	mModel->Render(GetWorldTransform());
	ChildrenRender();
}

template<typename VertexType>
inline void Mesh<VertexType>::Cleanup()
{
	mat.Cleanup();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

template<typename MeshType>
inline void Model<MeshType>::Cleanup()
{
	for (auto& mesh : meshes)
	{
		mesh.Cleanup();
	}
}
