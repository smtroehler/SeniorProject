#pragma once
#ifndef __PlayerController__
#define __PlayerController__
#include "ShapeObj.h"
#include "glm/glm.hpp"
#include "GameObject.h"
class PlayerController 
{
   public:
      PlayerController();
      virtual ~PlayerController();

      void processKeyboardControl(bool *keyboard, float dt);
      void processMouseControl(int x, int y);

      glm::vec2 getViewRotation();
      glm::vec3 getViewPosition();
      glm::vec3 getPosition();
      glm::vec3 velocity;

      virtual OBoundingBox getOBB();
      bool floorColliding;
      bool specModeActive = true;

      bool posXColliding = false;
      bool negXColliding = false;
      bool posYColliding = false;
      bool negYColliding = false;
      bool posZColliding = false;
      bool negZColliding = false;
      bool willCollideY = false;
      void update(float dt);

      bool groundColliding = false;

      float jumpTimer = 0;
      float maxJumpTime = .5;
      float airTimer = 0;
      float fallDist;
      int playAnimation = 0;
      bool showPlayer = true;
      GameObject *playerModel = NULL;
      glm::vec3 worldPos;
      glm::vec2 rotation;
   protected:
      glm::vec3 cameraOffset;
      glm::vec2 cameraRotation;

     


      glm::vec2 mousePrev;
      float rfactor;
      
      
};

#endif