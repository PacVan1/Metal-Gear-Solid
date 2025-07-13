#pragma once

class Editor;

static u8 constexpr POINT_LIGHT_COUNT	= 1;
static u8 constexpr SPOTLIGHT_COUNT		= 11;

/// OpenGL ES 3.1 UBO point light block
struct alignas(16) PointLightBlock
{
	vec4  positions [POINT_LIGHT_COUNT];
	vec4  diffuses	[POINT_LIGHT_COUNT];
	vec4  speculars	[POINT_LIGHT_COUNT];
	ivec4 enabled	[POINT_LIGHT_COUNT];
};

/// OpenGL ES 3.1 UBO spotlight block
struct alignas(16) SpotlightBlock
{
	vec4  positions	 [SPOTLIGHT_COUNT];
	vec4  directions [SPOTLIGHT_COUNT];
	vec4  diffuses	 [SPOTLIGHT_COUNT];
	vec4  speculars	 [SPOTLIGHT_COUNT];
	ivec4 enabled	 [SPOTLIGHT_COUNT];
};

/// Light manager
class Lights
{
friend class Editor;
private:
	PointLightBlock mPointBlock;
	GLuint			mPointUBO;
	SpotlightBlock  mSpotBlock;
	GLuint			mSpotUBO;

	bool			mRedAlarm;

public:
	static Lights& Singleton();

public:
			Lights();
			~Lights();

	void	RandomlyFlickerAllSpotlights(float const dt);
	void	RedAlarmAllCeilingSpotlights(float const dt);
	void	ActivateAlarm();

	void	SetPointPosition(vec3 const position, u8 const idx);
	void	SetPointDiffuse(vec3 const diffuse, u8 const idx);
	void	SetPointSpecular(vec3 const specular, u8 const idx);
	void	SetPointEnabled(bool const enabled, u8 const idx);

	void	SetSpotPosition(vec3 const position, u8 const idx);
	void	SetSpotDirection(vec3 const position, u8 const idx);
	void	SetSpotDiffuse(vec3 const diffuse, u8 const idx);
	void	SetSpotSpecular(vec3 const specular, u8 const idx);
	void	SetSpotEnabled(bool const enabled, u8 const idx);

private:
	void	UploadPointAll();
	void	UploadPointPosition(u8 const idx);
	void	UploadPointDiffuse(u8 const idx);
	void	UploadPointSpecular(u8 const idx);
	void	UploadPointEnabled(u8 const idx);

	void	UploadSpotAll();
	void	UploadSpotPosition(u8 const idx);
	void	UploadSpotDirection(u8 const idx);
	void	UploadSpotDiffuse(u8 const idx);
	void	UploadSpotSpecular(u8 const idx);
	void	UploadSpotEnabled(u8 const idx);
};

#define LIGHTS Lights::Singleton()
