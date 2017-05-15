#pragma once

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

class Texture2D {
public:
  int textHeight;
  int textWidth;
  int myTexTureNum;

  GLuint myTexture2D;
  Texture2D(const GLchar* picturePath, int textTureNum = 0, ModeTexture2DWrap modeWrapS = MIRRORED_REPEAT, ModeTexture2DWrap modeWrapT = MIRRORED_REPEAT, ModeTexture2DFilter minFilter = NEAREST, ModeTexture2DFilter magFilter = LINEAR);
  
  Texture2D();
  
  void InitTexture(const GLchar* picturePath, int textTureNum = 0, ModeTexture2DWrap modeWrapS = MIRRORED_REPEAT, ModeTexture2DWrap modeWrapT = MIRRORED_REPEAT, ModeTexture2DFilter minFilter = NEAREST, ModeTexture2DFilter magFilter = LINEAR);
  
  void UseTexture2D(GLuint shaderProgram, const GLchar* textureName);

};
