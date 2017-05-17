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

class myOpenGL
{
public:
  GLuint winWidth;
  GLuint winHeight;
  GLFWwindow* window;
  GLuint VAO1, VAO2;
  GLuint VBO1, VBO2;
  GLuint EBO1;
  //Shader shader1;
  //Shader shader2;
  glm::mat4 myModel;
  glm::mat4 myView;
  glm::mat4 myProjection;
  
  //Texture2D texture1;
  //Texture2D texture2;
  myOpenGL(const GLuint width = WIDTH, const GLuint height = HEIGHT){
    winWidth = WIDTH;
    winHeight = HEIGHT;
    //texture1.InitTexture("wall.jpg");
    //texture2.InitTexture("baboon.jpg", 1);
  };
  
  ~myOpenGL(){};
  
  void InitWindow();

  void InitVertex();
  
  void RunGL(const char* filename/*, const char* filename_bg*/);
  
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
