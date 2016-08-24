#include "PlayerController.h"
#include <iostream>
#include <algorithm> 
#define GLM_FORCE_RADIANS
PlayerController::PlayerController() :
   rfactor(0.01f),
   rotation(0.0f, 0.0f),
   mousePrev(0.0f, 0.0f),
   worldPos(0.0f, 0.0f, 0.0f),
   cameraOffset(0.0f, 1.8f, 0.0f)
{

}


PlayerController::~PlayerController()
{

}
bool jump = false;
void PlayerController::processKeyboardControl(bool *keyboard, float dt)
{
   // this is where it calculates the movement in that direction
   // calculates movement in the left and right  of camera
   glm::vec3 side = glm::vec3(sin(cameraRotation.x - 3.14f / 2.0f), 0, cos(cameraRotation.x - 3.14f / 2.0f));
   // calculates movement in the foward and backward of camera
   glm::vec3 dir = glm::vec3(cos(cameraRotation.y) * sin(cameraRotation.x), 0,
      cos(cameraRotation.y) * cos(cameraRotation.x));
   
   //Normalize the vectors so you can get a constant velocity
   side = glm::normalize(side);
   dir = glm::normalize(dir);

   float g = 9.8;
   glm::vec3 dv(0.0f, 0.0f, 0.0f);
   if (specModeActive)
      velocity.y = 0;
   bool forward = false;
   bool backwards = false;
   rotation = glm::vec2(0.0f, 0.0f);

   if (keyboard['w']) {
      forward = true;
      dv -= dir * velocity.z ;
      if (keyboard[' '] && specModeActive) {
         if (cameraRotation.y > 0)
            velocity.y += 4 ;
         else
            velocity.y -= 4;
      }
      rotation.y = 0;
   }

   if (keyboard['s']) {
      dv += dir * velocity.z;

      if (keyboard[' '] && specModeActive) {
         if (cameraRotation.y < 0)
            velocity.y += 4;
         else
            velocity.y -= 4;
      }
      backwards = true;
      rotation.y = 3.141592;
   }

   if (keyboard['a']) {
      dv += side * velocity.x ;
      // do this to rotate the player model 
      // in the direction its running
      if (forward == true)
      {
         rotation.y += 3.141592 / 4;
      }
      else if (backwards == true)
      {
         rotation.y += -3.141592 / 4;
      }
      else
      {
         rotation.y += 3.141592 / 2;
      }
      

   }
   if (keyboard['d']) {
      dv -= side * velocity.x ;
      // do this to rotate the player model 
      // in the direction its running
      if (forward == true)
      {
         rotation.y += -3.141592 / 4;
      }
      else if (backwards == true)
      {
         rotation.y += 3.141592 / 4;
      }
      else
      {
         rotation.y += -3.141592 / 2;
      }
      
   }
   
   
   float gravModifier = 1.5;

   if (!specModeActive)
   {
      velocity.y = velocity.y - 9.8f * dt * gravModifier;
   }

   dv.y = velocity.y;
   jump = keyboard[' '];
   velocity = dv;
   
}

void PlayerController::processMouseControl(int x, int y)
{
   glm::vec2 mouseCurr(x, y);
   glm::vec2 dv = mouseCurr - mousePrev;
   dv = -dv;
   if (!((cameraRotation.y < 1.4 && dv.y > 0) || (cameraRotation.y > -1.5 && dv.y < 0)))
      dv.y = 0;

   cameraRotation += rfactor * dv;

   


   mousePrev = mouseCurr;
}

glm::vec2 PlayerController::getViewRotation() 
{
   return cameraRotation;
}

glm::vec3 PlayerController::getViewPosition()
{
   return cameraOffset + worldPos;
}

glm::vec3 PlayerController::getPosition()
{
   return worldPos;
}

OBoundingBox PlayerController::getOBB() 
{

   OBoundingBox out;
   MatrixStack MV;
   glm::vec3 Ax = glm::vec3(1.0f, 0.0f, 0.0f);
   glm::vec3 Ay = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 Az = glm::vec3(0.0f, 0.0f, 1.0f);

   out.center = this->worldPos + (cameraOffset / 2.0f);
   out.unit_vec_X = Ax;
   out.unit_vec_Y = Ay;
   out.unit_vec_Z = Az;

   out.half_width = .4;
   out.half_height = cameraOffset.y / 2.0f;
   out.half_depth = .4;
   return out;
}

void PlayerController::update(float dt)
{
   float maxTimeExtension = .5;
   if (groundColliding == false)
   {
      jumpTimer = 0;
      airTimer += dt;
   }
   else
   {
      jumpTimer += dt;
      airTimer =  0;
   }

 //  std::cout << jumpTimer << std::endl;
   float tempFallDist = fallDist;
   if (!specModeActive && negYColliding == false && willCollideY == false)
   {
      
   }
   else if ((!specModeActive && groundColliding == true) )
   {
      if (jump)
      {
         if (jumpTimer < maxJumpTime + maxTimeExtension && jumpTimer != 0 && fallDist > 2)
         {

            velocity.y = 6 + (fallDist / 1.5) * (std::min(maxJumpTime, jumpTimer) / maxJumpTime);
            fallDist = 0;
            
         }
         else {
            velocity.y = 6;

            fallDist = 0;
         }
      }
      else if(negYColliding == true && velocity.y < 0)
      {
     //    velocity.y = 0;
      }

   }

   if (!specModeActive && posYColliding == true && velocity.y > 0)
   {
      velocity.y = 0;
      jumpTimer = maxJumpTime + 1;
   }

   if (!specModeActive && posXColliding == true && velocity.x < 0)
   {
      velocity.x = 0;
      jumpTimer = maxJumpTime + 1;
   }

   if (!specModeActive && negXColliding == true && velocity.x > 0)
   {
      velocity.x = 0;
      jumpTimer = maxJumpTime + 1;
   }

   if (!specModeActive && posZColliding == true && velocity.z < -.00001)
   {
      velocity.z = 0;
      jumpTimer = maxJumpTime + 1;
   }

   if (!specModeActive && negZColliding == true && velocity.z > .00001)
   {
      velocity.z = 0;
      jumpTimer = maxJumpTime + 1;
   }

   if (groundColliding == true)
   {
      if (fabs(velocity.x) > 0.01 || fabs(velocity.z) > 0.01)
      {
         playAnimation = 1;
      }
      else if (jumpTimer <= maxJumpTime + maxTimeExtension && tempFallDist > 2)
      {
         playAnimation = 4;
      }
      else
      {
         playAnimation = 2;
      }
   }
   else
   {
      playAnimation = 3;
   }
   if (velocity.y < 0 && groundColliding == false)
   {
      fallDist += -velocity.y * dt;
   }

   posXColliding = false;
   negXColliding = false;
   posYColliding = false;
   negYColliding = false;
   posZColliding = false;
   negZColliding = false;
   groundColliding = false;
   
   //std::cout << velocity.x << " " << velocity.y << " " << velocity.z << std::endl;
   worldPos += velocity * dt;
   velocity = glm::vec3(5.0f, velocity.y, 5.0f);

   if (showPlayer && playerModel != NULL)
   {
      playerModel->setPosition(worldPos);
   }

   
}

