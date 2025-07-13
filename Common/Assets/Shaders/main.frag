uniform sampler2D c;

in vec2 u;

layout(location = 0) out vec4 f;

void main()
{
	f = texture(c, u);
} 