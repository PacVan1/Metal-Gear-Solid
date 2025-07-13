#include "common.h"
#include "gui2.h"

GUI2& GUI2::Singleton()
{
    static GUI2 singleton;
    return singleton;
}

GUI2::GUI2()
{
	mQuadVAO = OGL::VAO();
	mQuadVBO = OGL::VBO();
    static float QUAD_VERTS[] = 
    {
        // pos      // uv
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    glGenVertexArrays(1, &mQuadVAO);
    glBindVertexArray(mQuadVAO);
    glGenBuffers(1, &mQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    CheckOGL();
}

GUI2::~GUI2()
{
    glDeleteBuffers(1, &mQuadVBO);
    glDeleteVertexArrays(1, &mQuadVAO);
}

void GUI2::Render() const
{
    static GLuint const PROGRAM = Shader::LoadAndBind("ui_element", { BINDING_CAM });

    mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.1f));
    model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));

    Shader::Bind(PROGRAM);
    Shader::SetMat4("model", model);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(mQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}
