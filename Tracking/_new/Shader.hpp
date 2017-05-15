#pragma once

using namespace std;

// Shaders
class Shader
{
public:
	//Program ID;
	GLuint Program;
	//constructeur
	Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);
	void Use();
};
