#include "Camera.h"
#include "MatrixStack.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/transform.hpp"

Camera::Camera() :
	aspect(1.0f),
	fovy(45.0f/180.0f*3.141592),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.0, 0.0),
	translations(0.0f, 0.0f, -5.0f),
	rfactor(1),
	tfactor(0.05f),
	sfactor(0.005f),
	cameraState(0),
	followPos(0.0f, 0.0f, 0.0f)
{
}

Camera::~Camera()
{
	
}

void Camera::mouseClicked(int x, int y, bool shift, bool ctrl, bool alt)
{
	mousePrev = glm::vec2( x, y);
	if(shift) {
		mouseState = Camera::TRANSLATE;
	} else if(ctrl) {
		mouseState = Camera::SCALE;
	} else {
		mouseState = Camera::ROTATE;
	}
}

void Camera::EnableCenteredCamera(glm::vec3 loc)
{
	cameraState = Camera::CENTERED;
	translations = glm::vec3(loc);
   rotations = glm::vec2(0.0f, 0.0f);
}

void Camera::EnableFixedCamera(glm::vec3 loc, glm::vec2 rot)
{
	cameraState = Camera::FIXED;
	translations = glm::vec3(loc);
   rotations = glm::vec2(rot);
}

void Camera::mouseMoved(int x, int y)
{
	glm::vec2 mouseCurr(x, y);
	glm::vec2 dv = mouseCurr - mousePrev;
	
	switch (cameraState) {
		case Camera::CENTERED:
			switch (mouseState) {
				case Camera::ROTATE:
					rotations += rfactor * dv;
					break;
				case Camera::SCALE:
					translations.z *= (1.0f - sfactor * dv.y);
					break;
			}
			break;
	}

	
	mousePrev = mouseCurr;
}

void Camera::updateFixedCamera(glm::vec3 loc, glm::vec2 rot) {
   if (cameraState == Camera::FIXED) {
      rotations = glm::vec2(rot);
      translations = glm::vec3(loc);
   }
}

void Camera::applyProjectionMatrix(MatrixStack *P) const
{
	P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(MatrixStack *MV) const
{
   if (cameraState == Camera::FIXED) {
      glm::mat4 V;
      V = glm::translate(V, translations);
      V = glm::rotate(V, rotations.x, glm::vec3(0.0f, 1.0f, 0.0f));
      
      V = glm::rotate(V, rotations.y, glm::vec3(1.0f, 0.0f, 0.0f));
      MV->translate(glm::vec3(0.0f, 0.0f, zdist));
      MV->multMatrix(glm::inverse(V));
   }
   else if (cameraState == Camera::CENTERED) {
      MV->translate(translations);
      MV->rotate(rotations.y, glm::vec3(1.0f, 0.0f, 0.0f));
     
      MV->rotate(rotations.x, glm::vec3(0.0f, 1.0f, 0.0f));
   }
}

glm::vec2 Camera::getRot() 
{
   return rotations;
}
