in vec2 texCoord;

uniform mediump sampler2DArray	perlin;
uniform vec3					color;
uniform int						layer1;
uniform int						layer2;
uniform float					t;

out vec4 fragColor;

void main()                                         
{
	float value1 = texture(perlin, vec3(texCoord, layer1)).r; 
	float value2 = texture(perlin, vec3(texCoord, layer2)).r; 
	float interp = value1 * (1.0 - t) + value2 * t;
	vec3 final = color * interp;
	fragColor = vec4(final, interp);
}