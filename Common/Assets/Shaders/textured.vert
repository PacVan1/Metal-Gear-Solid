layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

out vec2 texCoord;

void main()                  
{ 					
	texCoord = aTexCoord;
	gl_Position = VP * model * vec4(aPosition, 1.0);
}