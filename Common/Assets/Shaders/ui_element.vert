layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 model;

out vec2 texCoord;

void main() {
    gl_Position = ortho * model * vec4(aPosition, 0.9, 1.0);
    texCoord = aTexCoord;
}
