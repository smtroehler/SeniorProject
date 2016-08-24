#include "GameObject.h"
#include <iostream>
#include "GLSL.h"
GameObject::GameObject(): ShapeObj(),
   rotation_o(0.0f,0.0f,0.0f),
   position_o(0.0f, 0.0f, 0.0f),
   scale_o(1.0f, 1.0, 1.0f),
   velocity_o(0.0f, 0.0f, 0.0f),
   cameraOffset(0.0f, 0.0f, 0.0f),
   textureScale(1.0f, 1.0f)
{
   type = "GameObject";
}

GameObject::GameObject(GameObject *toCopy)
{
    position_o = glm::vec3(toCopy->position_o);
    rotation_o = glm::vec3(toCopy->rotation_o);
    scale_o = glm::vec3(toCopy->scale_o);
    velocity_o = glm::vec3(toCopy->velocity_o);
    textureScale = glm::vec2(toCopy->textureScale);
    shapes = toCopy->shapes;
    boundingBox = toCopy->boundingBox;
    tempBBox = toCopy->tempBBox;
    cameraOffset = toCopy->cameraOffset;
    diffuseTexture = toCopy->diffuseTexture;
    posBuf = toCopy->posBuf;
    animated = toCopy->animated;
    weightBuf = toCopy->weightBuf;
    boneBuf = toCopy->boneBuf;
    numWeightsBuf = toCopy->numWeightsBuf;
    numJointsAtVertex = toCopy->numJointsAtVertex;
    animM = toCopy->animM;
    animVec = toCopy->animVec;
    bindVec = toCopy->bindVec;
    transformFrames = toCopy->transformFrames;
    bindPose = toCopy->bindPose;
    name  = toCopy->name;
    hitBoxes = toCopy->hitBoxes;
}

GameObject::~GameObject()
{

}

void GameObject::loadSmdAnim(const std::string &filename)
{
   ShapeObj::loadSmdAnim(filename);
   if(hitBoxes.empty() == false)
      hitBoxes.loadSmdAnim(filename);
}

void GameObject::loadDiffuseTexture(const std::string &f)
{
   diffuseTexture = new Texture();
   diffuseTexture->setFilename(f);
   
}

void GameObject::normalizeObject()
{
   float cur_max = 0;
   for (int i = 0; i < shapes[0].mesh.positions.size(); i++)
   {
      if (cur_max < abs(shapes[0].mesh.positions[i]))
      {
         cur_max = abs(shapes[0].mesh.positions[i]);
      }
   }
   setScale(glm::vec3(1 / cur_max, 1 / cur_max, 1 / cur_max));

}

void GameObject::init()
{
   if (diffuseTexture != NULL) 
      diffuseTexture->init();

   ShapeObj::init();
   generateBBox();
}

void GameObject::update(float dt)
{
   
   prev_position_o = position_o;
   position_o += (velocity_o * dt);
}

void GameObject::setDiffuseTexture(Texture *in) 
{
   diffuseTexture = in; 
}

void GameObject::draw(MatrixStack MV, Program *prog)
{
   MV.pushMatrix();

   
   MV.translate(position_o);
   MV.rotate(rotation_o.z, glm::vec3(0.0f, 0.0, 1.0));
   MV.rotate(rotation_o.y, glm::vec3(0.0f, 1.0, 0.0));
   MV.rotate(rotation_o.x, glm::vec3(1.0f, 0.0, 0.0));
   MV.scale(scale_o);

   if (diffuseTexture != NULL && prog->getUniform("texture0") > 0)
      diffuseTexture->bind(prog->getUniform("texture0"), 0);

   if(prog->getUniform("UVScale") > 0)
      glUniform2fv(prog->getUniform("UVScale"), 1, glm::value_ptr(textureScale));

   if(prog->getUniform("highlighted") > 0)
      if(highlightObject)
         glUniform1i(prog->getUniform("highlighted"), 1);
      else
         glUniform1i(prog->getUniform("highlighted"), 0);
   glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));

   ShapeObj::draw(prog);
   MV.popMatrix();

   if (diffuseTexture != NULL && prog->getUniform("texture0") > 0)
      diffuseTexture->unbind();
}


void GameObject::setRotation(glm::vec3 rot)
{
   rotation_o = glm::vec3(rot);
}

glm::vec3 GameObject::getRotation()
{
   return glm::vec3(rotation_o);
}

void GameObject::setScale(glm::vec3 scl)
{
   scale_o = glm::vec3(scl);
}

glm::vec3 GameObject::getScale()
{
   return glm::vec3(scale_o);
}


void GameObject::setPosition(glm::vec3 pos)
{
   position_o = glm::vec3(pos);
}

void GameObject::translate(glm::vec3 pos)
{
   position_o = glm::vec3(pos + position_o);
}

void GameObject::scale(glm::vec3 scl)
{
   scale_o = glm::vec3(scl.x * scale_o.x, scl.y * scale_o.y, scl.z * scale_o.z);
}

void GameObject::rotate(glm::vec3 rot)
{
   rotation_o += rot;
}

glm::vec3 GameObject::getPosition()
{

   return glm::vec3(position_o);
}

void GameObject::scaleTexture(glm::vec2 in)
{
   textureScale *= in;
}
void GameObject::setTextureScale(glm::vec2 in)
{
   textureScale = in;
}

void GameObject::setScaleNormalizedUV(glm::vec2 in)
{
   return;
}

void GameObject::setHighlight(bool in)
{ 
   highlightObject = in; 
}

glm::mat4 GameObject::getModelMatrix()
{
   MatrixStack MV = MatrixStack();
   MV.pushMatrix();
   MV.translate(position_o);
   MV.rotate(rotation_o.z, glm::vec3(0.0f, 0.0, 1.0));
   MV.rotate(rotation_o.y, glm::vec3(0.0f, 1.0, 0.0));
   MV.rotate(rotation_o.x, glm::vec3(1.0f, 0.0, 0.0));
   MV.scale(scale_o);

   return MV.topMatrix();
}


int GameObject::getObjectID()
{
   return objectID;
}

void GameObject::setObjectID(int newID)
{
   objectID = newID;
}

bool checkOverlap(BoundingBox *thisbbx, BoundingBox *otherbbx)
{
   glm::vec3 dist = otherbbx->position - thisbbx->position;

   // check the length of the distance vector against the max sizes of the boxes
   if (fabs(dist.x) <= ((thisbbx->max.x - thisbbx->min.x) / 2) + ((otherbbx->max.x - otherbbx->min.x) / 2) &&
       fabs(dist.y) <= ((thisbbx->max.y - thisbbx->min.y) / 2) + ((otherbbx->max.y - otherbbx->min.y) / 2) &&
       fabs(dist.z) <= ((thisbbx->max.z - thisbbx->min.z) / 2) + ((otherbbx->max.z - otherbbx->min.z) / 2))
      return true;
   else
      return false;
}

// Returns a vector indicating the normal of the collision
glm::vec3 GameObject::checkCollision(GameObject *other)
{
   
   BoundingBox otherbbx = other->getBBox();
   BoundingBox thisbbx = getBBox();
   
   if (checkOverlap(&thisbbx, &otherbbx))
   {
      std::cout << "HIT on: " << name << " and " << other->name <<std::endl;
      return glm::vec3(0.0, 0.0, 0.0);
   }
   return glm::vec3(0.0, 0.0, 0.0);
}




void GameObject::RefreshBBox() {
   BoundingBox out;
   out.position = position_o;
   out.velocity = velocity_o;
   out.scale = scale_o;
   out.rotation = rotation_o;
   out.min = glm::vec3(boundingBox.min);
   out.max = glm::vec3(boundingBox.min);
   //Tri used for XZ at max y plane to find new min and max after object has been rotated;
   std::vector<glm::vec4 > tri1;
   tri1.push_back(glm::vec4(boundingBox.min.x, boundingBox.min.y, boundingBox.min.z, 1.0f));
   tri1.push_back(glm::vec4(boundingBox.max.x, boundingBox.min.y, boundingBox.min.z, 1.0f));
   tri1.push_back(glm::vec4(boundingBox.min.x, boundingBox.min.y, boundingBox.max.z, 1.0f));

   std::vector<glm::vec4 > tri2;
   tri2.push_back(glm::vec4(boundingBox.min.x, boundingBox.max.y, boundingBox.min.z, 1.0f));
   tri2.push_back(glm::vec4(boundingBox.max.x, boundingBox.max.y, boundingBox.min.z, 1.0f));
   tri2.push_back(glm::vec4(boundingBox.min.x, boundingBox.max.y, boundingBox.max.z, 1.0f));

   std::vector<glm::vec4 > tri3;
   tri3.push_back(glm::vec4(boundingBox.max.x, boundingBox.min.y, boundingBox.max.z, 1.0f));
   tri3.push_back(glm::vec4(boundingBox.max.x, boundingBox.min.y, boundingBox.min.z, 1.0f));
   tri3.push_back(glm::vec4(boundingBox.min.x, boundingBox.min.y, boundingBox.max.z, 1.0f));

   std::vector<glm::vec4 > tri4;
   tri4.push_back(glm::vec4(boundingBox.max.x, boundingBox.max.y, boundingBox.max.z, 1.0f));
   tri4.push_back(glm::vec4(boundingBox.max.x, boundingBox.max.y, boundingBox.min.z, 1.0f));
   tri4.push_back(glm::vec4(boundingBox.min.x, boundingBox.max.y, boundingBox.max.z, 1.0f));


   MatrixStack adjustmentMatrix;

   // Create the Transformation matrix that will correspond to the model
   adjustmentMatrix.translate(position_o);
   adjustmentMatrix.rotate(rotation_o.z, glm::vec3(0.0f, 0.0f, 1.0f));
   adjustmentMatrix.rotate(rotation_o.y, glm::vec3(0.0f, 1.0f, 0.0f));
   adjustmentMatrix.rotate(rotation_o.x, glm::vec3(1.0f, 0.0f, 0.0f));
   adjustmentMatrix.scale(scale_o);

   // Adjust the vertices
   for (int i = 0; i < 3; i++)
   {
      tri1.at(i) = adjustmentMatrix.topMatrix() * tri1.at(i);
      tri2.at(i) = adjustmentMatrix.topMatrix() * tri2.at(i);
      

   }
   tri3.at(0) = adjustmentMatrix.topMatrix() * tri3.at(0);
   tri4.at(0) = adjustmentMatrix.topMatrix() * tri4.at(0);

   // Now we find the new minimums and maximums
   float xmin, xmax, ymin, ymax, zmin, zmax;
   xmin = ymin = zmin = FLT_MAX;
   xmax = ymax = zmax = -FLT_MAX;

   for (int i = 0; i < 3; i++)
   {
      if ((tri1.at(i).x) > xmax) xmax = tri1.at(i).x;
      if ((tri1.at(i).x) < xmin) xmin = tri1.at(i).x;
      if ((tri1.at(i).y) > ymax) ymax = tri1.at(i).y;
      if ((tri1.at(i).y) < ymin) ymin = tri1.at(i).y;
      if ((tri1.at(i).z) > zmax) zmax = tri1.at(i).z;
      if ((tri1.at(i).z) < zmin) zmin = tri1.at(i).z;
   }

   for (int i = 0; i < 3; i++)
   {
      if ((tri2.at(i).x) > xmax) xmax = tri2.at(i).x;
      if ((tri2.at(i).x) < xmin) xmin = tri2.at(i).x;
      if ((tri2.at(i).y) > ymax) ymax = tri2.at(i).y;
      if ((tri2.at(i).y) < ymin) ymin = tri2.at(i).y;
      if ((tri2.at(i).z) > zmax) zmax = tri2.at(i).z;
      if ((tri2.at(i).z) < zmin) zmin = tri2.at(i).z;
   }


   if ((tri3.at(0).x) > xmax) xmax = tri3.at(0).x;
   if ((tri3.at(0).x) < xmin) xmin = tri3.at(0).x;
   if ((tri3.at(0).y) > ymax) ymax = tri3.at(0).y;
   if ((tri3.at(0).y) < ymin) ymin = tri3.at(0).y;
   if ((tri3.at(0).z) > zmax) zmax = tri3.at(0).z;
   if ((tri3.at(0).z) < zmin) zmin = tri3.at(0).z;
 
   if ((tri4.at(0).x) > xmax) xmax = tri4.at(0).x;
   if ((tri4.at(0).x) < xmin) xmin = tri4.at(0).x;
   if ((tri4.at(0).y) > ymax) ymax = tri4.at(0).y;
   if ((tri4.at(0).y) < ymin) ymin = tri4.at(0).y;
   if ((tri4.at(0).z) > zmax) zmax = tri4.at(0).z;
   if ((tri4.at(0).z) < zmin) zmin = tri4.at(0).z;


   out.min.x = xmin;
   out.max.x = xmax;
   out.min.y = ymin;
   out.max.y = ymax;
   out.min.z = zmin;
   out.max.z = zmax;
   
   tempBBox =  out;
}


BoundingBox GameObject::getBBox()
{
   return tempBBox;
}

std::vector<float> poses;
std::vector<unsigned int> inds;
unsigned pbID(0);
unsigned eleBufID(0);

void GameObject::bindBoundingBox()
{

   BoundingBox toDraw = this->getBBox();
   poses = generateBBoxPoints(&toDraw);
   inds = generateBBoxIndices();

   std::vector<float> posBuffer = poses;
   std::vector<unsigned int> eleBuf = inds;

   // Send the vertex array to GPU
   glGenBuffers(1, &pbID);
   glBindBuffer(GL_ARRAY_BUFFER, pbID);
   glBufferData(GL_ARRAY_BUFFER, posBuffer.size()*sizeof(float), &posBuffer[0], GL_STATIC_DRAW);

   // Send the element array to the GPU
   glGenBuffers(1, &eleBufID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);

   // Unbind the arrays
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   assert(glGetError() == GL_NO_ERROR);
}

void GameObject::drawBoundingBox(MatrixStack MV, Program *prog)
{
   bindBoundingBox();

   // Now bind and draw
   glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));

   GLSL::enableVertexAttribArray(prog->getAttribute("vertPos"));
   glBindBuffer(GL_ARRAY_BUFFER, pbID);
   glVertexAttribPointer(prog->getAttribute("vertPos"), 3, GL_FLOAT, GL_FALSE, 0, 0);

 
  
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

   // Draw
   int nIndices = inds.size();
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
   // Unbind the arrays
   GLSL::disableVertexAttribArray(prog->getAttribute("vertPos"));
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // Check for errors
   assert(glGetError() == GL_NO_ERROR);
}

void GameObject::generateBBox()
{
   float xmin, xmax, ymin, ymax, zmin, zmax, max_radius;
   xmin = ymin = zmin = FLT_MAX;
   xmax = ymax = zmax = -FLT_MAX;

   for (int i = 0; i < shapes[0].mesh.positions.size(); i += 3)
   {
      glm::vec4 pos =  glm::vec4(shapes[0].mesh.positions[i + 0], 
                                 shapes[0].mesh.positions[i + 1],
                                 shapes[0].mesh.positions[i + 2], 0);


      if ((pos.x) > xmax) xmax = pos.x;
      if ((pos.x) < xmin) xmin = pos.x;
      if ((pos.y) > ymax) ymax = pos.y;
      if ((pos.y) < ymin) ymin = pos.y;
      if ((pos.z) > zmax) zmax = pos.z;
      if ((pos.z) < zmin) zmin = pos.z;

      
   }

   max_radius = fabs((xmax - xmin)/2);
   if (max_radius <  fabs((ymax - ymin) / 2))
      max_radius = fabs((ymax - ymin)) / 2;
   if (max_radius <  fabs((zmax - zmin)) / 2)
      max_radius = fabs((zmax - zmin)) / 2;

   BoundingBox out;
   out.min.x = xmin;
   out.max.x = xmax;
   out.min.y = ymin;
   out.max.y = ymax;
   out.min.z = zmin;
   out.max.z = zmax;
   out.max_radius = max_radius;
   out.position = position_o;
   out.velocity = velocity_o;
   out.scale = scale_o;
   boundingBox = out;
}


std::vector<float> GameObject::generateBBoxPoints(BoundingBox *bb)
{
   // Bot verts
   
   std::vector<float> points;
   points.push_back(bb->min.x);
   points.push_back(bb->min.y);
   points.push_back(bb->min.z);

   points.push_back(bb->max.x);
   points.push_back(bb->min.y);
   points.push_back(bb->min.z);

   points.push_back(bb->max.x);
   points.push_back(bb->min.y);
   points.push_back(bb->max.z);

   points.push_back(bb->min.x);
   points.push_back(bb->min.y);
   points.push_back(bb->max.z);

   //Top verts
   points.push_back(bb->min.x);
   points.push_back(bb->max.y);
   points.push_back(bb->min.z);

   points.push_back(bb->max.x);
   points.push_back(bb->max.y);
   points.push_back(bb->min.z);

   points.push_back(bb->max.x);
   points.push_back(bb->max.y);
   points.push_back(bb->max.z);

   points.push_back(bb->min.x);
   points.push_back(bb->max.y);
   points.push_back(bb->max.z);
  
   return points;
}

std::vector<unsigned int> GameObject::generateBBoxIndices()
{
   std::vector<unsigned int> indices;
   // Tri 1
   indices.push_back(0);
   indices.push_back(1);
   indices.push_back(3);

   // Tri 2
   indices.push_back(2);
   indices.push_back(3);
   indices.push_back(1);

   // Tri 3
   indices.push_back(1);
   indices.push_back(5);
   indices.push_back(2);

   // Tri 4
   indices.push_back(6);
   indices.push_back(2);
   indices.push_back(5);

   // Tri 5
   indices.push_back(4);
   indices.push_back(0);
   indices.push_back(7);

   // Tri 6
   indices.push_back(3);
   indices.push_back(7);
   indices.push_back(0);

   // Tri 7
   indices.push_back(3);
   indices.push_back(2);
   indices.push_back(7);

   // Tri 8
   indices.push_back(6);
   indices.push_back(7);
   indices.push_back(2);

   // Tri 9
   indices.push_back(4);
   indices.push_back(5);
   indices.push_back(0);

   // Tri 10
   indices.push_back(1);
   indices.push_back(0);
   indices.push_back(5);

   // Tri 11
   indices.push_back(7);
   indices.push_back(6);
   indices.push_back(4);

   // Tri 12
   indices.push_back(5);
   indices.push_back(4);
   indices.push_back(6);

   return indices;
}

void GameObject::animate(int frame, int animation)
{
   ShapeObj::animate(frame, animation);
   if (hitBoxes.empty() == false)
   {
      hitBoxes.animate(frame, animation);
   }
}


void GameObject::loadHitBox(const std::string &meshName)
{
   hitBoxes.load(meshName);
   hitBoxes.bindToObject(this);
   hitBoxes.init();
}

HitBoxes *GameObject::getHitBox()
{
   return &hitBoxes;
}