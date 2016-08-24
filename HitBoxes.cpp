#define GLM_FORCE_RADIANS
#include "HitBoxes.h"
#include <iostream>
#include "MatrixStack.h"
#include "glm/gtc/type_ptr.hpp"
#include "GameObject.h"
#include "GLSL.h"

HitBoxes::HitBoxes()
{

}

HitBoxes::~HitBoxes()
{

}

bool HitBoxes::load(const std::string &meshName)
{
   ShapeObj::load(meshName);
   
   
   // if the number of indices / 3 ( which equals the number of vertices)
   // which is again / 3 (which equals the number of faces)
   // is not a multiple of 12 (the number of triangles needed for a cube/rectangle)
   // then the hitboxes are invalid
   if ((shapes[0].mesh.indices.size()) / 3 / 3 % 12 != 0 || shapes[0].mesh.indices.size() == 0) {
      std::cout << "HITBOX ERR: this is an incorrect hit box. The number of triangles is not a multiple of 12." 
                << "\nIf it is not a muiltiple of 12 that means that the shape imported are not cubes " << std::endl
                << "There are: " << shapes[0].mesh.indices.size()/3/3 << " faces" << std::endl;
      
      return false;
   }

   // this vector represents indices of vertices that create a face
   std::vector<unsigned int> culledIndicesVec = shapes[0].mesh.indices;
   // indices of verts that are currently in the box
   std::vector<int> mustBeInstanceTo;
   int i = 0;

  // std::cout << culledIndicesVec.size() << std::endl;
   while (culledIndicesVec.empty() == false)
   {
      std::vector<glm::vec3> points;
      HitBox newBox;
      // Add the 3 verts making up a tri to a new hitbox.
      points.push_back(glm::vec3(shapes[0].mesh.positions[culledIndicesVec[i]],
                                 shapes[0].mesh.positions[culledIndicesVec[i+1]],
                                 shapes[0].mesh.positions[culledIndicesVec[i+2]]));
      points.push_back(glm::vec3(shapes[0].mesh.positions[culledIndicesVec[i+3]],
                                 shapes[0].mesh.positions[culledIndicesVec[i+4]],
                                 shapes[0].mesh.positions[culledIndicesVec[i+5]]));
      points.push_back(glm::vec3(shapes[0].mesh.positions[culledIndicesVec[i+6]],
                                 shapes[0].mesh.positions[culledIndicesVec[i+7]],
                                 shapes[0].mesh.positions[culledIndicesVec[i+8]]));

      // Error check the box to make sure that the weights are all the same
      if (shapes[0].mesh.numbonesbinded[culledIndicesVec[i]/3] != 1 || 
          shapes[0].mesh.numbonesbinded[culledIndicesVec[i+3]/3] != 1 || 
          shapes[0].mesh.numbonesbinded[culledIndicesVec[i+6] / 3] != 1 )
      {
         std::cout << "Have more or less than one bone bound to a vertice in your hitBox" << std::endl;
      }
      else if (shapes[0].mesh.boneparents[culledIndicesVec[i] / 3] !=
               shapes[0].mesh.boneparents[culledIndicesVec[i+3] / 3] &&
               shapes[0].mesh.boneparents[culledIndicesVec[i+3] / 3] !=
               shapes[0].mesh.boneparents[culledIndicesVec[i+6] / 3])
      {
         std::cout << "Have different weights" << std::endl;
      }

      newBox.boneName = shapes[0].skeleton.bone_name[shapes[0].mesh.boneparents[culledIndicesVec[i] / 3]];
      newBox.boneBoundTo = shapes[0].mesh.boneparents[culledIndicesVec[i] / 3];
      newBox.points.push_back(points[0]);
      newBox.points.push_back(points[1]);
      newBox.points.push_back(points[2]);

      culledIndicesVec.erase(culledIndicesVec.begin(), culledIndicesVec.begin() + 9);

      int j = 0;
      while(j < culledIndicesVec.size())  {
        // std::cout << culledIndicesVec.size() << std::endl;
         int matches[3];
         int numMatches = 0;
         matches[0] = matches[1] = matches[2] = 0;
         glm::vec3 pt1,pt2,pt3;
         unsigned int pt1_indice, pt2_indice, pt3_indice; 


         pt1 = glm::vec3(shapes[0].mesh.positions[culledIndicesVec[j]],
                         shapes[0].mesh.positions[culledIndicesVec[j+1]],
                         shapes[0].mesh.positions[culledIndicesVec[j+2]]);
         pt2 = glm::vec3(shapes[0].mesh.positions[culledIndicesVec[j+3]],
                         shapes[0].mesh.positions[culledIndicesVec[j+4]],
                         shapes[0].mesh.positions[culledIndicesVec[j+5]]);
         pt3 = glm::vec3(shapes[0].mesh.positions[culledIndicesVec[j+6]],
                         shapes[0].mesh.positions[culledIndicesVec[j+7]],
                         shapes[0].mesh.positions[culledIndicesVec[j+8]]);

         if (shapes[0].mesh.numbonesbinded[culledIndicesVec[j]/3] != 1 ||
            shapes[0].mesh.numbonesbinded[culledIndicesVec[j+3] / 3] != 1 ||
            shapes[0].mesh.numbonesbinded[culledIndicesVec[j+6] / 3] != 1)
         {
            std::cout << "Have more or less than one bone bound to a vertice in your hitBox" << std::endl;
         }
         if (shapes[0].mesh.boneparents[culledIndicesVec[j] / 3] !=
            shapes[0].mesh.boneparents[culledIndicesVec[j + 3] / 3] &&
            shapes[0].mesh.boneparents[culledIndicesVec[j + 3] / 3] !=
            shapes[0].mesh.boneparents[culledIndicesVec[j + 6] / 3])
         {
            std::cout << "Have different weights in triangle" << std::endl;
         }
         

         // Check if any of the points on this tri are incident to any points to the current box
         for (int k = 0; k < points.size(); k++) {
            if (fabs(glm::distance(pt1, points.at(k))) == 0)
            {
               matches[0] = -1;
               numMatches++;
               pt1_indice = k;
            }
            if (fabs(glm::distance(pt2, points.at(k))) == 0 )
            {
               matches[1] = -1;
               numMatches++;
               pt2_indice = k;
            }
            if (fabs(glm::distance(pt3, points.at(k))) == 0)
            {
               matches[2] = -1;
               numMatches++;
               pt3_indice = k;
            }
            
         }
         // Check the number of matches and if there are matches then add verts that AREN't match to the box
         if (numMatches > 0)
         {
            if (shapes[0].mesh.boneparents[culledIndicesVec[j] / 3] != newBox.boneBoundTo)
            {
               std::cout << "Different weights than rest of the box" << std::endl;
            }
            if (matches[0] != -1)
            {
               points.push_back(pt1);
               newBox.points.push_back(pt1);
               pt1_indice = points.size() - 1;
            }
            if (matches[1] != -1)
            {
               points.push_back(pt2);
               newBox.points.push_back(pt2);
               pt2_indice = points.size() - 1;
            }
            if (matches[2] != -1)
            {
               points.push_back(pt3);
               newBox.points.push_back(pt3);
               pt3_indice = points.size() - 1;
            }

            glm::vec3 center;
            glm::vec3 u = pt2 - pt1;
            glm::vec3 v = pt3 - pt1;
            glm::vec3 w = pt3 - pt2;

            if (fabs(glm::length(w)) > fabs(glm::length(u)) &&
               fabs(glm::length(w)) > fabs(glm::length(v)))
               center = (pt3 + pt2) * 0.5f;

            if (fabs(glm::length(u)) > fabs(glm::length(w)) &&
               fabs(glm::length(u)) > fabs(glm::length(v)))
               center = (pt2 + pt1) * 0.5f;

            if (fabs(glm::length(v)) > fabs(glm::length(w)) &&
               fabs(glm::length(v)) > fabs(glm::length(u)))
               center = (pt3 + pt1) * 0.5f;

            glm::vec3 norm = glm::cross(u,v);
            
            bool faceFound = false;
            norm = glm::normalize(norm);
     
            for (int face = 0; face < newBox.faces.size(); face++)
            {
              
               float dist = glm::distance(center, newBox.faces.at(face).center);
   
               if (fabs(dist) < 0.000001)
               {  
                  faceFound = true;
                  if (newBox.points.at(newBox.faces.at(face).boxIndices[0]) != pt1 &&
                     newBox.points.at(newBox.faces.at(face).boxIndices[1]) != pt1 &&
                     newBox.points.at(newBox.faces.at(face).boxIndices[2]) != pt1)
                     newBox.faces.at(face).boxIndices[3] = pt1_indice;
                  else if (newBox.points.at(newBox.faces.at(face).boxIndices[0]) != pt2 &&
                     newBox.points.at(newBox.faces.at(face).boxIndices[1]) != pt2 &&
                     newBox.points.at(newBox.faces.at(face).boxIndices[2]) != pt2)
                     newBox.faces.at(face).boxIndices[3] = pt2_indice;
                  else if (newBox.points.at(newBox.faces.at(face).boxIndices[0]) != pt3 &&
                     newBox.points.at(newBox.faces.at(face).boxIndices[1]) != pt3 &&
                     newBox.points.at(newBox.faces.at(face).boxIndices[2]) != pt3) 
                     newBox.faces.at(face).boxIndices[3] = pt3_indice;
               }
            }
            
            if (!faceFound)
            {
               Face newFace;
               newFace.boxIndices[0] = pt1_indice;
               newFace.boxIndices[1] = pt2_indice;
               newFace.boxIndices[2] = pt3_indice;
               newFace.normal = norm;
               
               newFace.center = center;
               newBox.faces.push_back(newFace);

               //std::cout << center.x << " " << center.y << " " << center.z << std::endl;
               if (newBox.front == -1)
               {
                  newBox.front = newBox.faces.size() - 1;
               }
               else if (newBox.back == -1 && fabs(newBox.faces.at(newBox.front).normal.x + norm.x) < .0001 &&
                  fabs(newBox.faces.at(newBox.front).normal.y + norm.y) < .0001 &&
                  fabs(newBox.faces.at(newBox.front).normal.z + norm.z) < .0001)
               {
                  newBox.back = newBox.faces.size() - 1;
               }
               else if (newBox.left == -1)
               {
                  newBox.left = newBox.faces.size() - 1;
               }
               else if (newBox.right == -1 && fabs(newBox.faces.at(newBox.left).normal.x + norm.x) < .0001 &&
                  fabs(newBox.faces.at(newBox.left).normal.y + norm.y) < .0001 &&
                  fabs(newBox.faces.at(newBox.left).normal.z + norm.z) < .0001)
               {
                  newBox.right = newBox.faces.size() - 1;
               }
               else if (newBox.top == -1)
               {
                  newBox.top = newBox.faces.size() - 1;
               }
               else if (newBox.bottom == -1 && fabs(newBox.faces.at(newBox.top).normal.x + norm.x) < .0001 &&
                  fabs(newBox.faces.at(newBox.top).normal.y + norm.y) < .0001 &&
                  fabs(newBox.faces.at(newBox.top).normal.z + norm.z) < .0001)
               {
                  newBox.bottom = newBox.faces.size() - 1;

               }

            }

            culledIndicesVec.erase(culledIndicesVec.begin() + j, culledIndicesVec.begin() + j + 9);
            j = 0;
         }
         else
         {
            j += 9;
         }
         
         
      }

      if (newBox.faces.size() != 6)
      {
         std::cout << "HITBOX ERR: After loop there should be 6 faces in a single hit box" << std::endl;
         std::cout << "There are: " << newBox.faces.size() << " sets." << std::endl;
      }
      if (points.size() != 8) {
         std::cout << "HITBOX ERR: After loop there should be 8 indices of vertices in vector that contains all indices of cur box" << std::endl;
         std::cout << "There are: " << points.size() << " sets." << std::endl;
      }

     /* for (int i = 0; i < newBox.faces.size(); i++)
      {
         std::cout << "face " << i << std::endl;
         std::cout << "normal: " << newBox.faces.at(i).normal.x << " " << newBox.faces.at(i).normal.y << " " << newBox.faces.at(i).normal.z << std::endl;
         std::cout << "center: " << newBox.faces.at(i).center.x << " " << newBox.faces.at(i).center.y << " " << newBox.faces.at(i).center.z << std::endl;
      }
      std::cout << newBox.front << " " << newBox.back << std::endl;
      std::cout << newBox.left << " " << newBox.right<< std::endl;
      std::cout << newBox.top<< " " << newBox.bottom << std::endl;*/
      boxes.push_back(newBox);
   }

   return true;
   
}

void HitBoxes::RefreshHitBox()
{
   // We will need to move all the vertices according to M matrix, and the current animation
   glm::mat4 M = boundTo->getModelMatrix();

   //Each index relates to the transformation of that bone
   std::vector<glm::mat4> frames = animM;

   for (int bxInd = 0; bxInd < boxes.size();bxInd++)
   {
      boxes.at(bxInd).transformedPoints.clear();
      for (int ptInd = 0; ptInd < boxes.at(bxInd).points.size(); ptInd++)
      {
         boxes.at(bxInd).transformedPoints.push_back(
            // M * animM[joint] * bindPose[joint] * vertPos;
            glm::vec3(M * animM[boxes.at(bxInd).boneBoundTo] * bindPose[boxes.at(bxInd).boneBoundTo] * glm::vec4(boxes.at(bxInd).points.at(ptInd), 1.0f)));
      }

      for (int fcInd = 0; fcInd < boxes.at(bxInd).faces.size(); fcInd++)
      {
         boxes.at(bxInd).faces.at(fcInd).transformedNormal=
            // M * animM[joint] * bindPose[joint] * vertPos;
            glm::vec3(M * animM[boxes.at(bxInd).boneBoundTo] * bindPose[boxes.at(bxInd).boneBoundTo] * glm::vec4(boxes.at(bxInd).faces.at(fcInd).normal, 0.0f));
         boxes.at(bxInd).faces.at(fcInd).transformedCenter =
            glm::vec3(M * animM[boxes.at(bxInd).boneBoundTo] * bindPose[boxes.at(bxInd).boneBoundTo] * glm::vec4(boxes.at(bxInd).faces.at(fcInd).center, 1.0f));
      }
   }
}

bool HitBoxes::checkVectorCollision(glm::vec3 origin, glm::vec3 ray)
{

   
 /*  p is a point on plane
   n is the normal of plane
   o is the origin of ray
   d is direction of ray
   t = (p dot n - o dot n )/ (d dot n)
    */

   ray = glm::normalize(ray);
   for (int bxInd = 0; bxInd < boxes.size();bxInd++)
   {
      HitBox *curBox = &(boxes.at(bxInd));
      if (checkVectorAgainstBox(origin, ray, curBox))
      {
         std::cout << "hit at " << curBox->boneName << std::endl;

      }
   }
   return false;
}

bool checkFaceDist(float *tmax, float *tmin, glm::vec3 point1, glm::vec3 point2, glm::vec3 normal, glm::vec3 ray, glm::vec3 origin) 
{
   float denom = glm::dot(ray, normal);
   if (fabs(denom) > 0.00001f)
   {
      float t1 = (glm::dot(point1 - origin, normal)) / denom;
      float t2 = (glm::dot(point2 - origin, -normal)) / glm::dot(ray, -normal);

      if (t1 > t2)
      {
         float w = t1;
         t1 = t2;
         t2 = w;
      }

      // t1 is the lesser one
      if (t2 < *tmax)
         *tmax = t2;    

      if (t1 > *tmin)
         *tmin = t1;

      if (*tmax < *tmin)
         return false;

   }
   else
   {
      if(glm::dot(point1 - origin, normal)  < 0 || glm::dot(point2 - origin, normal) > 0)
         return false;
   }
   return true;
}

//https://github.com/opengl-tutorials/ogl/blob/master/misc05_picking/misc05_picking_custom.cpp
bool HitBoxes::checkVectorAgainstBox(glm::vec3 origin, glm::vec3 ray, HitBox *box)
{
  
   glm::vec3 point1 = box->faces.at(box->front).transformedCenter;
   glm::vec3 point2 = box->faces.at(box->back).transformedCenter;
   glm::vec3 normal = box->faces.at(box->front).transformedNormal;

   float tmax = FLT_MAX;
   float tmin = -FLT_MAX;

   if(checkFaceDist(&tmax, &tmin, point1, point2, normal, ray, origin) == false)
      return false;

   point1 = box->faces.at(box->left).transformedCenter;
   point2 = box->faces.at(box->right).transformedCenter;
   normal = box->faces.at(box->left).transformedNormal;

   if(checkFaceDist(&tmax, &tmin, point1, point2, normal, ray, origin) == false)
      return false;

   point1 = box->faces.at(box->top).transformedCenter;
   point2 = box->faces.at(box->bottom).transformedCenter;
   normal = box->faces.at(box->top).transformedNormal;

   if(checkFaceDist(&tmax, &tmin, point1, point2, normal, ray, origin) == false)
      return false;

  // std::cout << tmin << std::endl;


   return  true;
}

void HitBoxes::generateShapes()
{
   
}

void HitBoxes::bindToObject(GameObject *toBind)
{
   boundTo = toBind;
}

std::vector<HitBox *> HitBoxes::getHitBoxes()
{
   return std::vector<HitBox *>();
}

void HitBoxes::draw(MatrixStack MV, Program *prog) const
{
   MV.multMatrix(boundTo->getModelMatrix());
   glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
   
   ShapeObj::draw(prog);
}

void HitBoxes::drawSimple(MatrixStack MV, Program *prog) const
{
   MV.pushMatrix();
   MV.multMatrix(boundTo->getModelMatrix());
   glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
   ShapeObj::drawSimple(prog);
   MV.popMatrix();

}

bool HitBoxes::empty()
{ 
   if (boxes.size() == 0)
      return true;
   else 
      return false;
}