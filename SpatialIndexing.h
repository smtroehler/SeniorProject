#pragma once
#ifndef __SpatialIndexing__
#define __SpatialIndexing__

#include "GameObject.h"
#include "glm/glm.hpp"

struct XZBound
{
   glm::vec2 center;
   float w;
   float h;
};

class QuadTreeLevel
{
   public:


      QuadTreeLevel(int lvl, XZBound newBnds);
      QuadTreeLevel(int lvl, glm::vec2 bnds[]);
      ~QuadTreeLevel();
      void createTree(std::vector<GameObject *> shape_objects);
      void clear();
      void insert(GameObject *toInsert);
      std::vector<GameObject *> retrieve(GameObject *toFind);
      int getIndex(GameObject *toFind);

      void split();
   private:
      
      

      int MAX_LEVELS = 10;
      int MAX_OBJECTS = 3;
      int level;
    //  int width;
    //  int height;
      std::vector<QuadTreeLevel* > children;
    //  glm::vec2 center;
      static const int NUM_CHILDREN = 4;

      XZBound bnds;

      //glm::vec2 bounds[4]; //Need 4 coords that determine this items boundaries  
      // [0] +X+Z, [1] +X-Z, [2] -X-Z, [3] -X+Z
      //QuadTreeLevel parent;
      std::vector<GameObject *> objects_contained;

};



#endif