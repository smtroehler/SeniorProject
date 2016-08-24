#include "SpatialIndexing.h"
#include <iostream>


QuadTreeLevel::QuadTreeLevel(int lvl, XZBound newBnds)
{
   bnds = newBnds;
   level = lvl;
}

QuadTreeLevel::~QuadTreeLevel()
{

}

void QuadTreeLevel::createTree(std::vector<GameObject *> shape_objects)
{

}
void QuadTreeLevel::clear()
{
   // erase the pointers to the items in this level
   objects_contained.clear();
   for (int i = 0; i < children.size(); i++)
   {
         children[i]->clear();
   }
   // erase the actual quad tree levels from the vector
   children.clear();
}


std::vector<GameObject *> QuadTreeLevel::retrieve(GameObject* toFind)
{
   int index = getIndex(toFind);
   std::vector<GameObject *> out;
   out.clear();

   if (index != -1 && children.empty() == false)
   {
      out = children[index]->retrieve(toFind);
   }

   out.insert(out.end(), objects_contained.begin(), objects_contained.end());

   return out;
}

void QuadTreeLevel::insert(GameObject *toInsert)
{
   // check if this level is split into quads.
   if (children.empty() == false)
   {
      int index = getIndex(toInsert);
     // std::cout << index << std::endl;
      // if it fits in one of the quadrants, put it there. 
      if (index != -1)
      {
         children[index]->insert(toInsert);
         return;
      }
   }
   
   // if this level is not split into quads or the object does not fit into one of the quads,
   // just add it to this level.
   objects_contained.push_back(toInsert);

   // if the number of objects for this level has reached its capacity, then split this
   // level into quads
   if (objects_contained.size() > MAX_LEVELS && level < MAX_LEVELS)
   {
      
      if (children.empty() == true)
      {
         // std::cout << "split" << std::endl;
         split();
      }
      int i = 0;
      while (i < objects_contained.size())
      {
         int index = getIndex(objects_contained.at(i));
         if (index != -1)
         {
            children[index]->insert(objects_contained.at(i));
            objects_contained.erase(objects_contained.begin() + i);
         }
         else
            i++;
      }
      

      
   }
}

void QuadTreeLevel::split()
{
   XZBound newBound;
   // for upper left quad
   newBound.center = glm::vec2(bnds.center.x - bnds.w / 4, bnds.center.y + bnds.h / 4);
   newBound.w = bnds.w / 2;
   newBound.h = bnds.h / 2;
   children.push_back(new QuadTreeLevel(level + 1, newBound));
  
   // for upper right quad
   newBound = XZBound();
   newBound.center = glm::vec2(bnds.center.x + bnds.w / 4, bnds.center.y + bnds.h / 4);
   newBound.w = bnds.w / 2;
   newBound.h = bnds.h / 2;
   children.push_back(new QuadTreeLevel(level + 1, newBound));

   // for lower right quad
   newBound = XZBound();
   newBound.center = glm::vec2(bnds.center.x + bnds.w / 4, bnds.center.y - bnds.h / 4);
   newBound.w = bnds.w / 2;
   newBound.h = bnds.h / 2;
   children.push_back(new QuadTreeLevel(level + 1, newBound));

   // for lower left quad
   newBound = XZBound();
   newBound.center = glm::vec2(bnds.center.x - bnds.w / 4, bnds.center.y - bnds.h / 4);
   newBound.w = bnds.w / 2;
   newBound.h = bnds.h / 2;
   children.push_back(new QuadTreeLevel(level + 1, newBound));
}
int QuadTreeLevel::getIndex(GameObject *toFind)
{
   int index = -1; 
   glm::vec3 position = toFind->getPosition();
   BoundingBox bbox = toFind->getBBox();
  
   // pZ - positive Z quadrant
   bool pZQuad = (position.z > bnds.center.y && position.z - bbox.max_radius > bnds.center.y);
   // nZ - negatve Z quadrant
   bool nZQuad = (position.z < bnds.center.y);
   // pX - positive X quadrant
   bool pXQuad = (position.x > bnds.center.x && position.x - bbox.max_radius > bnds.center.x);
   // nX - negative X quadrant
   bool nXQuad = (position.x < bnds.center.x);
 //  std::cout << "position of object w/ radius: " << position.x << " " << position.z << " " << bbox.max_radius << std::endl;
 //  std::cout << "center of level: " << center.x << " " << center.y << std::endl;
   //std::cout << position.z << " " << center.y << " " << bbox.max_radius << std::endl;
   if (nXQuad && pZQuad)
      return 0;
   else if (pXQuad && pZQuad)
      return 1;
   else if (pXQuad && nZQuad)
      return 2;
   else if (nXQuad && nZQuad)
      return 3;

   return -1;
}
