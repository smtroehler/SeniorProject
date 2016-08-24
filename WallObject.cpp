#include "WallObject.h"
#include "MatrixStack.h"

WallObject::WallObject() : GameObject()
{
   type = "WallObject";
}

WallObject::WallObject(WallObject *toCopy) : GameObject(toCopy)
{
   type = "WallObject";
   for (int i = 0; i < 6; i++)
   {
      faces[i] = new GameObject(toCopy->faces[i]);
      textureIndexes[i] = toCopy->textureIndexes[i];
   }
   curSelectedFace = toCopy->curSelectedFace;
}

WallObject::~WallObject()
{

}

bool WallObject::load()
{
   for (int i = 0; i < 6; i++)
   {
      faces[i] = new GameObject;
   }
  

   faces[0].load("Models/cube_front.smd");
   faces[1].load("Models/cube_left.smd");
   faces[2].load("Models/cube_right.smd");
   faces[3].load("Models/cube_back.smd");
   faces[4].load("Models/cube_top.smd");
   faces[5].load("Models/cube_bot.smd");

   for (int i = 0; i < 6; i++)
   {
      faces[i].animated = false;
   }


   return true;
}

std::vector<tinyobj::shape_t *> WallObject::getShapes()
{
   std::vector<tinyobj::shape_t *> out;

   return out;
};

void WallObject::init()
{
   for (int i = 0; i < 6; i++)
   {
      faces[i].init();
   }
   generateBBox();
}

void WallObject::draw(MatrixStack MV, Program *prog)
{
   MV.pushMatrix();
   MV.multMatrix(getModelMatrix());

   

   for (int i = 0; i < 6; i++)
   {

      faces[i].draw(MV, prog);
   }
   MV.popMatrix();
}

GameObject* WallObject::getFace(int toGet)
{
   return &faces[toGet];
}

void WallObject::setHighlight(bool in)
{
   highlightObject = in;
   if (curSelectedFace != -1)
      faces[curSelectedFace].setHighlight(highlightObject);
   for (int i = 0; i < 6; i++)
   {
      
      if (highlightObject == false)
      {
         faces[i].setHighlight(false);
      }
      else if (curSelectedFace != -1 && i != curSelectedFace)
      {
         faces[i].setHighlight(false);
      }
      else if (i != curSelectedFace)
      {
         faces[i].setHighlight(true);
      }
   }
}

void WallObject::setDiffuseTexture(Texture *in)
{
   if (curSelectedFace == -1)
   {   
      for (int i = 0; i < 6; i++)
      {
         faces[i].setDiffuseTexture(in);
      }
   }
   else
   {
      faces[curSelectedFace].setDiffuseTexture(in);
   }
}

void WallObject::setTextureIndexes(int index)
{
   for (int i = 0; i < 6; i++)
   {
      textureIndexes[i] = index;
   }
}


void WallObject::animate(int frame, int animation)
{
   return;
}

void WallObject::setTextureScale(glm::vec2 in)
{
   if (curSelectedFace == -1)
   {
      for (int i = 0; i < 6; i++)
      {
         faces[i].setTextureScale(in);
      }
   }
   else
   {
      faces[curSelectedFace].setTextureScale(in);
   }
}

void WallObject::setScaleNormalizedUV(glm::vec2 in)
{
   if (curSelectedFace == -1)
   {
      glm::vec2 toScale;
      toScale.x = (1.0f * scale_o.x) / in.x;
      toScale.y = (1.0f * scale_o.z) / in.y;
      faces[0].setTextureScale(toScale);
      faces[3].setTextureScale(toScale);

      toScale.x = (1.0f * scale_o.z) / in.x;
      toScale.y = (1.0f * scale_o.y) / in.y;
      faces[1].setTextureScale(toScale);
      faces[2].setTextureScale(toScale);

      toScale.x = (1.0f * scale_o.x) / in.x;
      toScale.y = (1.0f * scale_o.y) / in.y;
      faces[4].setTextureScale(toScale);
      faces[5].setTextureScale(toScale);
   }
   if (curSelectedFace == 0 || curSelectedFace == 3)
   {
      glm::vec2 toScale;
      // front and back faces
      toScale.x = (1.0f * scale_o.x) * in.x;
      toScale.y = (1.0f * scale_o.z) * in.y;
      faces[0].setTextureScale(toScale);
      faces[3].setTextureScale(toScale);
   }
   else if (curSelectedFace == 1 || curSelectedFace == 2)
   {
      glm::vec2 toScale;
      // left and right faces
      toScale.x = (1.0f * scale_o.z) * in.x;
      toScale.y = (1.0f * scale_o.y) * in.y;
      faces[1].setTextureScale(toScale);
      faces[2].setTextureScale(toScale);
   }
   else if (curSelectedFace == 4 || curSelectedFace == 5)
   {
      glm::vec2 toScale;
      // top and bot faces
      toScale.x = (1.0f * scale_o.x) * in.x;
      toScale.y = (1.0f * scale_o.y) * in.y;
      faces[4].setTextureScale(toScale);
      faces[5].setTextureScale(toScale);
   }
}

OBoundingBox WallObject::getOBB()
{
   OBoundingBox out;
   MatrixStack MV;

   glm::vec3 Ax = glm::vec3(1.0f, 0.0f, 0.0f);
   glm::vec3 Ay = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 Az = glm::vec3(0.0f, 0.0f, 1.0f);

   MV.pushMatrix();
   MV.rotate(rotation_o.z, glm::vec3(0.0f, 0.0, 1.0));
   MV.rotate(rotation_o.y, glm::vec3(0.0f, 1.0, 0.0));
   MV.rotate(rotation_o.x, glm::vec3(1.0f, 0.0, 0.0));
   Ax = glm::vec3(MV.topMatrix() * glm::vec4(Ax, 0.0f));
   MV.popMatrix();

   MV.pushMatrix();
   MV.rotate(rotation_o.z, glm::vec3(0.0f, 0.0, 1.0));
   MV.rotate(rotation_o.y, glm::vec3(0.0f, 1.0, 0.0));
   MV.rotate(rotation_o.x, glm::vec3(1.0f, 0.0, 0.0));
   Ay = glm::vec3(MV.topMatrix() * glm::vec4(Ay, 0.0f));
   MV.popMatrix();

   MV.pushMatrix();
   MV.rotate(rotation_o.z, glm::vec3(0.0f, 0.0, 1.0));
   MV.rotate(rotation_o.y, glm::vec3(0.0f, 1.0, 0.0));
   MV.rotate(rotation_o.x, glm::vec3(1.0f, 0.0, 0.0));
   Az = glm::vec3(MV.topMatrix() * glm::vec4(Az, 0.0f));
   MV.popMatrix();

   out.center = this->position_o;
   out.unit_vec_X = Ax;
   out.unit_vec_Y = Ay;
   out.unit_vec_Z = Az;
   
   out.half_width = scale_o.x;
   out.half_height = scale_o.y;
   out.half_depth = scale_o.z;
   return out;
}

//http://www.jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf
/*
L = Ax
CASE 2:
L = Ay
CASE 3 :
L = Az
CASE 4 :
L = Bx
CASE 5 :
L = By
CASE 6 :
L = Bz
CASE 7 :
L = Ax x Bx
CASE 8 :
L = Ax x By
CASE 9 :
L = Ax x Bz
CASE 10 :
L = Ay x Bx
CASE 11 :
L = Ay x By
CASE 12 :
L = Ay x Bz
CASE 13 :
L = Az x Bx
CASE 14 :
L = Az x By
CASE 15 :
L = Az x B*/
/*
Gen Inequality:
T is Pb - Pa
T dot L > |Wa * Ax dot L| + |Ha * Ay dot L| + |Da * Az dot L| + |Wb * Bx dot L| + |Hb * By dot L| + |Db * Bx dot L|

*/
float getExpression(OBoundingBox *thisBox, OBoundingBox *otherBox, glm::vec3 L)
{
   float a = fabs(thisBox->half_width * glm::dot(thisBox->unit_vec_X, L));
   float b = fabs(thisBox->half_height * glm::dot(thisBox->unit_vec_Y, L));
   float c = fabs(thisBox->half_depth * glm::dot(thisBox->unit_vec_Z, L));
   float d = fabs(otherBox->half_width * glm::dot(otherBox->unit_vec_X, L));
   float e = fabs(otherBox->half_height * glm::dot(otherBox->unit_vec_Y, L));
   float f = fabs(otherBox->half_depth * glm::dot(otherBox->unit_vec_Z, L));
   return a + b + c + d + e + f;
}

float getSignedExpression(OBoundingBox *thisBox, OBoundingBox *otherBox, glm::vec3 L)
{
   float a = (thisBox->half_width * glm::dot(thisBox->unit_vec_X, L));
   float b = (thisBox->half_height * glm::dot(thisBox->unit_vec_Y, L));
   float c = (thisBox->half_depth * glm::dot(thisBox->unit_vec_Z, L));
   float d = (otherBox->half_width * glm::dot(otherBox->unit_vec_X, L));
   float e = (otherBox->half_height * glm::dot(otherBox->unit_vec_Y, L));
   float f = (otherBox->half_depth * glm::dot(otherBox->unit_vec_Z, L));
   return a + b + c + d + e + f;
}

bool isOutside(float maxDist, float dist)
{
   return (fabs(maxDist) > dist);
}
float ratio;
bool isInsideInNextFrame(float maxDist, float speed, float dist, float dt, int thing)
{
      // we will test if 
      if (maxDist >= 0) {
         if (speed < 0) {
            float distFromFace = fabs(fabs(maxDist) - dist);
            float timeToFace = distFromFace / fabs(speed);

            if (timeToFace > dt)
               return false;
         }
         else
            return false;
      }
      else if (maxDist < 0) {
         if (speed > 0) {
            float distFromFace = fabs(fabs(maxDist) - dist);
            float timeToFace = distFromFace / fabs(speed);
            if (timeToFace > dt)
               return false;
         }
         else
            return false;
      }
      return true;
}

// http://www.jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf
glm::vec3 WallObject::checkPlayerCollision(PlayerController *other, float dt)
{
   OBoundingBox thisBox = this->getOBB();
   OBoundingBox playerBox = other->getOBB();
   glm::vec3 vel = other->velocity;
   float dist;
   float maxDist;
   glm::vec3 L;
   glm::vec3 T = (playerBox.center) - thisBox.center;
   bool willX, willY, willZ;
   willX = false;
   willY = false;
   willZ = false;

   float ratioY = 1;


   std::vector<float> distances;
   float speed;
   // Case 1
   L = thisBox.unit_vec_X;
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
    //     std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }
      
   distances.push_back(maxDist - dist);

   // Case 2 
   L = thisBox.unit_vec_Y;
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
      //   std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 3 
   L = thisBox.unit_vec_Z;
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
       //  std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 4 
   L = playerBox.unit_vec_X;
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
    //     std::cout << "will X hit in next frame" << std::endl;
         willX = true;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);
   float playerXDist = fabs(maxDist - dist);

   // Case 5 
   L = playerBox.unit_vec_Y;
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 1) == true)
      {
       
         float distFromFace = fabs(fabs(maxDist) - dist);
         float timeToFace = distFromFace / fabs(speed);
         ratioY = timeToFace / dt;
         willY = true;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }
 //  std::cout << "dist " << fabs(fabs(maxDist) - dist) << std::endl;
   distances.push_back(maxDist - dist);
   float playerYDist = fabs(fabs(maxDist) - dist);
   

   // Case 6
   L = playerBox.unit_vec_Z;
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
      //   std::cout << "will Z hit in next frame" << std::endl;
         willZ = true;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);
   float playerZDist = fabs(maxDist - dist);

   // Case 7
   L = glm::cross(thisBox.unit_vec_X, playerBox.unit_vec_X);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 8
   L = glm::cross(thisBox.unit_vec_X, playerBox.unit_vec_Y);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {

      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 9
   L = glm::cross(thisBox.unit_vec_X, playerBox.unit_vec_Z);
   glm::vec3 temp = vel;
   temp.x = 0;
   temp.z = 0;
   speed = glm::dot(L, temp);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
         float distFromFace = fabs(fabs(maxDist) - dist);
         float timeToFace = distFromFace / fabs(speed);
         float rat = timeToFace / dt;
         if (ratioY > rat)
            ratioY = rat;
         willY = true;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(fabs(maxDist) - fabs( dist));
   float xEdgeDist = fabs(fabs(maxDist) - dist);
   // Case 10
   L = glm::cross(thisBox.unit_vec_Y, playerBox.unit_vec_X);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 11
   L = glm::cross(thisBox.unit_vec_Y, playerBox.unit_vec_Y);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
     //    std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 12
   L = glm::cross(thisBox.unit_vec_Y, playerBox.unit_vec_Z);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
    //     std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 13
   L = glm::cross(thisBox.unit_vec_Z, playerBox.unit_vec_X);
   temp = vel;
   temp.x = 0;
   temp.z = 0;
   speed = glm::dot(L, temp);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   bool willzEdge = false;
   float ratioZEdge = 0;
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
         willzEdge = true;

         float distFromFace = fabs(fabs(maxDist) - dist);
         float timeToFace = distFromFace / fabs(speed);
         ratioZEdge = timeToFace / dt;
         if (ratioY > ratioZEdge)
            ratioY = ratioZEdge;
         willY = true;
         
    //     std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   float zEdgeDist = fabs(fabs(maxDist) - dist);

   distances.push_back(maxDist - dist);

   // Case 14
   L = glm::cross(thisBox.unit_vec_Z, playerBox.unit_vec_Y);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
     //    std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);

   // Case 15
   L = glm::cross(thisBox.unit_vec_Z, playerBox.unit_vec_Z);
   speed = glm::dot(L, vel);
   dist = getExpression(&thisBox, &playerBox, L);
   maxDist = (glm::dot(T, L));
   // is outside of box
   if (isOutside(maxDist, dist) == true)
   {
      if (isInsideInNextFrame(maxDist, speed, dist, dt, 0) == true)
      {
      //   std::cout << "will hit in next frame" << std::endl;
      }
      else
      {
         return glm::vec3(0.0, 0.0, 0.0);
      }
   }

   distances.push_back(maxDist - dist);


   if (playerXDist < .05 || willX)
   {
      if (T.x < 0)
      {
         other->negXColliding = true;
      }
      else
         other->posXColliding = true;
   }
   
   if (willY)
   {
      other->groundColliding = true;
      other->velocity.y = other->velocity.y * ratioY;
   }
   if (playerYDist < .03)// || otherDist < .05 )
   {
      other->groundColliding = true;
      if (T.y > 0)
      {
         if(other->velocity.y < 0)
            other->velocity.y = 0;
         other->negYColliding = true;
      }
      else
         other->posYColliding = true;
   }
   else 
   {
      
      if ((zEdgeDist < .02 || xEdgeDist < .02))
      {
         other->groundColliding = true;
         MatrixStack m;
         m.rotate(rotation_o.z, glm::vec3(0.0f, 0.0, 1.0));
         //   m.rotate(rotation_o.y, glm::vec3(0.0f, 1.0, 0.0));
         m.rotate(rotation_o.x, glm::vec3(1.0f, 0.0, 0.0));
         glm::vec3 temp = other->velocity;
         temp.y = 0;

         temp = glm::vec3(m.topMatrix() * glm::vec4(temp, 0));

         if (temp.y > 0 && other->negYColliding == true)
            other->velocity = temp;
         else if(temp.y < 0 && other->negYColliding == true)
         {
            other->velocity.y = 0;
         }
         else
         {
            other->velocity = temp;
         }
      }

   }

   if ((playerZDist < .05) || willZ)
   {
      if (T.z < 0)
         other->negZColliding = true;
      else
         other->posZColliding = true;
   }

   

  

  //std::cout << "start " << std::endl;
   for (int i = 0; i < distances.size(); i++)
   {

  //    std::cout << i + 1 << ": " << distances.at(i) << std::endl;
   }

   return glm::vec3(1, 1, 1);


   
}


void WallObject::generateBBox()
{
   BoundingBox out;
   out.min.x = -1;
   out.max.x = 1;
   out.min.y = -1;
   out.max.y = 1;
   out.min.z = -1;
   out.max.z = 1;
   out.max_radius = 2;
   out.position = position_o;
   out.velocity = velocity_o;
   out.scale = scale_o;
   boundingBox = out;
}