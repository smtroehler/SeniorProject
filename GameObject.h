#pragma once
#ifndef __GameObject__
#define __GameObject__

#include "ShapeObj.h"
#include "MatrixStack.h"
#include "Texture.h"
#include "HitBoxes.h"


struct BoundingBox {
   glm::vec3 min;
   glm::vec3 max;
   float max_radius;
   glm::vec3 position;
   glm::vec3 velocity;
   glm::vec3 scale;
   glm::vec3 rotation;
} ;

struct OBoundingBox
{
   glm::vec3 center;
   glm::vec3 unit_vec_X;
   glm::vec3 unit_vec_Y;
   glm::vec3 unit_vec_Z;
   float half_height;
   float half_width;
   float half_depth;
};

class GameObject: public ShapeObj
{
   public:
   
      GameObject();
      GameObject(GameObject *toCopy);
      virtual ~GameObject();

      virtual void draw(MatrixStack MV, Program *prog);
      virtual void animate(int frame, int animation);

      void loadSmdAnim(const std::string &filename);
      void loadDiffuseTexture(const std::string &f);
      virtual void setDiffuseTexture(Texture *in);

      void setRotation(glm::vec3 rot);
      void setScale(glm::vec3 scl);
      void setPosition(glm::vec3 pos);
      void setObjectID(int newID);
      virtual void setTextureScale(glm::vec2 in);

      glm::vec3 getRotation();
      glm::vec3 getScale();
      glm::vec3 getPosition();
      glm::mat4 getModelMatrix();
      glm::vec2 getTextureScale() { return glm::vec2(textureScale); }



      void translate(glm::vec3 pos);
      void scale(glm::vec3 scl);
      void scaleTexture(glm::vec2 in);
      void rotate(glm::vec3 rot);
      void update(float dt);
      int getObjectID();
      
      virtual void setScaleNormalizedUV(glm::vec2 in);


      void init();
      void normalizeObject();
      glm::vec3 getCameraPosition();
      
      
      // Bounding Box functions
      void drawBoundingBox(MatrixStack MV, Program *prog);
      void RefreshBBox();
      BoundingBox getBBox();
      virtual void generateBBox();
      virtual glm::vec3 checkCollision(GameObject *other);

      // Hit box functions
      void loadHitBox(const std::string &meshName);
      HitBoxes *getHitBox();

      bool highlightObject = false;
      virtual void setHighlight(bool in);
   protected: 
      //Bounding BoxFunctions
      void bindBoundingBox();
      std::vector<float> generateBBoxPoints(BoundingBox *bb);
      std::vector< unsigned int> generateBBoxIndices();

      BoundingBox boundingBox; // used for holding initial bbox
      BoundingBox tempBBox; // Used for holding current transformed bbx


      // Positional and Identification data
      
      int objectID;
      Texture *diffuseTexture = NULL;
      glm::vec3 prev_position_o;
      glm::vec3 position_o;
      glm::vec3 rotation_o;
      glm::vec3 scale_o;
      glm::vec3 velocity_o;

      // used if there is a camera bound to this object
      glm::vec3 cameraOffset;
      glm::vec2 textureScale;

      // Hit box data
      HitBoxes hitBoxes;

};
#endif