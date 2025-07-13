#include "common.h"
#include "fog.h"

static void UploadMesh(FogMesh& m)
{
	m.VAO = OGL::VAO();
	m.VBO = OGL::VBO();
	m.EBO = OGL::EBO();
	glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(vec3), m.vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(u32), m.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	CheckOGL();
}

FogModel::FogModel()
{
	PerlinData p;
	p.width = 100;
	p.height = 100;
	p.depth = 10;
	p.octaves = 4;
	p.cellSize = 50;
	p.cellSizeZ = 5;
	p.seed = 1000;
	ComputeHelperValues(p);

	vector<Texture<u8>> layers;
	GenPerlinTextureArrayMT(p, layers);
	animatedNoise = OGLTexture2DArray(layers);
	for (Texture<u8>& layer : layers) Destruct(layer);

	meshes.emplace_back();
	FogMesh& m = meshes.back();

	m.vertices.reserve(4);
	m.vertices.resize(4);
	m.vertices[0] = vec3(0, 0, 1);
	m.vertices[1] = vec3(1, 0, 1);
	m.vertices[2] = vec3(1, 0, 0);
	m.vertices[3] = vec3(0, 0, 0);

	m.indices.reserve(6);
	m.indices.resize(6);
	m.indices[0] = 0;
	m.indices[1] = 1;
	m.indices[2] = 3;
	m.indices[3] = 3;
	m.indices[4] = 1;
	m.indices[5] = 2;

	UploadMesh(m);
}

FogModel::~FogModel()
{
	Cleanup();
	glDeleteTextures(1, &animatedNoise.ID);
}

FogModel::FogModel(PerlinData const& data)
{
	vector<Texture<u8>> layers;
	GenPerlinTextureArrayMT(data, layers);
	animatedNoise = OGLTexture2DArray(layers);
	for (Texture<u8>& layer : layers) Destruct(layer);

	meshes.emplace_back();
	FogMesh& m = meshes.back();

	m.vertices.reserve(4);
	m.vertices.resize(4);
	m.vertices[0] = vec3(0, 0, 1);
	m.vertices[1] = vec3(1, 0, 1);
	m.vertices[2] = vec3(1, 0, 0);
	m.vertices[3] = vec3(0, 0, 0);

	m.indices.reserve(6);
	m.indices.resize(6);
	m.indices[0] = 0;
	m.indices[1] = 1;
	m.indices[2] = 3;
	m.indices[3] = 3;
	m.indices[4] = 1;
	m.indices[5] = 2;

	UploadMesh(m);
}

FogInstance::FogInstance(FogModel* model) : 
	mPosition(-1326.0f, 137.75f, -462.25f),
	mColor(1.0f),
	mSize(1300.0f, 828.75f),
	mRate(1.6f)
{
	mModel = model;

	Reset();
}

void FogInstance::Tick(f32 const dt)
{
	if (!GS.mFog || !mModel) return;

	mTime += dt * mRate;
	if (mTime > 1.0f)
	{
		mLayer1 = mLayer2;
		mLayer2 = (mLayer2 + 1) % mModel->animatedNoise.layerCount;
		mTime -= 1.0f;
	}
	mT = mTime / 1.0f;
}

void FogInstance::Render() const
{
	static u32 const PROGRAM = Shader::LoadAndBind("fog", { BINDING_CAM });

	// no fog or no model
	if (!GS.mFog || !mModel) return;

	Shader::Bind(PROGRAM);
	Shader::SetFloat("t", mT);
	Shader::SetInt("layer1", mLayer1);
	Shader::SetInt("layer2", mLayer2);
	Shader::SetVec3("color", mColor);
	Shader::SetVec3("position", mPosition);
	Shader::SetVec2("size", mSize);
	Shader::SetTexture2DArray("perlin", mModel->animatedNoise);
	for (FogMesh const& mesh : mModel->meshes)
	{
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}
}

void FogInstance::Reset()
{
	mLayer1 = 0;
	mLayer2 = 1 % mModel->animatedNoise.layerCount;
	mT		= 0.0f;
	mTime	= 0.0f;
}

FogNode::FogNode() :
	mPosition(-1326.0f, 137.75f, -462.25f),
	mColor(1.0f),
	mSize(1300.0f, 828.75f),
	mRate(1.6f)
{
	SetName("Fog");

	mModel = new FogModel();
	Reset();
}

FogNode::~FogNode()
{
	delete mModel;
}

void FogNode::Tick(float const dt)
{
	if (!GS.mFog || !mModel) return;

	mTime += dt * mRate;
	if (mTime > 1.0f)
	{
		mLayer1 = mLayer2;
		mLayer2 = (mLayer2 + 1) % mModel->animatedNoise.layerCount;
		mTime -= 1.0f;
	}
	mT = mTime / 1.0f;
}

void FogNode::Render() const
{
	static u32 const PROGRAM = Shader::LoadAndBind("fog_with_light", { BINDING_CAM, BINDING_POINT_LIGHT, BINDING_SPOTLIGHT });

	// no fog or no model
	if (!GS.mFog || !mModel) return;

	Shader::Bind(PROGRAM);
	Shader::SetFloat("t", mT);
	Shader::SetInt("layer1", mLayer1);
	Shader::SetInt("layer2", mLayer2);
	Shader::SetVec3("color", mColor);
	Shader::SetVec3("position", mPosition);
	Shader::SetVec2("size", mSize);
	Shader::SetTexture2DArray("perlin", mModel->animatedNoise);
	for (FogMesh const& mesh : mModel->meshes)
	{
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}
}

void FogNode::Reset()
{
	mLayer1 = 0;
	mLayer2 = 1 % mModel->animatedNoise.layerCount;
	mT = 0.0f;
	mTime = 0.0f;
}
