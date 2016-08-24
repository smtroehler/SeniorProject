#include "ShadowMap.h"
#include "GLSL.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "MatrixStack.h"
ShadowMap::ShadowMap()
{

}

ShadowMap::~ShadowMap()
{

}
extern int width;
extern int height;
//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
// read data from buffer 
// examine the fram buffer in the  
void ShadowMap::setup()
{
   assert(glGetError() == GL_NO_ERROR);
   shadowShader.setShaderNames("Shaders/shadow_vert.glsl", "Shaders/shadow_frag.glsl");
   shadowShader.init();
   shadowShader.addUniform("depthMVP");
   shadowShader.addAttribute("vertPos");

   assert(glGetError() == GL_NO_ERROR);
   shadowShader.bind();
   glm::vec3 lightInvDir = glm::vec3(10, 2, 20);



   // Compute the MVP matrix from the light's point of view
   depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
   depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  // depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

   

  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
   shadowFramebuffer = 0;
   glGenFramebuffers(1, &shadowFramebuffer);
   glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);

   // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
   glGenTextures(1, &depthTexture);
   glBindTexture(GL_TEXTURE_2D, depthTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

   // No color output in the bound framebuffer, only depth.
   glDrawBuffer(GL_NONE);

   
   assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
 //  glBindFramebuffer(GL_FRAMEBUFFER, 0);

   assert(glGetError() == GL_NO_ERROR);

   shadowShader.unbind();
}

void ShadowMap::bind()
{
   
   glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
   shadowShader.bind();
}



void ShadowMap::render(glm::mat4 in, GameObject *shape, MatrixStack *MV)
{

   depthModelMatrix = in;

  
   assert(glGetError() == GL_NO_ERROR);
   depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

   glUniformMatrix4fv(shadowShader.getUniform("depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));

   assert(glGetError() == GL_NO_ERROR);
}

void ShadowMap::unbind()
{
   shadowShader.unbind();
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ShadowMap::bindShadowMap(Program *prog)
{
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, depthTexture);
   if (prog->getUniform("shadowMap") > 0)
      glUniform1i(prog->getUniform("shadowMap"), 1);
   else
      std::cout << "warning trying to bind shadow map to shader with no shadowMap variable\n";
}

glm::mat4 ShadowMap::getDepthBiasMVP(glm::mat4 m)
{
   depthModelMatrix = m;
   depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
   glm::mat4 biasMatrix(
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0
      );
   depthBiasMVP = biasMatrix*depthMVP;
   return glm::mat4(depthMVP);
}
