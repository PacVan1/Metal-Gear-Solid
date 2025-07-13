#include "common.h"
#include "skybox.h"

static u32 constexpr gUNIT_CUBE_VERT_COUNT = 36;
static float constexpr gUNIT_CUBE_VERTS[gUNIT_CUBE_VERT_COUNT * 3] =
{     
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void Skybox::Init()
{
    sCubeVAO = OGL::VAO();
    sCubeVBO = OGL::VBO();
    glBufferData(GL_ARRAY_BUFFER, gUNIT_CUBE_VERT_COUNT * sizeof(float) * 3, gUNIT_CUBE_VERTS, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);
    CheckOGL();
}

void Skybox::Cleanup()
{
    glDeleteBuffers(1, &sCubeVBO);
    glDeleteVertexArrays(1, &sCubeVAO);
}

Skybox::Skybox(char const* path) :
    mCubeMap(path)
{}

void Skybox::Render() const
{
	static GLuint const PROGRAM = Shader::LoadAndBind("skybox", { BINDING_CAM });

	glDepthMask(GL_FALSE);
	Shader::Bind(PROGRAM);
    Shader::SetCubeMap("cubeMap", mCubeMap.ID);
    glBindVertexArray(sCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}
