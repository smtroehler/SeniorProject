#pragma once
#ifndef _HUDOBJ_H_
#define _HUDOBJ_H_

#include "tiny_obj_loader.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Texture.h"
#include <string>
#include "Camera.h"
#include "glm/glm.hpp"
class HudObject
{
   public:

   HudObject();
   void setTextureName(const std::string &filename) {texture.setFilename(filename);}
   void setBlendName(const std::string &filename) {blendMap.setFilename(filename);}

   void rescale();
   void mirror();
   void loadFile(const std::string &meshName);
   void setWH(float w, float h) {size_w = w; size_h = h;}
   void bindProg(Program *p) {prog = p;}
   void init();

   void draw(MatrixStack *P, MatrixStack *MV);


   std::vector<tinyobj::shape_t> shapes;
   Texture texture;
   Texture blendMap;
   Camera *cameraptr;
   Program *prog;

   unsigned posBufID;
   unsigned norBufID;
   unsigned texBufID;
   unsigned indBufID;

   float size_w;
   float size_h;

   glm::vec3 pos;
   glm::vec3  scale;
   glm::vec3  rot;
};
#endif