#pragma once
#ifndef __SHADOW_MAP__
#define __SHADOW_MAP__


#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#ifdef _WIN32

#include <Windows.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Program.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GameObject.h"
#endif

class ShadowMap
{
public:
   ShadowMap();
   virtual ~ShadowMap();
   void setup();
   void bind();
   void bindShadowMap(Program *prog);
   void ShadowMap::render(glm::mat4 in, GameObject *shape, MatrixStack *MV);
   void unbind();
   glm::mat4 getDepthBiasMVP(glm::mat4 m);
   glm::mat4 depthMVP;
   glm::mat4 depthBiasMVP;
   glm::mat4 depthModelMatrix;
   glm::mat4 depthViewMatrix;
   glm::mat4 depthProjectionMatrix;

   GLuint shadowMapSize = 1024;
   GLuint depthTexture = 0;
   GLuint shadowFramebuffer = 0;
   Program shadowShader;
private:
};

#endif