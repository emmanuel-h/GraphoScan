#pragma once

enum Camera_Mouvement
  {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
  };

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

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
  
  //Constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),GLfloat yaw = YAW, GLfloat pitch = PITCH) :mFront(glm::vec3(0.0f, 0.0f, -1.0f)),MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){};
  
  //Constructor with salar values
  Camera(GLfloat xpos, GLfloat ypos, GLfloat zpos, GLfloat xup, GLfloat yup, GLfloat zup, GLfloat yaw = YAW, GLfloat pitch = PITCH) :mFront(glm::vec3(0.0f, 0.0f, -1.0f)),MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){};
  
  //Returns the view matrix calculated using Eular Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix();
  //Returns the projection matrix calculated using Zoom and the Projective Matrix
  glm::mat4 GetProjectionMatrix(float height = 800, float width = 600);
  //My own lookat function
  glm::mat4 myLookAt();

  //Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
  void ProcessKeyboard(Camera_Mouvement direction, GLfloat deltaTime);

  //Processe input received from a mouse input system. Excepts the offset value in both the x and y direction.
  void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
  //Precesse input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void ProcessMouseScroll(GLfloat yoffset);
private:
  //Calculates the front vector from the Camera's (updated) Eular Angles
  void updateCameraVectors();
};
