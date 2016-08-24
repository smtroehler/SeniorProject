#pragma once
#ifndef __WallObject__
#define __WallObject__

#include "GameObject.h"
#include <iostream>
#include "PlayerController.h"

class WallObject : public GameObject
{
public:
   WallObject();
   WallObject(WallObject *toCopy);
   virtual ~WallObject();

   void init();
   void draw(MatrixStack MV, Program *prog);
   bool load();
   virtual void animate(int frame, int animation);
   virtual void setDiffuseTexture(Texture *in);

   virtual OBoundingBox getOBB();

  // virtual void setDiffuseTextureOfFace(Texture *in, int face);
   virtual std::vector<tinyobj::shape_t *> getShapes();
   int curSelectedFace = -1;
   int textureIndexes[6];
   GameObject faces[6];
   virtual void setTextureScale(glm::vec2 in);
   virtual void setScaleNormalizedUV(glm::vec2 in);
   virtual void setHighlight(bool in);
   void highlightFace();
   GameObject* getFace(int toGet);
   void setTextureIndexes(int index);
   void normalizeObject() { std::cout << "invalid operation for walls\n"; }
   virtual void generateBBox();
   glm::vec3 checkPlayerCollision(PlayerController *other, float dt);
private:
};

#endif