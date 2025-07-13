#pragma once

class GUI2
{
public:
	GLuint mQuadVAO;
	GLuint mQuadVBO;

public:
	static GUI2& Singleton();

public:
	GUI2();
	~GUI2();
	void Render() const;
};