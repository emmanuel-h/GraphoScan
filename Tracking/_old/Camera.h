#pragma once

//Pour Linux
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Inutile avec Linux
//#include <glm\glm.hpp>
//#include <glm\gtc\matrix_transform.hpp>
//#include <glm\gtc\type_ptr.hpp>

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

class Camera
{
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
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat yaw = YAW, GLfloat pitch = PITCH) :mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		mPosition = position;
		mWorldUp = up;
		mYaw = yaw;
		mPitch = pitch;
		updateCameraVectors();
	}
	//Constructor with salar values
	Camera(GLfloat xpos, GLfloat ypos, GLfloat zpos, GLfloat xup, GLfloat yup, GLfloat zup, GLfloat yaw = YAW, GLfloat pitch = PITCH) :mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		mPosition = glm::vec3(xpos, ypos, zpos);
		mWorldUp = glm::vec3(xup, yup, zup);
		mYaw = yaw;
		mPitch = pitch;
		updateCameraVectors();
	}
	//Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return myLookAt();
		//return glm::lookAt(mPosition, mPosition + mFront, mUp);
	}
	//Returns the projection matrix calculated using Zoom and the Projective Matrix
	glm::mat4 GetProjectionMatrix(float height = 800, float width = 600)
	{
		return glm::perspective(glm::radians(Zoom), height / width, 0.1f, 100.0f);
	}
	//My own lookat function
	glm::mat4 myLookAt()
	{
		glm::mat4 view;
		glm::mat4 rotation, translation;

		glm::vec3 front = mFront;
		glm::vec3 worldUp = mWorldUp;
		//glm::vec3 right = mRight;

		glm::vec3 r, u, d;											//x,y,z
		d = glm::normalize(-front);									//zaxis
		r = glm::normalize(glm::cross(glm::normalize(worldUp),d));	//xaxis = y X z
		u = glm::normalize(glm::cross(d, r));						//yaxis = z X x;
		
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

	//Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Mouvement direction, GLfloat deltaTime)
	{
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

	//Processe input received from a mouse input system. Excepts the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
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
	//Precesse input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (Zoom <= 45.0f && Zoom >= 1.0f)
			Zoom -= yoffset;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
	}
private:
	//Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(mPitch))*cos(glm::radians(mYaw));
		front.y = sin(glm::radians(mPitch));
		front.z = cos(glm::radians(mPitch))*sin(glm::radians(mYaw));
		//when use my own lookat()
		mFront = glm::normalize(front);
		mRight = glm::normalize(glm::cross(mWorldUp, mFront));
		mUp = glm::normalize(glm::cross(mFront, mRight));
		//when use LookAt()
		//mRight = glm::normalize(glm::cross(mFront,mWorldUp ));
		//mUp = glm::normalize(glm::cross(mRight, mFront));
	}
};
