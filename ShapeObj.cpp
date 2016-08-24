#define GLM_FORCE_RADIANS
#include <iostream>
#include "ShapeObj.h"
#include "GLSL.h"
#include <assert.h>
#include <cmath> 
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"


using namespace std;

ShapeObj::ShapeObj() :
   posBufID(0),
   norBufID(0),
   texBufID(0),
   indBufID(0)
{
   type = "ShapeObject";
}

ShapeObj::~ShapeObj()
{
}

bool ShapeObj::load(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::material_t> objMaterials;
   string err;
   name = meshName;
   if (meshName.length() < 4)
   {
      std::cout << "Could Not load file, too short of name" << std::endl;
      return false;
   }

   if (meshName.compare(meshName.length() - 4, 4, ".obj") == 0)
      err = tinyobj::LoadObj(shapes, objMaterials, meshName.c_str());
   else if (meshName.compare(meshName.length() - 4, 4, ".smd") == 0) {
      tinyobj::LoadSmd(shapes, meshName.c_str());
      
      initializeBindPose();
      animated = true;
   }
   else
   {
      cerr << "No fitting file ending" << endl;
      return false;
   }

	if(!err.empty()) {
		cerr << err << endl;
      return false;
	}
   return true;

}

void ShapeObj::normalizeObject()
{
   float cur_max = 0;
   for (int i = 0; i < shapes[0].mesh.positions.size(); i++)
   {
      if (cur_max < abs(shapes[0].mesh.positions[i]))
      {
         cur_max = abs(shapes[0].mesh.positions[i]);
      }
   }

   for (int i = 0; i < shapes[0].mesh.positions.size(); i++)
   {
      shapes[0].mesh.positions[i] = shapes[0].mesh.positions[i] / cur_max;

   }

}


void ShapeObj::init()
{
	// Send the position array to the GPU
   posBuf = shapes[0].mesh.positions;
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	 


	// Send the normal array (if it exists) to the GPU
	const vector<float> &norBuf = shapes[0].mesh.normals;
	if(!norBuf.empty()) {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	} else {
		norBufID = 0;
	}
	
	// Send the texture coordinates array (if it exists) to the GPU
	const vector<float> &texBuf = shapes[0].mesh.texcoords;
	if(!texBuf.empty()) {
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	} else {
		texBufID = 0;
	}
	
	// Send the index array to the GPU
	const vector<unsigned int> &indBuf = shapes[0].mesh.indices;
	glGenBuffers(1, &indBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);
   if (animated)
   {
      glGenBuffers(1, &boneBufID);
      glBindBuffer(GL_ARRAY_BUFFER, boneBufID);
      glBufferData(GL_ARRAY_BUFFER, boneBuf.size()*sizeof(float), &boneBuf[0], GL_STATIC_DRAW);

      glGenBuffers(1, &weightBufID);
      glBindBuffer(GL_ARRAY_BUFFER, weightBufID);
      glBufferData(GL_ARRAY_BUFFER, weightBuf.size()*sizeof(float), &weightBuf[0], GL_STATIC_DRAW);



      glGenBuffers(1, &numWeightsBufID);
      glBindBuffer(GL_ARRAY_BUFFER, numWeightsBufID);
      glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.numbonesbinded.size()*sizeof(int), &(shapes[0].mesh.numbonesbinded[0]), GL_STATIC_DRAW);
   }
   else {
      for (int i = 0; i < indBuf.size(); i++)
         numJointsAtVertex.push_back(0);

      glGenBuffers(1, &numWeightsBufID);
      glBindBuffer(GL_ARRAY_BUFFER, numWeightsBufID);
      glBufferData(GL_ARRAY_BUFFER, numJointsAtVertex.size()*sizeof(int), &(numJointsAtVertex[0]), GL_STATIC_DRAW);
   }

   // Unbind the arrays
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   assert(glGetError() == GL_NO_ERROR);

}


void ShapeObj::draw(Program *prog) const
{
   if (prog->getAttribute("vertPos") >= 0)
   {
      GLSL::enableVertexAttribArray(prog->getAttribute("vertPos"));
      glBindBuffer(GL_ARRAY_BUFFER, posBufID);
      glVertexAttribPointer(prog->getAttribute("vertPos"), 3, GL_FLOAT, GL_FALSE, 0, 0);
   }
   else
   {
      std::cout << "NO VERTPOS\n";
   }
	// Enable and bind normal array (if it exists) for drawing
	if(norBufID && prog->getAttribute("vertNor") > 0) {
		GLSL::enableVertexAttribArray(prog->getAttribute("vertNor"));
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(prog->getAttribute("vertNor"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Enable and bind texcoord array (if it exists) for drawing
	if(texBufID && prog->getAttribute("vertTex") > 0) {
		GLSL::enableVertexAttribArray(prog->getAttribute("vertTex"));
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glVertexAttribPointer(prog->getAttribute("vertTex"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	


   if (animated) {
      if (weightBufID && prog->getAttribute("weights0") > 0) {
         GLSL::enableVertexAttribArray(prog->getAttribute("weights0"));
         GLSL::enableVertexAttribArray(prog->getAttribute("weights1"));
         GLSL::enableVertexAttribArray(prog->getAttribute("weights2"));
         GLSL::enableVertexAttribArray(prog->getAttribute("weights3"));
         glBindBuffer(GL_ARRAY_BUFFER, weightBufID);

         unsigned stride = 16 * sizeof(float);
         glVertexAttribPointer(prog->getAttribute("weights0"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(0 * sizeof(float)));
         glVertexAttribPointer(prog->getAttribute("weights1"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(4 * sizeof(float)));
         glVertexAttribPointer(prog->getAttribute("weights2"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(8 * sizeof(float)));
         glVertexAttribPointer(prog->getAttribute("weights3"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(12 * sizeof(float)));


         GLSL::enableVertexAttribArray(prog->getAttribute("bones0"));
         GLSL::enableVertexAttribArray(prog->getAttribute("bones1"));
         GLSL::enableVertexAttribArray(prog->getAttribute("bones2"));
         GLSL::enableVertexAttribArray(prog->getAttribute("bones3"));
         glBindBuffer(GL_ARRAY_BUFFER, boneBufID);

         glVertexAttribPointer(prog->getAttribute("bones0"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(0 * sizeof(float)));
         glVertexAttribPointer(prog->getAttribute("bones1"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(4 * sizeof(float)));
         glVertexAttribPointer(prog->getAttribute("bones2"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(8 * sizeof(float)));
         glVertexAttribPointer(prog->getAttribute("bones3"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(12 * sizeof(float)));

         glUniformMatrix4fv(prog->getUniform("animM"), 18, GL_FALSE, glm::value_ptr(animM[0]));
         glUniformMatrix4fv(prog->getUniform("bindPose"), 18, GL_FALSE, glm::value_ptr(bindPose[0]));
      }
   }

   if (numWeightsBufID && prog->getAttribute("numWeights") > 0)
   {
      GLSL::enableVertexAttribArray(prog->getAttribute("numWeights"));
      glBindBuffer(GL_ARRAY_BUFFER, numWeightsBufID);
      glVertexAttribPointer(prog->getAttribute("numWeights"), 1, GL_INT, GL_FALSE, 0, 0);
   }

   // Bind index array for drawing
   int nIndices = (int)shapes[0].mesh.indices.size();
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufID);
   // Draw
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   // Disable and unbind
   if (texBufID && prog->getAttribute("vertTex") >= 0) {
      GLSL::disableVertexAttribArray(prog->getAttribute("vertTex"));
   }

   if (norBufID && prog->getAttribute("vertNor") >= 0) {
      GLSL::disableVertexAttribArray(prog->getAttribute("vertNor"));
   }

   GLSL::disableVertexAttribArray(prog->getAttribute("vertPos"));
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   if (numWeightsBufID && prog->getAttribute("numWeights") > 0)
   {
      GLSL::disableVertexAttribArray(prog->getAttribute("numWeights"));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   }
   assert(glGetError() == GL_NO_ERROR);

}

void ShapeObj::drawSimple(Program *prog) const
{
   GLSL::enableVertexAttribArray(prog->getAttribute("vertPos"));
   glBindBuffer(GL_ARRAY_BUFFER, posBufID);
   glVertexAttribPointer(prog->getAttribute("vertPos"), 3, GL_FLOAT, GL_FALSE, 0, 0);

   if (animated) {

      GLSL::enableVertexAttribArray(prog->getAttribute("weights0"));
      GLSL::enableVertexAttribArray(prog->getAttribute("weights1"));
      GLSL::enableVertexAttribArray(prog->getAttribute("weights2"));
      GLSL::enableVertexAttribArray(prog->getAttribute("weights3"));
      glBindBuffer(GL_ARRAY_BUFFER, weightBufID);

      unsigned stride = 16 * sizeof(float);
      glVertexAttribPointer(prog->getAttribute("weights0"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(0 * sizeof(float)));
      glVertexAttribPointer(prog->getAttribute("weights1"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(4 * sizeof(float)));
      glVertexAttribPointer(prog->getAttribute("weights2"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(8 * sizeof(float)));
      glVertexAttribPointer(prog->getAttribute("weights3"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(12 * sizeof(float)));


      GLSL::enableVertexAttribArray(prog->getAttribute("bones0"));
      GLSL::enableVertexAttribArray(prog->getAttribute("bones1"));
      GLSL::enableVertexAttribArray(prog->getAttribute("bones2"));
      GLSL::enableVertexAttribArray(prog->getAttribute("bones3"));
      glBindBuffer(GL_ARRAY_BUFFER, boneBufID);

      glVertexAttribPointer(prog->getAttribute("bones0"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(0 * sizeof(float)));
      glVertexAttribPointer(prog->getAttribute("bones1"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(4 * sizeof(float)));
      glVertexAttribPointer(prog->getAttribute("bones2"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(8 * sizeof(float)));
      glVertexAttribPointer(prog->getAttribute("bones3"), 4, GL_FLOAT, GL_FALSE, stride, (const void *)(12 * sizeof(float)));

      glUniformMatrix4fv(prog->getUniform("animM"), 18, GL_FALSE, glm::value_ptr(animM[0]));
      glUniformMatrix4fv(prog->getUniform("bindPose"), 18, GL_FALSE, glm::value_ptr(bindPose[0]));

   }
   GLSL::enableVertexAttribArray(prog->getAttribute("numWeights"));
   glBindBuffer(GL_ARRAY_BUFFER, numWeightsBufID);
   glVertexAttribPointer(prog->getAttribute("numWeights"), 1, GL_INT, GL_FALSE, 0, 0);


   // Bind index array for drawing
   int nIndices = (int)shapes[0].mesh.indices.size();
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufID);

   // Draw
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   GLSL::disableVertexAttribArray(prog->getAttribute("vertPos"));
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   GLSL::disableVertexAttribArray(prog->getAttribute("numWeights"));
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   assert(glGetError() == GL_NO_ERROR);
}

#include "MatrixStack.h"


glm::mat4 ShapeObj::getTotalMatforFrame(int joint, tinyobj::keyframe kf, vector<int> parents) {
   std::vector<float> rots = kf.rotations;
   std::vector<float> pos = kf.positions;

   std::vector<glm::mat4> mats;
   mats.clear();
   do
   {
      glm::vec3 p = glm::vec3(pos[joint * 3 + 0], pos[joint * 3 + 1], pos[joint * 3 + 2]);
      glm::mat4 other = glm::eulerAngleYXZ(0.0f, 0.0f, rots[joint * 3 + 2]);
      glm::mat4 mrot = glm::eulerAngleYXZ(rots[joint * 3 + 1], rots[joint * 3 + 0], 0.0f );
      mrot = other * mrot;

      mrot[3] = glm::vec4(p, mrot[3][3]);
      mats.push_back(mrot);
      joint = parents[joint];

   } while (joint != -1);
   glm::mat4 out = glm::mat4(1.0f);

   if (1 < mats.size())
   {
      out = mats[1] * mats[0] ;
   }
   else
      out = mats[0];

   for (int i = 2; i < mats.size(); i++)
   {
      out = mats[i] * out;
   }

   return out;
}

std::vector<glm::mat4> ShapeObj::getFrame(tinyobj::keyframe kf) {

   std::vector<glm::mat4> frame;
   frame.clear();

   int numJoints = shapes[0].skeleton.numbones;

   for (int j = 0; j < numJoints; j++) {
      frame.push_back(getTotalMatforFrame(j, kf, shapes[0].skeleton.parent)); 
   }
   return frame;
}

void ShapeObj::AnimationToMatrix(int animationIndex) {
   int numFrames = shapes[0].animations[animationIndex].numframes;

   std::vector<tinyobj::keyframe> frames = shapes[0].animations[animationIndex].frames;

   std::vector< std::vector<glm::mat4>> out;

   for (int i = 0; i < numFrames; i++) {

      out.push_back(getFrame(frames[i])); 
   }
   transformFrames.push_back(out);
   animM = transformFrames[transformFrames.size() - 1][0];
}

int ShapeObj::getNumFrames(int animationIndex) {
   return shapes[0].animations[animationIndex].numframes;
}

void ShapeObj::BindPoseToMatrix() {
   std::vector<tinyobj::keyframe> frames = shapes[0].animations[0].frames;
   std::vector< std::vector<glm::mat4>> out;
   // Only call once since bind pose only has one frame of animation
   bindPose = getFrame(frames[0]);
   out.push_back(bindPose);
   transformFrames.push_back(out);

   int numbones = shapes[0].skeleton.numbones;
   for (int i = 0; i < numbones; i++) {
      glm::mat4 inv = glm::inverse(bindPose[i]);
      bindPose[i] = inv;
   }

}

void ShapeObj::initializeBindPose()
{

      int nVerts = shapes[0].mesh.positions.size() / 3;

      for (int i = 0; i < nVerts; i++) {
         for (int j = 0; j < 16; j++) {
            int numBonesAtVertex = shapes[0].mesh.numbonesbinded[i];
            int startingPosofBoneList = shapes[0].mesh.boneindices[i];
            if (j < numBonesAtVertex) {
               int joint = shapes[0].mesh.boneparents[startingPosofBoneList + j];


               boneBuf.push_back(joint);
               weightBuf.push_back(shapes[0].mesh.boneweights[startingPosofBoneList + j]);

            }
            else {
               boneBuf.push_back(0);
               weightBuf.push_back(0);
            }
         }
      }

      int numJoints = shapes[0].skeleton.numbones;
      animM.clear();
      for (int i = 0; i < numJoints; i++)
         animM.push_back(glm::mat4(1.0f));
      BindPoseToMatrix();
}

void ShapeObj::loadSmdAnim(const std::string &filename) {
   tinyobj::LoadSmdAnimation(shapes[0], filename.c_str());
   AnimationToMatrix(transformFrames.size());
   
}

void ShapeObj::animate(int frame, int animation) {
   shapes[0].animations;

   frame = frame % shapes[0].animations[animation].numframes;
   curFrame = frame;
   animM.clear();
   animM = transformFrames[animation][frame];
}

std::vector<glm::mat4> ShapeObj::getAnimationMatricesForCurFrame(int animation) {

   return transformFrames[animation][curFrame];
}

std::vector<glm::mat4> ShapeObj::getBindPose()
{
   return std::vector<glm::mat4>(bindPose);
}

std::vector<tinyobj::shape_t *> ShapeObj::getShapes()
{
   std::vector<tinyobj::shape_t *> out;
   for (int i = 0; i < shapes.size(); i++)
      out.push_back(&(shapes.at(i)));
   return out;
};