#pragma once

#include <soil.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

const GLchar ModeWrap[] = { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };
const GLchar ModeFilter[] = { GL_NEAREST, GL_LINEAR };

const GLfloat TextureX[] = {
	GL_TEXTURE0,
	GL_TEXTURE1,
	GL_TEXTURE2,
	GL_TEXTURE3
};

enum ModeTexture2DWrap
{
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER
};

enum ModeTexture2DFilter
{
	NEAREST,
	LINEAR
};

class Texture2D
{
public:
	int textHeight;
	int textWidth;
	int myTexTureNum;

	GLuint myTexture2D;
	Texture2D(const GLchar* picturePath, int textTureNum = 0, ModeTexture2DWrap modeWrapS = MIRRORED_REPEAT, ModeTexture2DWrap modeWrapT = MIRRORED_REPEAT,
		ModeTexture2DFilter minFilter = NEAREST, ModeTexture2DFilter magFilter = LINEAR)
	{
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
	Texture2D()
	{
		
	}
	void InitTexture(const GLchar* picturePath, int textTureNum = 0, ModeTexture2DWrap modeWrapS = MIRRORED_REPEAT, ModeTexture2DWrap modeWrapT = MIRRORED_REPEAT,
		ModeTexture2DFilter minFilter = NEAREST, ModeTexture2DFilter magFilter = LINEAR)
	{
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
	void UseTexture2D(GLuint shaderProgram, const GLchar* textureName)
	{
		glActiveTexture(TextureX[myTexTureNum]);
		glBindTexture(GL_TEXTURE_2D, myTexture2D);
		glUniform1i(glGetUniformLocation(shaderProgram, textureName), 1);
	}
private:

};
