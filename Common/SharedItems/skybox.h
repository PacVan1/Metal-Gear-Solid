#pragma once

class Skybox
{
private:
	inline static GLuint sCubeVAO = 0;
	inline static GLuint sCubeVBO = 0;

public:
	OGLCubeMap mCubeMap;

public:
	static void Init(); 
	static void Cleanup();

public:
			Skybox() = default;
			Skybox(char const* path);
	void	Render() const;
};