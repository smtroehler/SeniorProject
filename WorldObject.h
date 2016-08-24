#pragma once
#ifndef __WorldObject__
#define __WorldObject__

#include "ShapeObj.h"

class WorldObject
{
public:
   enum {
      IN_GAME = 0,
      IN_MENU

   };
   int cur_game_state;



private:
   std::vector<ShapeObj> inplayObjects;

  


   

};
#endif

