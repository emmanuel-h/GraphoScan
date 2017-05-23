#include "Camera.hpp"

glm::mat4 Camera::GetViewMatrix(){
  return myLookAt();
}

glm::mat4 Camera::GetProjectionMatrix(float height, float width) {
  return glm::perspective(glm::radians(Zoom), height / width, 0.1f, 100.0f);
}

glm::mat4 Camera::myLookAt() {
  glm::mat4 view;
  glm::mat4 rotation, translation;
  
  glm::vec3 front = mFront;
  glm::vec3 worldUp = mWorldUp;
  
  glm::vec3 r, u, d;						//x,y,z
  d = glm::normalize(-front);					//zaxis
  r = glm::normalize(glm::cross(glm::normalize(worldUp),d));	//xaxis = y X z
  u = glm::normalize(glm::cross(d, r));			        //yaxis = z X x;
  
  rotation[0][0] = r.x;
  rotation[1][0] = r.y;
  rotation[2][0] = r.z;
  
  rotation[0][1] = u.x;
  rotation[1][1] = u.y;
  rotation[2][1] = u.z;
  
  rotation[0][2] = d.x;
  rotation[1][2] = d.y;
  rotation[2][2] = d.z;
  
  translation[3][0] = -mPosition.x;
  translation[3][1] = -mPosition.y;
  translation[3][2] = -mPosition.z;
  
  view = rotation*translation;
  return view;
}

void Camera::ProcessKeyboard(Camera_Mouvement direction, GLfloat deltaTime) {
  GLfloat velocity = MovementSpeed*deltaTime;
  if (direction == FORWARD)
    mPosition += mFront*velocity;
  if (direction == BACKWARD)
    mPosition -= mFront*velocity;
  if (direction == RIGHT)
    mPosition += glm::normalize(glm::cross(mFront, mUp))*velocity;
  if(direction == LEFT)
    mPosition -= glm::normalize(glm::cross(mFront, mUp))*velocity;
  updateCameraVectors();
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;
  mYaw += xoffset;
  mPitch += yoffset;
  
  //Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch)
    {
      if (mPitch > 89.0f)
	mPitch = 89.0f;
      if (mPitch < -89.0f)
	mPitch = -89.0f;
    }
  updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset) {
  if(Zoom <= 500.0f && Zoom >= -500.0f)
    Zoom -= yoffset;
  if (Zoom > 500.0f)
    Zoom = 500.0f;
  if (Zoom < -500.0f)
    Zoom = -500.0f;
}

void Camera::updateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(mPitch))*cos(glm::radians(mYaw));
  front.y = sin(glm::radians(mPitch));
  front.z = cos(glm::radians(mPitch))*sin(glm::radians(mYaw));
  //when use my own lookat()
  mFront = glm::normalize(front);
  mRight = glm::normalize(glm::cross(mWorldUp, mFront));
  mUp = glm::normalize(glm::cross(mFront, mRight));
}

void Camera::setDefaultPosition(){
  mPosition=glm::vec3(0.0f, 0.0f, 3.0f);
  mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  mYaw = YAW;
  mPitch = PITCH;
  updateCameraVectors();
}
