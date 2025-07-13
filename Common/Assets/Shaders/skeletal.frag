in vec2 texCoord;

uniform sampler2D albedo;

out vec4 fragColor;

void main()                                         
{          
	fragColor = texture(albedo, texCoord);   
}