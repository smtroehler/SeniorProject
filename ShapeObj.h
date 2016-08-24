#pragma once
#ifndef _SHAPEOBJ_H_
#define _SHAPEOBJ_H_

#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "Program.h"
#include <cstring>


class ShapeObj
{
public:
	ShapeObj();
	virtual ~ShapeObj();
   virtual bool load(const std::string &meshName);

	virtual void init();

   virtual bool load() { return false; };
   void normalizeObject();
	virtual void draw(Program *prog) const;
   
   std::vector<float> posBuf;
   


   // Animation stuff
   void loadSmdAnim(const std::string &filename);

   int getNumFrames(int animationIndex);
   virtual void animate(int frame, int animation);
   void AnimationToMatrix(int anim_index);
   std::vector<glm::mat4> getFrame(tinyobj::keyframe kf);
   void BindPoseToMatrix();
   glm::mat4 getTotalMatforFrame(int joint, tinyobj::keyframe kf, std::vector<int> parents);
   int curFrame = 0;
   std::vector<glm::mat4> getAnimationMatricesForCurFrame(int animation);
   std::vector<glm::mat4> getBindPose();
   std::string name;
   std::vector<tinyobj::shape_t> shapes;
   void drawSimple(Program *prog) const;

   void initializeBindPose();
   std::string type;
   bool animated = false;

   virtual std::vector<tinyobj::shape_t *> getShapes();
   

   unsigned posBufID;
   unsigned norBufID;
   unsigned texBufID;
   unsigned indBufID;
protected:
   
   



   //animation stuff
   std::vector<glm::mat4> getBindFrame(tinyobj::keyframe kf);
   glm::mat4 getBindTotalMatforFrame(int joint, tinyobj::keyframe kf, std::vector<int> parents);
   std::vector<float> weightBuf;
   std::vector<float> boneBuf;
   std::vector<float> numWeightsBuf;
   std::vector<int> numJointsAtVertex;
   std::vector<glm::mat4> animM;
   std::vector<float> animVec;
   std::vector<float> bindVec;
   //animation - frame - bone
   std::vector<std::vector< std::vector<glm::mat4>> >transformFrames;
   std::vector<glm::mat4> bindPose;

   unsigned boneBufID;
   unsigned weightBufID;
   unsigned numWeightsBufID;
};



#endif
