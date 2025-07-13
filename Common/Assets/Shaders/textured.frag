in vec2 texCoord;

uniform sampler2D diffuse;

out vec4 fragColor;

void main()                                         
{ 
	//fragColor = vec4(texCoord, 0.0, 1.0);    
	fragColor = texture(diffuse, texCoord);
}