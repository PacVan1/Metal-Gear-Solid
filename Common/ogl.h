#pragma once

#ifdef WINDOWS_BUILD
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#endif

static char const* const GLSL_VERSION		= "#version 310 es";
static char const* const FLOAT_PRECISION	= "precision highp float;";

static char const* const IMGUI_GLSL_VERSION = "#version 100"; // 310/320es not available
static std::string const DEFAULT_SHADER_DIR = "../Common/Assets/Shaders/";

void _CheckOGL(char const* file, int line);
#define CheckOGL() _CheckOGL(__FILE__, __LINE__);

/// OpenGL texture wrapper class
struct OGLTexture2D
{
	GLuint	ID;
	GLint	width;
	GLint	height;

	OGLTexture2D() = default;
	OGLTexture2D(char const* path);
	OGLTexture2D(char const* name, std::string const& dir);
};

/// OpenGL texture wrapper class
struct OGLTexture2DArray
{
	GLuint	ID;
	GLint	width;
	GLint	height;
	GLint	layerCount;

	OGLTexture2DArray() = default;
	OGLTexture2DArray(vector<Texture<u8>> const& layers);
};

/// OpenGL cubemap wrapper class
struct OGLCubeMap
{
	GLuint ID;
	GLint width;
	GLint height;

	OGLCubeMap() = default;
	OGLCubeMap(char const* path);
	OGLCubeMap(char const* path, std::string const& dir);
};

/// OpenGL common functions wrapper class
class OGL
{
public:
	static void		Init();
	static void		Clear();
	static GLuint	VAO(); // vertex array
	static GLuint	VBO(); // vertex buffer
	static GLuint	EBO(); // element buffer
	static GLuint	UBO(); // uniform buffer
	static GLuint	FBO(); // frame	buffer
	static GLuint	RBO(); // render buffer
	static GLuint	Texture2D();
	static GLuint	Texture2DArray();
	static GLuint	CubeMap();
};

using Defs	= vector<std::string> const;
using Binds = vector<u8> const;

enum UBOBindings : GLuint
{
	BINDING_CAM, 
	BINDING_POINT_LIGHT,
	BINDING_SPOTLIGHT,
};

/// OpenGL shader wrapper class
class Shader
{
private:
	inline static GLuint sProgram = 0;

public:
	static GLuint	Load(char const* vertPath, char const* fragPath, Defs& defs = {});
	static GLuint	Load(char const* name, Defs& defs = {}, std::string const& dir = DEFAULT_SHADER_DIR);
	static GLuint	LoadAndBind(char const* vertPath, char const* fragPath, Binds& binds = {}, Defs & defs = {});
	static GLuint	LoadAndBind(char const* name, Binds& binds = {}, Defs& defs = {}, std::string const& dir = DEFAULT_SHADER_DIR);
	static GLuint	GetBlockIndex(char const* name);
	static GLuint	GetLoc(char const* name);
	static void		Bind(GLuint const programID);
	static void		Unbind();
	static void		Delete(GLint const programID);
	static void		SetBool(char const* name, GLboolean const value);
	static void		SetBool(GLint const loc, GLboolean const value);
	static void		SetInt(char const* name, GLint const value);
	static void		SetInt(GLint const loc, GLint const value);
	static void		SetUInt(char const* name, GLuint const value);
	static void		SetUInt(GLint const loc, GLuint const value);
	static void		SetFloat(char const* name, GLfloat const value);
	static void		SetFloat(GLint const loc, GLfloat const value);
	static void		SetMat4(char const* name, mat4 const& value);
	static void		SetMat4(GLint const loc, mat4 const& value);
	static void		SetVec2(char const* name, vec2 const& value);
	static void		SetVec2(char const* name, float const x, float const y);
	static void		SetVec2(GLint const loc, vec2 const& value);
	static void		SetVec2(GLint const loc, float const x, float const y);
	static void		SetVec3(char const* name, vec3 const& value);
	static void		SetVec3(char const* name, float const x, float const y, float const z);
	static void		SetVec3(GLint const loc, vec3 const& value);
	static void		SetVec3(GLint const loc, float const x, float const y, float const z);
	static void		SetTexture2D(char const* name, OGLTexture2D const& tex, GLuint const slot = 0);
	static void		SetTexture2D(char const* name, GLuint const tex, GLuint const slot = 0);
	static void		SetTexture2D(GLint const loc, OGLTexture2D const& tex, GLuint const slot = 0);
	static void		SetTexture2DArray(char const* name, OGLTexture2DArray const& tex, GLuint const slot = 0);
	static void		SetTexture2DArray(GLint const loc, OGLTexture2DArray const& tex, GLuint const slot = 0);
	static void		SetCubeMap(char const* name, GLuint const ID, GLuint const slot = 0);
	static void		SetCubeMap(GLint const loc, GLuint const ID, GLuint const slot = 0);
	static void		SetBlock(char const* name, GLuint const point);
	static void		SetBlock(GLuint const block, GLuint const point);

	static void		PrintActiveBlocks();
};