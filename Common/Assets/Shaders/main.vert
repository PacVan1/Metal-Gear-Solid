layout (location = 0) in vec2 p;

out vec2 u;

void main()
{
	u			= vec2((p.x + 1.0) / 2.0, (p.y + 1.0) / 2.0);
	gl_Position	= vec4(p.x, p.y, 1.0, 1.0);
}