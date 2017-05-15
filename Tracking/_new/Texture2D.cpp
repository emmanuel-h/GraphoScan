#include "Texture2D.hpp"

#include <soil.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

Texture2D::Texture2D(const GLchar* picturePath, int textTureNum, ModeTexture2DWrap modeWrapS, ModeTexture2DWrap modeWrapT, ModeTexture2DFilter minFilter, ModeTexture2DFilter magFilter) {
  myTexTureNum = textTureNum;
  glActiveTexture(TextureX[myTexTureNum]);
  glGenTextures(1, &myTexture2D);
  glBindTexture(GL_TEXTURE_2D, myTexture2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ModeWrap[modeWrapS]);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ModeWrap[modeWrapT]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ModeFilter[minFilter]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ModeFilter[magFilter]);

  unsigned char* image = SOIL_load_image(picturePath, &textWidth, &textHeight, 0, SOIL_LOAD_RGB);
  //bind the texture image with texture 2D
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textWidth, textHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  //unbind the texture 2D and release the memory
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D()
{
		
}

void Texture2D::InitTexture(const GLchar* picturePath, int textTureNum, ModeTexture2DWrap modeWrapS, ModeTexture2DWrap modeWrapT, ModeTexture2DFilter minFilter, ModeTexture2DFilter magFilter) {
  myTexTureNum = textTureNum;
  glActiveTexture(TextureX[myTexTureNum]);
  glGenTextures(1, &myTexture2D);
  glBindTexture(GL_TEXTURE_2D, myTexture2D);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ModeWrap[modeWrapS]);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ModeWrap[modeWrapT]);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ModeFilter[minFilter]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ModeFilter[magFilter]);

  unsigned char* image = SOIL_load_image(picturePath, &textWidth, &textHeight, 0, SOIL_LOAD_RGB);
  //bind the texture image with texture 2D
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textWidth, textHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  //unbind the texture 2D and release the memory
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::UseTexture2D(GLuint shaderProgram, const GLchar* textureName)
{
  glActiveTexture(TextureX[myTexTureNum]);
  glBindTexture(GL_TEXTURE_2D, myTexture2D);
  glUniform1i(glGetUniformLocation(shaderProgram, textureName), 1);
}
