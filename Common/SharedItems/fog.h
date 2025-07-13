#pragma once

using FogMesh = Mesh<vec3>;
struct FogModel : public Model<FogMesh>
{
	OGLTexture2DArray animatedNoise;

	FogModel();
	~FogModel(); 
	FogModel(PerlinData const& data);
};

class FogInstance : public ModelInstance<FogModel>
{
public:
	vec3	mPosition;
	vec3	mColor;
	vec2	mSize;
	float	mTime;
	float	mT;
	float	mRate;
	int		mLayer1;
	int		mLayer2;

public:
	FogInstance() = default;
	FogInstance(FogModel* model);
	virtual void Tick(f32 const dt) override;
	virtual void Render() const override;

private:
	void Reset();
};

class FogNode : public ModelNode<FogModel>
{
public:
	vec3	mPosition;
	vec3	mColor;
	vec2	mSize;
	float	mTime;
	float	mT;
	float	mRate;
	int		mLayer1;
	int		mLayer2;

public:
	FogNode();
	~FogNode();
	virtual void Tick(float const dt) override;
	virtual void Render() const override;

private:
	void Reset();
};