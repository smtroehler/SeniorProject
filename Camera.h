#pragma  once
#ifndef __Camera__
#define __Camera__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
class MatrixStack;

class Camera
{
public:
	
	enum {
		ROTATE = 0,
		TRANSLATE,
		SCALE
	};
	
	enum {
		FREE = 0,
      FIXED,
		CENTERED
	};

	Camera();
	virtual ~Camera();
	void setAspect(float a) { aspect = a; };
	void setRotationFactor(float f) { rfactor = f; };
	void setTranslationFactor(float f) { tfactor = f; };
	void setScaleFactor(float f) { sfactor = f; };
	void mouseClicked(int x, int y, bool shift, bool ctrl, bool alt);


	void EnableCenteredCamera(glm::vec3 loc);
	void EnableFixedCamera(glm::vec3 loc, glm::vec2 rot);
	void updateFixedCamera(glm::vec3 loc, glm::vec2 rot);

	void mouseMoved(int x, int y);
   float zdist = -2.0f;
	void applyProjectionMatrix(MatrixStack *P) const;
	void applyViewMatrix(MatrixStack *MV) const;
	glm::vec2 getRot();
   glm::vec3 translations;
   glm::vec2 rotations;
private:
	float aspect;
	float fovy;
	float znear;
	float zfar;
   
	float scale;

	glm::vec2 mousePrev;
	glm::vec3 followPos;

	int cameraState;
	int mouseState;

	float rfactor;
	float tfactor;
	float sfactor;
};

#endif
