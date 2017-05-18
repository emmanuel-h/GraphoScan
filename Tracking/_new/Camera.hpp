#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * \enum Camera_Mouvement
 * \brief possible movement of the Camera
 *
 */
enum Camera_Mouvement {
  FORWARD,     /*!< move forward */
  BACKWARD,    /*!< move backward */
  LEFT,        /*!< move on the left */
  RIGHT        /*!< move on the right */
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

/*!
 * \class Camera
 * \brief class for Camera management
 *
 * Class which initialize Camera and generate PVM matrix for 3D reconstruction
 *
 */
class Camera {
public:
  //Camera Attributes
  glm::vec3 mPosition;
  glm::vec3 mFront;
  glm::vec3 mUp;
  glm::vec3 mRight;
  glm::vec3 mWorldUp;
  //Eular Angles
  GLfloat mYaw;
  GLfloat mPitch;
  //Camera options
  GLfloat MovementSpeed;
  GLfloat MouseSensitivity;
  GLfloat Zoom;

  /*!
   * \brief Constructor
   * 
   * Class constructor with vector
   *
   * \param position new position vector
   * \param up 
   * \param yaw yaw angles
   * \param pitch pitch angles
   */
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),GLfloat yaw = YAW, GLfloat pitch = PITCH) : mFront(glm::vec3(0.0f, 0.0f, -1.0f)),MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
     mPosition = position;
  mWorldUp = up;
  mYaw = yaw;
  mPitch = pitch;
  updateCameraVectors();
  };
  
  /*!
   * \brief Constructor
   * 
   * Class constructor with scalar values
   *
   * \param xpos x coordinate of postion vector
   * \param ypos y coordinate of postion vector
   * \param zpos z coordinate of postion vector
   * \param xup x coordinate of up vector
   * \param yup y coordinate of up vector
   * \param yaw yaw angles
   * \param pitch pitch angles
   */
  Camera(GLfloat xpos, GLfloat ypos, GLfloat zpos, GLfloat xup, GLfloat yup, GLfloat zup, GLfloat yaw = YAW, GLfloat pitch = PITCH) : mFront(glm::vec3(0.0f, 0.0f, -1.0f)),MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
mPosition = glm::vec3(xpos, ypos, zpos);
  mWorldUp = glm::vec3(xup, yup, zup);
  mYaw = yaw;
  mPitch = pitch;
  updateCameraVectors();
  };

  /*!
   * \brief Create view matrix
   *
   * Returns the view matrix calculated using Eular Angles and the LookAt Matrix
   *
   * \return view matrix
   */
  glm::mat4 GetViewMatrix();

  /*!
   * \brief Create projection matrix
   *
   * Returns the projection matrix calculated using Zoom and the Projective Matrix
   *
   * \param height window height
   * \param width window width
   * \return projection matrix
   */
  glm::mat4 GetProjectionMatrix(float height = 800, float width = 600);

  /*!
   * \brief Calculate look at
   *
   * Calculate my own look at and return it
   *
   * \return window look at
   */
  glm::mat4 myLookAt();

  /*!
   * \brief Update camera on press keyboard
   *
   * Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
   *
   * \param direction camera direction
   * \param deltaTime time
   */
  void ProcessKeyboard(Camera_Mouvement direction, GLfloat deltaTime);

  /*!
   * \brief Update camera on mouse movements
   *
   * Processe input received from a mouse input system. Excepts the offset value in both the x and y direction.
   *
   * \param xoffset x coordinate of mouse offset
   * \param yoffset y coordinate of mouse offset
   * \param constrainPitch test if pitch is out of bounds
   */
  void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);

  /*!
   * \brief Update camera on mouse scroll-wheel up/down
   *
   * Precesse input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
   *
   * \param yoffset y cordinate of mouse offset on scroll
   */
  void ProcessMouseScroll(GLfloat yoffset);
  
private:
  
  /*!
   * \brief update camera parameters
   *
   * Calculates the front vector from the Camera's (updated) Eular Angles
   *
   */
  void updateCameraVectors();
  
};
