#pragma once
#ifndef __HitBoxes__
#define __HitBoxes__

#include "ShapeObj.h"
#include "MatrixStack.h"
#include "Texture.h"

struct Face {
   int boxIndices[4];
   glm::vec3 normal;
   glm::vec3 transformedNormal;
   bool filled = false;
   glm::vec3 center;
   glm::vec3 transformedCenter;
};

typedef struct
{
   // Will Contain 4 pts;
   std::vector<glm::vec3> points;
   std::vector<glm::vec3> transformedPoints;
   // Contains the indices of the points in shapes[0].mesh.positions. Strides of 12 will get you 3 points making up a triangle
   //std::vector<unsigned int> point_indices;
   // Set of 4 vertices that refer to the points vector
   std::vector<Face> faces;
   int boneBoundTo;
   std::string boneName;
   int front = -1;
   int back = -1;
   int left = -1;
   int right = -1;
   int top = -1;
   int bottom = -1;
   
}HitBox;

class GameObject;
class HitBoxes : public ShapeObj
{
public:
   HitBoxes();
   virtual ~HitBoxes();

   
   bool load(const std::string &meshName);
   void bindToObject(GameObject *toBind);
   std::vector<HitBox *> getHitBoxes();
   bool checkVectorCollision(glm::vec3 origin, glm::vec3 ray);
   bool checkVectorAgainstBox(glm::vec3 origin, glm::vec3 ray, HitBox *box);
   void RefreshHitBox();
   void drawSimple(MatrixStack MV, Program *prog) const;
   void draw(MatrixStack MV, Program *prog) const;
   bool empty();
  
private:
   void generateShapes();
   std::vector<HitBox *> initialBoxes;
   std::vector<HitBox > boxes;
   GameObject *boundTo;
};
#endif