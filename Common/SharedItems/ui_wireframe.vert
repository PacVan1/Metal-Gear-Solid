layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 model;

void main()                  
{ 
	gl_Position = projection * vec4(aPosition.x, aPosition.y, aPosition.z - 10.0, 1.0);
}