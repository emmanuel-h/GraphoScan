#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "Shader.hpp"
#include "Camera.hpp"

const GLuint WIDTH = 800, HEIGHT = 600;

/*!
 * \class MyOpenGL
 * \brief class for 3D reconstruct
 *
 * Class which initialize OpenGL context with GLFW 3 and 3D reconstruct
 */
class MyOpenGL {
  
public:
  //Camera size parameter
  GLuint winWidth;
  GLuint winHeight;
  GLFWwindow* window;
  //Camera attributs for OpenGL
  GLuint VAO1, VAO2;
  GLuint VBO1, VBO2;
  // PVM Matrixes
  glm::mat4 myModel;
  glm::mat4 myView;
  glm::mat4 myProjection;

  /*!
   * \fn MyOpenGL(const GLuint width, const GLuint height)
   * \brief Constructor
   * 
   * Class constructor which set window height and width
   *
   * \param width window width
   * \param height window height
   */
  MyOpenGL(const GLuint width = WIDTH, const GLuint height = HEIGHT){
    winWidth = WIDTH;
    winHeight = HEIGHT;
  };

  /*!
   * \fn ~MyOpenGL
   * \brief class destructor
   */
  ~MyOpenGL(){};

  /*!
   * \fn void InitWindow()
   * \brief OpenGL window initialization
   *
   * Window initialization with GLFW 3 function and bind callback functions
   */
  void InitWindow();

  /*!
   * \fn void InitVertex()
   * \brief OpenGL context initialization
   *
   * OpenGL context initialization, created VAOs and VBOs for 3D reconstruct
   */
  void InitVertex();

  /*!
   * \fn void RunGL(const char* filename)
   * \brief 3D reconstruct function
   *
   * OpenGL context initialization, created VAOs and VBOs for 3D reconstruct
   * 
   * \param filename filename of file which contains 3D points
   */
  void RunGL(const char* filename);
  
};

/*!
 * \fn void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
 * \brief key call-back function
 *
 * On press keyboard keys[key] = true, if it's Escape close Window
 * 
 * \param window pointer on window
 * \param key key pressed on key board
 * \param scancode must be remove
 * \param action if key pressed equals to GLFW_PRESS
 * \param mode must be remove
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

/*!
 * \fn void mouse_callback(GLFWwindow* window, double xpos, double ypos)
 * \brief mouse call-back function
 *
 * calculate xoffset and yoffset in terms of x position and y position and send it to Camera for update
 * 
 * \param window pointer on window
 * \param xpos x coordinate of mouse position
 * \param ypos y coordinate of mouse position
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/*!
 * \fn scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
 * \brief mouse scroll call-back function
 *
 * calculate yoffset in terms of y position and send it to Camera for update
 * 
 * \param window pointer on window
 * \param xoffset must be removed
 * \param yoffset offset value of mouse scroll
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/*!
 * \fn void do_movement()
 * \brief on movement (key pressed) call back function
 *
 * Call camera update function in terms of movement key event 
 */
void do_movement();
