#include "WorldBuilder.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include "WallObject.h"
WorldBuilder::WorldBuilder()
{
    rotateMode = false;
    scaleMode = false;
    translateMode = false;

    curObject = NULL;
    curObjectIndex = -1;

    no_texture = new Texture;
    std::string f;
    f = "Materials/no_texture_set.tga";
    no_texture->setFilename(f);
    
}
WorldBuilder::~WorldBuilder()
{

}

void WorldBuilder::ImportTexture()
{
    Texture *newTexture = new Texture;
    std::string f;
    std::cout << "Please enter the file name of the texture\n";
    std::cin >> f;

    for(int i =0 ; i < textureList.size(); i++)
    {
        if(f.compare(textureList.at(i)->getName()) == 0)
        {
            std::cout << "This texture is already loaded\n" ;
            return;
        }
            
    }
    newTexture->setFilename(f);
    if (!newTexture->init())
    {
       std::cout << "there was an error loading the file: " << f << std::endl;
       return;
    }
    textureList.push_back(newTexture);

}
void WorldBuilder::ApplyTextureFromLib()
{
   if (textureList.size() == 0)
   {
      std::cout << "There are no textures loaded to apply" << std::endl;
      return;
   }

   if(curObject == NULL)
   {
      std::cout <<"There was no object selected to apply a texture to\n";
      return;
   }

   std::cout <<"Here are the files you can select from. Type the corresponding number to apply it\n";
   
   for(int i = 0; i < textureList.size(); i++)
   {
      std::cout << i << " " << textureList.at(i)->getName() << std::endl;
   }

   int index;

   std::cin >> index;
   
   // Update this to say where this models texture can be found in the texture library
   

   if(index >= textureList.size())
   {
      std::cout << "This is an incorrect value. Max texture index is " << textureList.size() - 1 << ". You entered " << index << std::endl;
      return;
   }
   worldObjectIndexesInTextureList.at(curObjectIndex) = index;
   WallObject *ptr = dynamic_cast <WallObject *> (curObject);
   if (ptr != 0)
   {
      if(ptr->curSelectedFace != -1)
         ptr->textureIndexes[ptr->curSelectedFace] = index;
      else
      {
         for (int i = 0; i < 6;i++)
         {
            ptr->textureIndexes[i] = index;
         }
      }
   }


   curObject->setDiffuseTexture(textureList.at(index));
   

}

void WorldBuilder::ProcessKeyPresses(bool *keyToggled, bool *keyPressed, float dt)
{
   if(keyToggled['1'])
   {
      if(translateMode == false)
         std::cout<< "Translate Mode Activated" << std::endl;
      translateMode = true;
      
   }
   else if(keyToggled['2'])
   {
      if (rotateMode == false)
         std::cout << "Rotate Mode Activated" << std::endl;
      rotateMode = true;
   } 
   else if(keyToggled['3'])
   {
      if(scaleMode == false)
         std::cout << "Scale Mode Activated" << std::endl;
      scaleMode = true;
   }
   else if (keyToggled['5'])
   {
      if (scaleModeAxis == false)
         std::cout << "Scale Along Axis Mode Activated" << std::endl;
      scaleModeAxis = true;
   }

   if(translateMode && !keyToggled['1']) {
      std::cout<< "Translate Mode Deactivated" << std::endl;
      translateMode = false;
   }

   if (rotateMode && !keyToggled['2']) {
      std::cout << "Rotate Mode Deactivated" << std::endl;
      rotateMode = false;
   }
   if (scaleMode && !keyToggled['3']) {
      std::cout << "Scale Mode Deactivated" << std::endl;
      scaleMode = false;
   }
   if (scaleModeAxis && !keyToggled['5']) {
      std::cout << "Scale Along Axis Mode Deactivated" << std::endl;
      scaleModeAxis = false;
   }



   if(keyToggled['4']) {
      if(precisionMode == false)
         std::cout<< "Precision Mode Activated" << std::endl;
      precisionMode = true;
   }

   if(precisionMode && !keyToggled['4']) {
      std::cout<< "Precision Mode Deactivated" << std::endl;
      precisionMode = false;
   }




   if(translateMode)
   {
      glm::vec3 toTranslate = glm::vec3(1.0f, 1.0f, 1.0f) * dt * 3.0f;
      if(precisionMode)
         toTranslate = toTranslate * 0.05f * 0.33f;
      if(keyPressed['t'])
         TranslateSelected(glm::vec3(0.0f, toTranslate.y, 0.0f));
      if(keyPressed['g'])
         TranslateSelected(glm::vec3(0.0f, -toTranslate.y, 0.0f));
      if(keyPressed['f'])
         TranslateSelected(glm::vec3(toTranslate.x, 0.0f, 0.0f));
      if(keyPressed['h'])
         TranslateSelected(glm::vec3(-toTranslate.x, 0.0f, 0.0f));
      if(keyPressed['r'])
         TranslateSelected(glm::vec3(0.0f, 0.0f, toTranslate.z));
      if(keyPressed['y'])
         TranslateSelected(glm::vec3(0.0f, 0.0f, -toTranslate.z));
   }
   else if (scaleMode)
   {
      glm::vec3 toScale = glm::vec3(0.3f, 0.3f, 0.3f) * dt;
      if (precisionMode)
         toScale *= 0.5f;
      if (keyPressed['t'])
         ScaleSelected(glm::vec3(1 - toScale.x, 1 - toScale.y, 1 - toScale.z));
      if (keyPressed['g'])
         ScaleSelected(glm::vec3(1 + toScale.x, 1 + toScale.y, 1 + toScale.z));
   }
   else if (rotateMode)
   {
      glm::vec3 toRotate = glm::vec3(0.0314159265359f, 0.0314159265359f, 0.0314159265359f);
      toRotate *= 2;
      if (precisionMode)
         toRotate = toRotate * 0.05f;
      if (keyPressed['t'])
         RotateSelected(glm::vec3(0.0f, toRotate.y, 0.0f));
      if (keyPressed['g'])
         RotateSelected(glm::vec3(0.0f, -toRotate.y, 0.0f));
      if (keyPressed['f'])
         RotateSelected(glm::vec3(toRotate.x, 0.0f, 0.0f));
      if (keyPressed['h'])
         RotateSelected(glm::vec3(-toRotate.x, 0.0f, 0.0f));
      if (keyPressed['r'])
         RotateSelected(glm::vec3(0.0f, 0.0f, toRotate.z));
      if (keyPressed['y'])
         RotateSelected(glm::vec3(0.0f, 0.0f, -toRotate.z));
   }
   else if (scaleModeAxis)
   {
      glm::vec3 toScale = glm::vec3(0.3f, 0.3f, 0.3f) * dt;
      if (precisionMode)
         toScale *= 0.5f;
      if (keyPressed['t'])
         ScaleSelected(glm::vec3(1.0f, 1 - toScale.y, 1.0f));
      if (keyPressed['g'])
         ScaleSelected(glm::vec3(1.0f, 1 + toScale.y, 1.0f));
      if (keyPressed['f'])
         ScaleSelected(glm::vec3(1 - toScale.x, 1.0f, 1.0f));
      if (keyPressed['h'])
         ScaleSelected(glm::vec3(1 + toScale.x, 1.0f, 1.0f));
      if (keyPressed['r'])
         ScaleSelected(glm::vec3(1.0f, 1.0f, 1 - toScale.z));
      if (keyPressed['y'])
         ScaleSelected(glm::vec3(1.0f, 1.0f, 1 + toScale.z));
   }
}

void WorldBuilder::ImportModelFile()
{
    GameObject *newModel = new GameObject;
    std::string f;
    std::cout << "Please enter the file name of the model\n";
    std::cin >> f;

    for(int i =0 ; i < modelList.size(); i++)
    {
        if(f.compare(modelList.at(i)->name) == 0)
        {
            std::cout << "This model is already loaded\n" ;
            return;
        }
            
    }
    if (!newModel->load(f))
    { 
       std::cout << "there was an error loading the file: " << f << std::endl;
       return;
    }
    newModel->init();
    modelList.push_back(newModel);
}

void WorldBuilder::CreateModelFromLib()
{
   if (modelList.size() == 0)
   {
      std::cout << "There are no models loaded to create" << std::endl;
      return;
   }

   std::cout <<"Here are the files you can select from. Type the corresponding number to apply it\n";
   for(int i = 0; i < modelList.size(); i++)
   {
      std::cout << i << " " << modelList.at(i)->name << std::endl;
   }
   int index;

   std::cin >> index;

   // Update this to say which model this object uses in the model library
   worldObjectIndexesInModelList.push_back(index);
   // Update this to say that this object has no texture right now
   worldObjectIndexesInTextureList.push_back(-1);

   if(index >= modelList.size())
   {
      std::cout << "This is an incorrect value. Max texture index is " << modelList.size() - 1 << ". You entered " << index << std::endl;
      return;
   }

   if(curObjectIndex == -1)
      curObjectIndex = 0;

   GameObject *toCreate = new GameObject(modelList.at(index));
   toCreate->init();

   if (!no_texture->init())
      std::cout << "there was an error loading the file: " << "/Materials/no_texture_set.png" << std::endl;

   toCreate->setDiffuseTexture(no_texture);
   worldObjects.push_back(toCreate);
   if(curObject != NULL)
      curObject->setHighlight(false);

   
   curObject = worldObjects.at(worldObjects.size() - 1);
   curObjectIndex = worldObjects.size() - 1;
   curObject->setHighlight(true);
}

void WorldBuilder::DuplicateCurObject()
{
   if(curObject == NULL)
   {
      std::cout << "There was no selected Object to duplicate\n";
      return;
   }

   GameObject *toDup;
   if (curObject->type.compare("WallObject") == 0)
   {
      WallObject *ptr = dynamic_cast <WallObject *> (curObject);
      toDup = new WallObject(ptr);
   }
   else
   {
      toDup = new GameObject(curObject);
   }
   toDup->init();
   
   worldObjects.push_back(toDup);
   worldObjectIndexesInModelList.push_back(worldObjectIndexesInModelList.at(curObjectIndex));

   worldObjectIndexesInTextureList.push_back(worldObjectIndexesInTextureList.at(curObjectIndex));

   curObject->setHighlight(false);
   curObject = worldObjects.at(worldObjects.size() - 1);
   curObjectIndex = worldObjects.size() - 1;
   curObject->setHighlight(true);
   

}

void WorldBuilder::CreateWallObject()
{
   if (!no_texture->init())
   {
      std::cout << "there was an error loading the file: " << "/Materials/no_texture_set.png" << std::endl;
      //  return;
   }
   WallObject *ptr = new WallObject();
   ptr->load();
   ptr->init();
   ptr->setDiffuseTexture(no_texture);
   ptr->setTextureIndexes(-1);
   worldObjects.push_back(ptr);
   worldObjectIndexesInModelList.push_back(-1);

   worldObjectIndexesInTextureList.push_back(-1);
   if(curObject != NULL)
      curObject->setHighlight(false);
   curObject = worldObjects.at(worldObjects.size() - 1);
   curObjectIndex = worldObjects.size() - 1;
   curObject->setHighlight(true);
}

void WorldBuilder::AddSelectedToCollideable()
{

}
void WorldBuilder::RemoveSelectedFromCollideable()
{
   
}
void WorldBuilder::SelectNextObject()
{
    if(worldObjects.size() == 0)
    {
        std::cout << "there are no objects to select\n";
    }
    curObjectIndex++;
    if(curObjectIndex >= worldObjects.size())
    {
        curObjectIndex = 0;
    }
    curObject->setHighlight(false);
    curObject = worldObjects.at(curObjectIndex);
    curObject->setHighlight(true);
}
void WorldBuilder::SelectPrevObject()
{
    if(worldObjects.size() == 0)
    {
        std::cout << "there are no objects to select\n";
    }
    curObjectIndex--;
    if(curObjectIndex < 0)
    {
        curObjectIndex = worldObjects.size() - 1;
    }
    curObject->setHighlight(false);
    curObject = worldObjects.at(curObjectIndex);
    curObject->setHighlight(true);
}
void WorldBuilder::DeleteSelectedObject()
{
   if(curObject == NULL)
   {
      std::cout << "No object selected to delete\n";
      return;
   }

   worldObjects.erase(worldObjects.begin() + curObjectIndex);
   worldObjectIndexesInModelList.erase(worldObjectIndexesInModelList.begin() + curObjectIndex);
   worldObjectIndexesInTextureList.erase(worldObjectIndexesInTextureList.begin() + curObjectIndex);

   curObjectIndex--;
   if (curObjectIndex < 0)
      curObject = NULL;
   else
      curObject->setHighlight(true);
}

void WorldBuilder::NormalizeSelectedObject()
{
   if (curObject == NULL)
   {
      std::cout << "No object selected to Normalize\n";
      return;
   }

   curObject->normalizeObject();
}

void WorldBuilder::ScaleSelected(glm::vec3 in)
{
   if(curObject == NULL)
   {
      std::cout << "No object selected to Scale\n";
      return;
   }

   curObject->scale(in);
}
void WorldBuilder::RotateSelected(glm::vec3 in)
{
   if(curObject == NULL)
   {
      std::cout << "No object selected to rotate\n";
      return;
   }

   curObject->rotate(in);
}
void WorldBuilder::TranslateSelected(glm::vec3 in)
{
   if(curObject == NULL)
   {
      std::cout << "No object selected to translate\n";
      return;
   }

   curObject->translate(in);
}

void WorldBuilder::ScaleSelectedUV(glm::vec2 in)
{

}

void WorldBuilder::SelectNextFaceOfWall()
{
   WallObject *ptr = dynamic_cast <WallObject *> (curObject);
   if (ptr == 0)
   {
      std::cout << "This is not a Wall Object" << std::endl;
      return;
   }

   ptr->curSelectedFace++;
   if (ptr->curSelectedFace == 6)
   {
      ptr->curSelectedFace = -1;
   }
   ptr->setHighlight(true);
}

void WorldBuilder::SelectWholeWall()
{
   WallObject *ptr = dynamic_cast <WallObject *> (curObject);
   if (ptr == 0)
   {
      std::cout << "This is not a Wall Object" << std::endl;
      return;
   }

   ptr->curSelectedFace = -1;
   ptr->setHighlight(true);
}


void WorldBuilder::SetSelectedUVScale()
{
   if (curObject == NULL)
   {
      std::cout << "No object selected to set its UV Scale\n";
      return;
   }

   std::cout << "Please enter the x and y scale of the UV\n";
   float inx, iny;
   std::cin >> inx;
   std::cin >> iny;

   curObject->setTextureScale(glm::vec2(inx, iny));
}

void WorldBuilder::SetSelectedNormalizedUVScale()
{
   if (curObject == NULL)
   {
      std::cout << "No object selected to set its UV Scale\n";
      return;
   }

   std::cout << "Please enter the x and y scale of the UV\n";
   float inx, iny;
   std::cin >> inx;
   std::cin >> iny;

   curObject->setScaleNormalizedUV(glm::vec2(inx, iny));
}

void WorldBuilder::SetSelectedPosition()
{
   if (curObject == NULL)
   {
      std::cout << "No object selected to set its Position\n";
      return;
   }
   std::cout << "The current translation of this object is: " << 
      curObject->getPosition().x << " " << curObject->getPosition().y << " " << curObject->getPosition().z << std::endl;
   std::cout << "Please enter the new x y and z position \n";
   float inx, iny, inz;
   std::cin >> inx;
   std::cin >> iny;
   std::cin >> inz;
   curObject->setPosition(glm::vec3(inx, iny, inz));
}

void WorldBuilder::SetSelectedRotation()
{
   if (curObject == NULL)
   {
      std::cout << "No object selected to set its Rotation\n";
      return;
   }
   std::cout << "The current rotation of this object is: " <<
      (curObject->getRotation().x * 180.0f / 3.141592f) << " " << (curObject->getRotation().y * 180.0f / 3.141592f) << " " << (curObject->getRotation().z * 180.0f / 3.141592f) << std::endl;
   std::cout << "Please enter the new x y and z position (in degrees) \n";
   float inx, iny, inz;
   std::cin >> inx;
   std::cin >> iny;
   std::cin >> inz;
   curObject->setRotation(glm::vec3(inx * 3.141592f / 180.0f, iny * 3.141592f / 180.0f, inz * 3.141592f / 180.0f));
}
void WorldBuilder::SetSelectedScale()
{
   if (curObject == NULL)
   {
      std::cout << "No object selected to set its Scale\n";
      return;
   }
   std::cout << "The current scale of this object is: " <<
      curObject->getScale().x << " " << curObject->getScale().y << " " << curObject->getScale().z << std::endl;
   std::cout << "Please enter the new x y and z scale \n";
   float inx, iny, inz;
   std::cin >> inx;
   std::cin >> iny;
   std::cin >> inz;
   curObject->setScale(glm::vec3(inx, iny, inz));
}


void WorldBuilder::saveWorld()
{
   std::ofstream myFile;
   std::string writeTo;

   std::cout << "Enter the filename you want to save to\n";
   std::cin >> writeTo;
   myFile.open(writeTo);

   myFile << "textures\n";
   for (int i = 0; i < textureList.size(); i++)
   {
      myFile << textureList.at(i)->getName() << std::endl;
   }

   myFile << "models\n";
   for (int i = 0; i < modelList.size(); i++)
   {
      myFile << modelList.at(i)->name << std::endl;
   }

   myFile << "WorldObjects";

   
   for (int i = 0; i < worldObjects.size(); i++)
   {
      if (worldObjects.at(i)->type.compare("WallObject") == 0)
      {
         // Format of Wall entry WallObject [pos xyz] [rot xyz] [scale xyz] ([texture index] [uvscale uv]) x 6
         WallObject *ptr = dynamic_cast <WallObject *> (worldObjects.at(i));
         myFile << std::endl << "WallObject" << " "
            << ptr->getPosition().x << " " << worldObjects.at(i)->getPosition().y << " " << worldObjects.at(i)->getPosition().z << " "
            << ptr->getRotation().x << " " << worldObjects.at(i)->getRotation().y << " " << worldObjects.at(i)->getRotation().z << " "
            << ptr->getScale().x << " " << worldObjects.at(i)->getScale().y << " " << worldObjects.at(i)->getScale().z << " ";
            
            for (int j = 0; j < 6; j++)
            {
               myFile << ptr->textureIndexes[j]  << " " << ptr->getFace(j)->getTextureScale().x << " " << ptr->getFace(j)->getTextureScale().y << " ";
            }           
      }
      else
      {
         // Format of object entry [modelLibIndex] [textureLibIndex] [pos xyz] [rot xyz] [scale xyz] [uvscale uv]
         myFile << std::endl << worldObjectIndexesInModelList.at(i) << " "
            << worldObjectIndexesInTextureList.at(i) << " "
            << worldObjects.at(i)->getPosition().x << " " << worldObjects.at(i)->getPosition().y << " " << worldObjects.at(i)->getPosition().z << " "
            << worldObjects.at(i)->getRotation().x << " " << worldObjects.at(i)->getRotation().y << " " << worldObjects.at(i)->getRotation().z << " "
            << worldObjects.at(i)->getScale().x << " " << worldObjects.at(i)->getScale().y << " " << worldObjects.at(i)->getScale().z << " "
            << worldObjects.at(i)->getTextureScale().x << " " << worldObjects.at(i)->getTextureScale().y;
      }
   }

   myFile.close();
}

void WorldBuilder::readInWallObject(WallObject *newModel, std::ifstream *readFrom)
{
   std::string buffer;
   worldObjectIndexesInTextureList.push_back(-1);

   glm::vec3 pos;
   *readFrom >> buffer;
   pos.x = std::stof(buffer);
   *readFrom >> buffer;
   pos.y = std::stof(buffer);
   *readFrom >> buffer;
   pos.z = std::stof(buffer);
   newModel->setPosition(pos);


   glm::vec3 rot;
   *readFrom >> buffer;
   rot.x = std::stof(buffer);
   *readFrom >> buffer;
   rot.y = std::stof(buffer);
   *readFrom >> buffer;
   rot.z = std::stof(buffer);
   newModel->setRotation(rot);


   glm::vec3 scl;
   *readFrom >> buffer;
   scl.x = std::stof(buffer);
   *readFrom >> buffer;
   scl.y = std::stof(buffer);
   *readFrom >> buffer;
   scl.z = std::stof(buffer);
   newModel->setScale(scl);

   
   for (int i = 0; i < 6; i++)
   {
      
      *readFrom >> buffer;
      if (stoi(buffer) != -1)
         newModel->getFace(i)->setDiffuseTexture(textureList.at(stoi(buffer)));
      else
         newModel->getFace(i)->setDiffuseTexture(no_texture);
     // std::cout << buffer;
      newModel->textureIndexes[i] = stoi(buffer);

      glm::vec2 uvScale;
      *readFrom >> buffer;
      //std::cout << buffer;
      uvScale.x = std::stof(buffer);
      *readFrom >> buffer;
      uvScale.y = std::stof(buffer);
    //  std::cout << " " << uvScale.x << " " << uvScale.y << std::endl;
      newModel->getFace(i)->setTextureScale(uvScale);
      
   }
}

void WorldBuilder::readInGameObject(GameObject *newModel, std::ifstream *readFrom)
{
   
   std::string buffer;
   *readFrom >> buffer;
   int textureIndex = std::stoi(buffer);
   worldObjectIndexesInTextureList.push_back(textureIndex);

   if (textureIndex != -1)
   {
      newModel->setDiffuseTexture(textureList.at(textureIndex));
   }
   else
      newModel->setDiffuseTexture(no_texture);

   glm::vec3 pos;
   *readFrom >> buffer;
   pos.x = std::stof(buffer);
   *readFrom >> buffer;
   pos.y = std::stof(buffer);
   *readFrom >> buffer;
   pos.z = std::stof(buffer);
   newModel->setPosition(pos);


   glm::vec3 rot;
   *readFrom >> buffer;
   rot.x = std::stof(buffer);
   *readFrom >> buffer;
   rot.y = std::stof(buffer);
   *readFrom >> buffer;
   rot.z = std::stof(buffer);
   newModel->setRotation(rot);


   glm::vec3 scl;
   *readFrom >> buffer;
   scl.x = std::stof(buffer);
   *readFrom >> buffer;
   scl.y = std::stof(buffer);
   *readFrom >> buffer;
   scl.z = std::stof(buffer);
   newModel->setScale(scl);

   glm::vec2 uvScale;
   *readFrom >> buffer;
   uvScale.x = std::stof(buffer);
   *readFrom >> buffer;
   uvScale.y = std::stof(buffer);
   newModel->setTextureScale(uvScale);

}


void WorldBuilder::loadWorld()
{
   worldObjects.clear();
   modelList.clear();
   textureList.clear();
   worldObjectIndexesInModelList.clear();
   worldObjectIndexesInTextureList.clear();

   std::string fileName;

   std::cout << "Enter the filename you want to load in\n";
   std::cin >> fileName;
   std::ifstream readFrom;

   readFrom.open(fileName);
   if (readFrom.is_open()) {
      
      std::string buffer;
      // Reads in "textures
      readFrom >> buffer;
      // Reads in first entry
      readFrom >> buffer;
      while (buffer.compare("models") != 0)
      {
         Texture *newTexture = new Texture;
         newTexture->setFilename(buffer);
         newTexture->init();
         textureList.push_back(newTexture);
         // Will read in either the next entry or "models" 
         readFrom >> buffer;
      }
      readFrom >> buffer;
      while (buffer.compare("WorldObjects") != 0)
      {

         GameObject *newModel = new GameObject;
         newModel->load(buffer);
         newModel->init();
         modelList.push_back(newModel);
         readFrom >> buffer;
      }

      readFrom >> buffer;
      while (!readFrom.eof() || buffer.size() == 0)
      {
         
         std::string type = buffer;
         GameObject *newModel;
         
         if (type.compare("WallObject") == 0)
         {
            newModel = new WallObject();
            WallObject *ptr = dynamic_cast <WallObject *> (newModel);
            newModel->load();
            newModel->init();
            worldObjectIndexesInModelList.push_back(-1);
            readInWallObject(ptr, &readFrom);
         }
         else
         {
            int mdlIndex = std::stoi(type);
            newModel = new GameObject(modelList.at(mdlIndex));
            newModel->init();
            worldObjectIndexesInModelList.push_back(mdlIndex);
            readInGameObject(newModel, &readFrom);
         }
         
         worldObjects.push_back(newModel);
         
         curObject = newModel;
         curObjectIndex = worldObjects.size() - 1;

         if(readFrom.eof() == false)
            readFrom >> buffer;
      }
   }
   if(curObject != NULL)
      curObject->setHighlight(true);
   readFrom.close();
}