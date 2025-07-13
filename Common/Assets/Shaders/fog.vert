layout (location = 0) in vec3 aPosition;

uniform vec3 position;
uniform vec2 size;

out vec2 texCoord;

void main()                  
{ 	
	float p1	= aPosition.x * size.x;
	float p2	= aPosition.z * size.y;
	vec4 pos	= vec4(vec3(p1, 0.0, p2) + position, 1.0);
	texCoord	= pos.xz / 125.0; 
	gl_Position	= VP * pos;
}