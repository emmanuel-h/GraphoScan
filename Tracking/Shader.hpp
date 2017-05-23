#include <GL/glew.h>

/*!
 * \class Shader
 * \brief Class for Shader Management
 */
class Shader {
public:
  GLuint programID;
  
  /*!
   * \fn Shader (const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath)
   * \brief Class constructor
   * \param vertexSourcePath path to vertex shader
   * \param vertexSourcePath path to fragment shader
   */
  Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);
  
  /*!
   * \fn void Use()
   * \brief Link the Shader with his programID
   */
  void Use();
};
