#include "common.h"
#include "gui_element_3d.h"

GUIElementNode3D::GUIElementNode3D(char const* name) :
	ModelNode(&RM().GetStaticModel(name))
{}

void GUIElementNode3D::Render() const
{
	static u32 const PROGRAM = Shader::LoadAndBind("unified_shader", { BINDING_CAM }, { "STATIC", "UI_ELEMENT" });

	if (!mModel) return;

	Shader::Bind(PROGRAM);
	Shader::SetMat4("model", GetWorldTransform());
	Shader::SetVec3("color", vec3(1.0f, 0.0f, 0.0f));
	for (StaticMesh const& mesh : mModel->meshes)
	{
		Shader::SetTexture2D("diffuse", mesh.mat.texs[TEXTURE_TYPES_ALBEDO]);
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	}
}
