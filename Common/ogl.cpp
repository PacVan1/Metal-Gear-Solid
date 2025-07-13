#include "common.h"
#include "ogl.h"

void _CheckOGL(char const* file, int line)
{
	switch (glGetError())
	{
	case GL_NO_ERROR: break;
	case 0x500: DebugLogLoc(ERROR, "OGL INVALID ENUM", file, line);						break;
	case 0x502: DebugLogLoc(ERROR, "OGL INVALID OPERATION", file, line);				break;
	case 0x501: DebugLogLoc(ERROR, "OGL INVALID VALUE", file, line);					break;
	case 0x506: DebugLogLoc(ERROR, "OGL INVALID FRAMEBUFFER OPERATION", file, line);	break;
	default: DebugLogLoc(ERROR, "OGL UNKNOWN ERROR", file, line);						break;
	}
}

OGLTexture2D::OGLTexture2D(char const* path)
{
	Texture<u8> texture = LoadTexture(path);
	ID					= OGL::Texture2D();
	width				= texture.width; 
	height				= texture.height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
	Destruct(texture); 
	CheckOGL();
}

OGLTexture2D::OGLTexture2D(char const* name, std::string const& dir) :
	OGLTexture2D((dir + name).c_str())
{}

/// Assuming all layers have to same dimensions as layer 0
OGLTexture2DArray::OGLTexture2DArray(vector<Texture<u8>> const& layers)
{
	ID			= OGL::Texture2DArray();
	width		= layers[0].width;
	height		= layers[0].height;
	layerCount	= static_cast<GLint>(layers.size());

	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,
		GL_R8,
		width, height, layerCount);

	for (int i = 0; i < layerCount; i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
			width, height, 1,
			GL_RED, GL_UNSIGNED_BYTE, layers[i].data);
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	CheckOGL();
}

void OGL::Init()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void OGL::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint OGL::VAO()
{
	GLuint ID;
	glGenVertexArrays(1, &ID);
	glBindVertexArray(ID);
	return ID;
}

GLuint OGL::VBO()
{
	GLuint ID; 
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	return ID;
}

GLuint OGL::EBO()
{
	GLuint ID; 
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	return ID;
}

GLuint OGL::UBO()
{
	GLuint ID;
	glGenBuffers(1, &ID);
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	return ID;
}

GLuint OGL::FBO()
{
	GLuint ID;
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	return ID;
}

GLuint OGL::RBO()
{
	GLuint ID;
	glGenRenderbuffers(1, &ID);
	glBindRenderbuffer(GL_RENDERBUFFER, ID);
	return ID;
}

GLuint OGL::Texture2D()
{
	GLuint ID; 
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	return ID;
}

GLuint OGL::Texture2DArray()
{
	GLuint ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
	return ID;
}

GLuint OGL::CubeMap()
{
	GLuint ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return ID;
}

const char* camSource = 
R"(
layout(std140) uniform CamBlock 
{
    mat4 view;
    mat4 projection;
    mat4 VP;
	vec4 camPos;
};
)";

const char* pointLightSource = 
R"(
const int POINT_LIGHT_COUNT = 1;
layout(std140) uniform PointLightBlock
{
	vec4	positions   [POINT_LIGHT_COUNT];
	vec4	diffuses    [POINT_LIGHT_COUNT];
	vec4	speculars   [POINT_LIGHT_COUNT];
	ivec4	enabled		[POINT_LIGHT_COUNT];
} point;
)";

const char* spotlightSource =
R"(
const int SPOTLIGHT_COUNT = 11;
layout(std140) uniform SpotlightBlock
{
	vec4	positions   [SPOTLIGHT_COUNT];
	vec4	directions  [SPOTLIGHT_COUNT];
	vec4	diffuses    [SPOTLIGHT_COUNT];
	vec4	speculars   [SPOTLIGHT_COUNT];
	ivec4	enabled		[SPOTLIGHT_COUNT];
} spot;
)";

struct UBODescriptor
{
	const char* block;
	const char* source;
};

UBODescriptor UBOS[] =
{
	{ "CamBlock",			camSource },
	{ "PointLightBlock",	pointLightSource },
	{ "SpotlightBlock",		spotlightSource }
};

static UBODescriptor const& GetUBO(GLuint const binding)
{
	return UBOS[binding];
}

static void LoadCode(std::string const& vertPath, std::string const& fragPath, Defs& defs, Binds& binds, std::string& vertCode, std::string& fragCode)
{
	std::ifstream vertFile;
	std::ifstream fragFile;
	vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		if (!std::filesystem::exists(vertPath) || !std::filesystem::exists(fragPath))
		{
			throw std::runtime_error("File path does not exist\n");
		}
		vertFile.open(vertPath);
		fragFile.open(fragPath);
		std::stringstream vertStream, fragStream;

		vertStream << GLSL_VERSION << endl << FLOAT_PRECISION << endl;
		for (u8 bind : binds) vertStream << GetUBO(bind).source << endl;
		for (std::string def : defs) vertStream << "#define " << def << endl;
		vertStream << vertFile.rdbuf();

		fragStream << GLSL_VERSION << endl << FLOAT_PRECISION << endl;
		for (u8 bind : binds) fragStream << GetUBO(bind).source << endl;
		for (std::string def : defs) fragStream << "#define " << def << endl;
		fragStream << fragFile.rdbuf();

		vertFile.close();
		fragFile.close();
		vertCode = vertStream.str();
		fragCode = fragStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		DebugLog(ERROR, e.what());
	}
	catch (std::runtime_error& e)
	{
		DebugLog(ERROR, e.what());
	}
}

static void PrintShaderError(GLuint const ID)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	GLsizei length = 0;
	glGetShaderInfoLog(ID, sizeof(buffer), &length, buffer);
	if (length > 0 && strstr(buffer, "ERROR")) DebugLog(ERROR, buffer);
}

static void PrintProgramError(GLuint const ID)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	GLsizei length = 0;
	glGetProgramInfoLog(ID, sizeof(buffer), &length, buffer);
	if (length > 0) DebugLog(ERROR, buffer);
}

static GLuint Compile(char const* vertCode, char const* fragCode)
{
	GLuint vertID		= glCreateShader(GL_VERTEX_SHADER);
	GLuint fragID		= glCreateShader(GL_FRAGMENT_SHADER);
	GLuint programID	= glCreateProgram();
	glShaderSource(vertID, 1, &vertCode, nullptr);
	glCompileShader(vertID);
	PrintShaderError(vertID);
	glShaderSource(fragID, 1, &fragCode, nullptr);
	glCompileShader(fragID);
	PrintShaderError(fragID);
	glAttachShader(programID, vertID);
	glAttachShader(programID, fragID);
	glLinkProgram(programID);
	PrintProgramError(programID);
	glDeleteShader(vertID);
	glDeleteShader(fragID);
	return programID;
}

GLuint Shader::Load(char const* vertPath, char const* fragPath, Defs& defs)
{
	std::string vertCode, fragCode;
	LoadCode(vertPath, fragPath, defs, {}, vertCode, fragCode);
	return Compile(vertCode.c_str(), fragCode.c_str());
}

GLuint Shader::Load(char const* name, Defs& defs, std::string const& dir)
{
	std::string exclExtension = dir + name;
	return Load((exclExtension + ".vert").c_str(), (exclExtension + ".frag").c_str(), defs);
}

GLuint Shader::LoadAndBind(char const* vertPath, char const* fragPath, Binds& binds, Defs& defs)
{
	std::string vertCode, fragCode;
	LoadCode(vertPath, fragPath, defs, binds, vertCode, fragCode);
	GLuint ID = Compile(vertCode.c_str(), fragCode.c_str());
	Bind(ID);

	// bind UBOs
	for (u8 bind : binds)
		SetBlock(GetUBO(bind).block, bind);

	return ID;
}

GLuint Shader::LoadAndBind(char const* name, Binds& binds, Defs& defs, std::string const& dir)
{
	std::string exclExtension = dir + name;
	return LoadAndBind((exclExtension + ".vert").c_str(), (exclExtension + ".frag").c_str(), binds, defs);
}

GLuint Shader::GetBlockIndex(char const* name)
{
	return glGetUniformBlockIndex(sProgram, name);
}

GLuint Shader::GetLoc(char const* name)
{
	return glGetUniformLocation(sProgram, name);
}

void Shader::Bind(GLuint const programID)
{
	sProgram = programID;
	glUseProgram(programID);
	CheckOGL();
}

void Shader::Unbind()
{
	glUseProgram(0);
	CheckOGL();
}

void Shader::Delete(GLint const programID)
{
	glDeleteProgram(programID);
	CheckOGL();
}

void Shader::SetBool(char const* name, GLboolean const value)
{
	glUniform1i(GetLoc(name), value);
	CheckOGL();
}

void Shader::SetInt(char const* name, GLint const value)
{
	glUniform1i(GetLoc(name), value);
	CheckOGL();
}

void Shader::SetUInt(char const* name, GLuint const value)
{
	glUniform1ui(GetLoc(name), value);
	CheckOGL();
}

void Shader::SetFloat(char const* name, GLfloat const value)
{
	glUniform1f(GetLoc(name), value);
	CheckOGL();
}

void Shader::SetMat4(char const* name, mat4 const& value)
{
	glUniformMatrix4fv(GetLoc(name), 1, GL_FALSE, glm::value_ptr(value));
	CheckOGL();
}

void Shader::SetVec3(char const* name, vec3 const& value)
{
	glUniform3f(GetLoc(name), value.x, value.y, value.z);
	CheckOGL();
}

void Shader::SetVec3(char const* name, float const x, float const y, float const z)
{
	glUniform3f(GetLoc(name), x, y, z);
	CheckOGL();
}

void Shader::SetTexture2D(char const* name, OGLTexture2D const& tex, GLuint const slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex.ID);
	glUniform1i(GetLoc(name), slot);
	CheckOGL();
}

void Shader::SetTexture2D(char const* name, GLuint const tex, GLuint const slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(GetLoc(name), slot);
	CheckOGL();
}

void Shader::SetBool(GLint const loc, GLboolean const value)
{
	glUniform1i(loc, value);
	CheckOGL();
}

void Shader::SetInt(GLint const loc, GLint const value)
{
	glUniform1i(loc, value);
	CheckOGL();
}

void Shader::SetUInt(GLint const loc, GLuint const value)
{
	glUniform1ui(loc, value);
	CheckOGL();
}

void Shader::SetFloat(GLint const loc, GLfloat const value)
{
	glUniform1f(loc, value);
	CheckOGL();
}

void Shader::SetMat4(GLint const loc, mat4 const& value)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	CheckOGL();
}

void Shader::SetVec2(char const* name, vec2 const& value)
{
	glUniform2f(GetLoc(name), value.x, value.y);
	CheckOGL();
}

void Shader::SetVec2(char const* name, float const x, float const y)
{
	glUniform2f(GetLoc(name), x, y);
	CheckOGL();
}

void Shader::SetVec2(GLint const loc, vec2 const& value)
{
	glUniform2f(loc, value.x, value.y);
	CheckOGL();
}

void Shader::SetVec2(GLint const loc, float const x, float const y)
{
	glUniform2f(loc, x, y);
	CheckOGL();
}

void Shader::SetVec3(GLint const loc, vec3 const& value)
{
	glUniform3f(loc, value.x, value.y, value.z);
	CheckOGL();
}

void Shader::SetVec3(GLint const loc, float const x, float const y, float const z)
{
	glUniform3f(loc, x, y, z);
	CheckOGL();
}

void Shader::SetTexture2D(GLint const loc, OGLTexture2D const& tex, GLuint const slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex.ID);
	glUniform1i(loc, slot);
	CheckOGL();
}

void Shader::SetTexture2DArray(char const* name, OGLTexture2DArray const& tex, GLuint const slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex.ID);
	glUniform1i(GetLoc(name), slot);
	CheckOGL();
}

void Shader::SetTexture2DArray(GLint const loc, OGLTexture2DArray const& tex, GLuint const slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex.ID);
	glUniform1i(loc, slot);
	CheckOGL();
}

void Shader::SetCubeMap(char const* name, GLuint const ID, GLuint const slot)
{
	SetCubeMap(GetLoc(name), ID, slot);
}

void Shader::SetCubeMap(GLint const loc, GLuint const ID, GLuint const slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glUniform1i(loc, slot);
	CheckOGL();
}

void Shader::SetBlock(char const* name, GLuint const point)
{
	glUniformBlockBinding(sProgram, GetBlockIndex(name) , point);
	CheckOGL();
}

void Shader::SetBlock(GLuint const block, GLuint const point)
{
	glUniformBlockBinding(sProgram, block, point);
	CheckOGL();
}

void Shader::PrintActiveBlocks()
{
	GLint numBlocks;
	glGetProgramiv(sProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	DebugLog(INFO, (std::string("Active Blocks: ") + std::to_string(numBlocks)).c_str());
}

OGLCubeMap::OGLCubeMap(char const* path)
{
	static std::string faces[6] = { "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png" };

	ID = OGL::CubeMap();

	for (u32 i = 0; i < 6; i++)
	{
		Texture<u8> texture = LoadTexture((path + faces[i]).c_str());
		if (!texture.data)
		{
			DebugLog(ASSET_FAILED, path);
			glDeleteTextures(1, &ID);
			return;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
		width = texture.width; height = texture.height;
		Destruct(texture);
		DebugLog(ASSET_LOADED, path);
	}
}