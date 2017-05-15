#include "Shader.hpp"

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Shader::Shader(const GLchar * vertexSourcePath, const GLchar * fragmentSourcePath)
{
  std::string vertexCode;
  std::string fragmentCode;

  try
    {
      std::ifstream vShaderFile(vertexSourcePath);
      std::ifstream fShaderFile(fragmentSourcePath);

      std::stringstream vShaderStream, fShaderStream;
      //read the file to the stream
      vShaderStream << vShaderFile.rdbuf();//read the file
      fShaderStream << fShaderFile.rdbuf();//read the file

      //release the handle of the file streams
      vShaderFile.close();
      fShaderFile.close();

      //transform the stream to a GLchar table
      vertexCode = vShaderStream.str();//stream to string
      fragmentCode = fShaderStream.str();
    }
  catch (const std::exception&)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

  const GLchar* vertexShaderCode = vertexCode.c_str();//string to char
  const GLchar* fragmentShaderCode = fragmentCode.c_str();//string to char
  //compile the shders
  GLuint vertex, fragment;
  GLint success;
  GLchar infoLog[512];
  //vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexShaderCode, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success)
    {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
  //fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success)
    {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      std::cout<< "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

  Program = glCreateProgram();
  glAttachShader(Program, vertex);
  glAttachShader(Program, fragment);
  glLinkProgram(Program);
  glGetProgramiv(Program, GL_LINK_STATUS, &success);
  if(!success)
    {
      glGetProgramInfoLog(Program, 512, NULL, infoLog);
      std::cout<< "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Use()
{
  glUseProgram(Program);
}
