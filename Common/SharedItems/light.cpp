#include "common.h"
#include "light.h"

Lights& Lights::Singleton()
{
	static Lights singleton;
	return singleton;
}

Lights::Lights() : 
	mRedAlarm(false)
{
	// generate UBOs
	mPointUBO = OGL::UBO();
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightBlock), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_LIGHT, mPointUBO);
	CheckOGL();
	// setting lights to default value
	for (int i = 0; i < POINT_LIGHT_COUNT; i++)
	{
		mPointBlock.positions[i] = vec4( 0.000f);
		mPointBlock.diffuses [i] = vec4(10.000f);
		mPointBlock.speculars[i] = vec4( 8.000f);
		mPointBlock.enabled  [i] = ivec4(0);
	}
	UploadPointAll();

	mSpotUBO = OGL::UBO();
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotlightBlock), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_SPOTLIGHT, mSpotUBO);
	CheckOGL();
	for (int i = 0; i < SPOTLIGHT_COUNT; i++)
	{
		mSpotBlock.positions [i] = vec4(0.0f);
		mSpotBlock.directions[i] = vec4(AHEAD, 0.0f);
		mSpotBlock.diffuses  [i] = vec4(60.000f);
		mSpotBlock.speculars [i] = vec4(40.000f);
		mSpotBlock.enabled   [i] = ivec4(1);
	}
	UploadSpotAll();
}

Lights::~Lights()
{
	glDeleteBuffers(1, &mPointUBO);
	glDeleteBuffers(1, &mSpotUBO);
}

void Lights::RandomlyFlickerAllSpotlights(float const dt)
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	static float chance = 0.6f;
	static float time[SPOTLIGHT_COUNT] = { 0.0f };

	for (u8 i = 0; i < SPOTLIGHT_COUNT; i++)
	{
		float const random = dist(rng);

		time[i] += dt * 5.0f;
		if (time[i] >= random)
		{
			time[i] = 0.0f;
			SetSpotEnabled(random < chance, i);
		}
	}
}

void Lights::RedAlarmAllCeilingSpotlights(float const dt)
{
	if (!mRedAlarm) return;

	static float time = 0.0f;
	static float wave = 0.0f;

	time += dt * 15.0f;
	wave = 0.5f * (1.0f + glm::sin(time));
	for (u8 i = 0; i < SPOTLIGHT_COUNT; i++)
	{
		SetSpotDiffuse(vec3(wave * 1800.0f, 350.0f, 350.0f), i);
		SetSpotSpecular(vec3(wave * 2500.0f, 300.0f, 300.0f), i);
	}
}

void Lights::ActivateAlarm()
{
	mRedAlarm = true;
}

void Lights::SetPointPosition(vec3 const position, u8 const idx)
{
	mPointBlock.positions[idx] = vec4(position, 1.0f);
	UploadPointPosition(idx);
}

void Lights::SetPointDiffuse(vec3 const diffuse, u8 const idx)
{
	mPointBlock.diffuses[idx] = vec4(diffuse, 1.0f);
	UploadPointDiffuse(idx);
}

void Lights::SetPointSpecular(vec3 const specular, u8 const idx)
{
	mPointBlock.speculars[idx] = vec4(specular, 1.0f);
	UploadPointSpecular(idx);
}

void Lights::SetPointEnabled(bool const enabled, u8 const idx)
{
	mPointBlock.enabled[idx] = ivec4(enabled);
	UploadPointEnabled(idx);
}

void Lights::SetSpotPosition(vec3 const position, u8 const idx)
{
	mSpotBlock.positions[idx] = vec4(position, 1.0f);
	UploadSpotPosition(idx);
}

void Lights::SetSpotDirection(vec3 const position, u8 const idx)
{
	mSpotBlock.directions[idx] = vec4(position, 1.0f);
	UploadSpotDirection(idx);
}

void Lights::SetSpotDiffuse(vec3 const diffuse, u8 const idx)
{
	mSpotBlock.diffuses[idx] = vec4(diffuse, 1.0f);
	UploadSpotDiffuse(idx);
}

void Lights::SetSpotSpecular(vec3 const specular, u8 const idx)
{
	mSpotBlock.speculars[idx] = vec4(specular, 1.0f);
	UploadSpotSpecular(idx);
}

void Lights::SetSpotEnabled(bool const enabled, u8 const idx)
{
	mSpotBlock.enabled[idx] = ivec4(enabled);
	UploadSpotEnabled(idx);
}

void Lights::UploadPointAll()
{
	glBindBuffer(GL_UNIFORM_BUFFER, mPointUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightBlock), &mPointBlock);
	CheckOGL();
}

void Lights::UploadPointPosition(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mPointUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(PointLightBlock, positions) + sizeof(vec4) * idx, sizeof(vec4), &mPointBlock.positions[idx]);
	CheckOGL();
}

void Lights::UploadPointDiffuse(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mPointUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(PointLightBlock, diffuses) + sizeof(vec4) * idx, sizeof(vec4), &mPointBlock.diffuses[idx]);
	CheckOGL();
}

void Lights::UploadPointSpecular(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mPointUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(PointLightBlock, speculars) + sizeof(vec4) * idx, sizeof(vec4), &mPointBlock.speculars[idx]);
	CheckOGL();
}

void Lights::UploadPointEnabled(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mPointUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(PointLightBlock, enabled) + sizeof(vec4) * idx, sizeof(vec4), &mPointBlock.enabled[idx]);
	CheckOGL();
}

void Lights::UploadSpotAll()
{
	glBindBuffer(GL_UNIFORM_BUFFER, mSpotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotlightBlock), &mSpotBlock);
	CheckOGL();
}

void Lights::UploadSpotPosition(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mSpotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SpotlightBlock, positions) + sizeof(vec4) * idx, sizeof(vec4), &mSpotBlock.positions[idx]);
	CheckOGL();
}

void Lights::UploadSpotDirection(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mSpotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SpotlightBlock, directions) + sizeof(vec4) * idx, sizeof(vec4), &mSpotBlock.directions[idx]);
	CheckOGL();
}

void Lights::UploadSpotDiffuse(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mSpotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SpotlightBlock, diffuses) + sizeof(vec4) * idx, sizeof(vec4), &mSpotBlock.diffuses[idx]);
	CheckOGL();
}

void Lights::UploadSpotSpecular(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mSpotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SpotlightBlock, speculars) + sizeof(vec4) * idx, sizeof(vec4), &mSpotBlock.speculars[idx]);
	CheckOGL();
}

void Lights::UploadSpotEnabled(u8 const idx)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mSpotUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SpotlightBlock, enabled) + sizeof(vec4) * idx, sizeof(vec4), &mSpotBlock.enabled[idx]);
	CheckOGL();
}
