layout (location = 0) in vec3 aPosition;

out vec3 texCoord;

void main() 
{
    texCoord    = aPosition;
    gl_Position = projection * mat4(mat3(view)) * vec4(aPosition, 1.0);
}
