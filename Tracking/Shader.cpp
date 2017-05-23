#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

Shader::Shader(const GLchar * vertexSourcePath, const GLchar * fragmentSourcePath) {
  string vertexCode;
  string fragmentCode;

  try {
    ifstream vShaderFile(vertexSourcePath);
    ifstream fShaderFile(fragmentSourcePath);
    
    stringstream vShaderStream, fShaderStream;
    //read the file to the stream
    vShaderStream << vShaderFile.rdbuf();//read the file
    fShaderStream << fShaderFile.rdbuf();//read the file
    
    //release the handle of the file streams
    vShaderFile.close();
    fShaderFile.close();
    
    //transform the stream to a GLchar table
    vertexCode = vShaderStream.str();//stream to string
    fragmentCode = fShaderStream.str();
  } catch (const exception&) {
    cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ0" << endl;
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
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    cout <<"ERROR::SHADER::VERTEX::COMPILATION_FAILED1\n" << infoLog << endl;
  }
  
  //fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED2\n" << infoLog << endl;
  }

  programID = glCreateProgram();
  glAttachShader(programID, vertex);
  glAttachShader(programID, fragment);
  glLinkProgram(programID);
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programID, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED3\n" << infoLog << endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Use() {
  glUseProgram(programID);
}
