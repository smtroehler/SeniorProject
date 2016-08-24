#include <cstdlib>
#include <cstring>
#include <cassert>

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <cmath>
#include "tiny_obj_loader.h"
#include <iostream>
namespace tinyobj {

   void importSkeleton(shape_t& shape, std::ifstream& ifs) {
      //gets first num in bones

      std::string buffer;
      ifs >> buffer;

      int boneCount = 0;
      while (buffer != "end") {
         ifs >> buffer;
         shape.skeleton.bone_name.push_back(buffer);
         //std::cout << buffer << std::endl;
         ifs >> buffer;
         shape.skeleton.parent.push_back(stoi(buffer));
         //gets first num in bones or end
         ifs >> buffer;
         boneCount++;
         
      }

      shape.skeleton.numbones = boneCount;

      //skeleton time 0
      ifs >> buffer;
      ifs >> buffer;
      ifs >> buffer;
      anim toCreate;
      std::vector<anim> newThing;
      shape.animations = std::vector<anim>();
      toCreate.animName = std::string("bind_pose");
      keyframe frame;

      toCreate.numframes = 1;
      frame.positions.clear();
      frame.rotations.clear();

      ifs >> buffer;
      while (buffer != "end") {

         ifs >> buffer;
         frame.positions.push_back(stof(buffer));

         ifs >> buffer;
         frame.positions.push_back(stof(buffer));

         ifs >> buffer;
         frame.positions.push_back(stof(buffer));

         ifs >> buffer;
         frame.rotations.push_back(stof(buffer));

         ifs >> buffer;
         frame.rotations.push_back(stof(buffer));

         ifs >> buffer;
         frame.rotations.push_back(stof(buffer));

         ifs >> buffer;
      }

      toCreate.frames.push_back(frame);
      shape.animations.push_back(toCreate);
   }


   void importTriangles(shape_t& shape, std::ifstream& ifs) {
      int cur_vert = 0;
      int bone_num = 0;
      std::string line;
      std::string buffer;
      // first read in will be texture name. We aren't going to use this
      ifs >> buffer;
      while (buffer != "end") {



         for (int i = 0; i < 3; i++) {
            // read in bone parent. not used in this context
            ifs >> buffer;

            ifs >> buffer;
            shape.mesh.positions.push_back(stof(buffer));
            ifs >> buffer;
            shape.mesh.positions.push_back(stof(buffer));
            ifs >> buffer;
            shape.mesh.positions.push_back(stof(buffer));

            ifs >> buffer;
            shape.mesh.normals.push_back(stof(buffer));
            ifs >> buffer;
            shape.mesh.normals.push_back(stof(buffer));
            ifs >> buffer;
            shape.mesh.normals.push_back(stof(buffer));

            ifs >> buffer;
            float t1, t2;
            t1 = stof(buffer);
            ifs >> buffer;
            t2 = stof(buffer);

            if (std::abs(t1) > 1)
               t1 = std::abs(t1) - std::abs(floor(t1));
            if (std::abs(t2) > 1)
               t2 = std::abs(t2) - std::abs(floor(t2));

            shape.mesh.texcoords.push_back(t1);
            shape.mesh.texcoords.push_back(1 - t2);

            // number of links
            ifs >> buffer;
            shape.mesh.numbonesbinded.push_back(stoi(buffer));
            shape.mesh.boneindices.push_back(bone_num);

            int bonecount = stoi(buffer);
            bone_num += bonecount;

            for (int j = 0; j < bonecount;j++) {
               ifs >> buffer;

               shape.mesh.boneparents.push_back(stoi(buffer));

               ifs >> buffer;
               shape.mesh.boneweights.push_back(stof(buffer));

            }
            shape.mesh.indices.push_back(cur_vert + 0);
            shape.mesh.indices.push_back(cur_vert + 1);
            shape.mesh.indices.push_back(cur_vert + 2);
            cur_vert += 3;
         }
         

         
         // first read in will be texture name. We aren't going to use this
         ifs >> buffer;
      }

   }


   void LoadSmd(
      std::vector<shape_t>& shapes,
      const char* filename)
   {
    //  std::cout << filename << std::endl;
   //   shapes.clear();

      shape_t shape;


      std::ifstream ifs(filename);

      if (!ifs) {
         std::cerr << "Cannot open file [" << filename << "]" << std::endl;
         return;
      }
      shape.name = filename;
      shape.numAnimations = 1;

      std::string buffer;

      //gets "version 1 nodes"
      while (buffer.compare("nodes") != 0)
         ifs >> buffer;
      importSkeleton(shape, ifs);
      ifs >> buffer;
      importTriangles(shape, ifs);

      //shape = shape_smd();
      shapes.push_back(shape);



   }

   void LoadSmdAnimation(
      shape_t &shape,
      const char* filename)
   {
      shape_t newshape;
      std::string buffer;
      shape.numAnimations++;
      std::ifstream ifs(filename);
      if (!ifs) {
         std::cerr << "Cannot open file [" << filename << "]" << std::endl;
         return;
      }

      ifs >> buffer;
      while (buffer != "time") {
         if (ifs.eof()) {
            std::cerr << "Something is wrong with [" << filename << "]. Could not find time string indicating beginning of animation" << std::endl;
            return;
         }
         ifs >> buffer;
      }


      anim animation;
      animation.animName = filename;

      //reads in 0 at time 0
      ifs >> buffer;
      ifs >> buffer;

      int framecount = 0;
      while (buffer != "end") {

         framecount++;
         keyframe key;
         key.rotations.clear();
         key.positions.clear();
         while (buffer != "time" && buffer != "end") {




            ifs >> buffer;
            key.positions.push_back(stof(buffer));
            ifs >> buffer;
            key.positions.push_back(stof(buffer));
            ifs >> buffer;
            key.positions.push_back(stof(buffer));
            ifs >> buffer;
            key.rotations.push_back(stof(buffer));
            ifs >> buffer;
            key.rotations.push_back(stof(buffer));
            ifs >> buffer;
            key.rotations.push_back(stof(buffer));

            //read in index, not used or it will read in time
            ifs >> buffer;

         }

         animation.frames.push_back(key);
         if (buffer == "end")
            break;

         ifs >> buffer;
         ifs >> buffer;
      }
      animation.numframes = framecount;
      shape.animations.push_back(animation);

   }

}