#pragma  once
#ifndef __WorldBuilder__
#define __WorldBuilder__

#define GLM_FORCE_RADIANS

#include <vector>
#include "GameObject.h"
#include "WallObject.h"
#include "Texture.h"
#include <fstream>
class WorldBuilder {
public:
    WorldBuilder();
    virtual ~WorldBuilder();

    void ImportTexture();
    void ApplyTextureFromLib();

    void ProcessKeyPresses(bool *keyToggled, bool *keyPressed, float dt);
    
    void ImportModelFile();
    void CreateModelFromLib();

    void DuplicateCurObject();
    void AddSelectedToCollideable();
    void RemoveSelectedFromCollideable();
    void SelectNextObject();
    void SelectPrevObject();
    void SelectNextFaceOfWall();
    void SelectWholeWall();
    void DeleteSelectedObject();
    void NormalizeSelectedObject();
    void ScaleSelected(glm::vec3 in);
    void RotateSelected(glm::vec3 in);
    void TranslateSelected(glm::vec3 in);
    void ScaleSelectedUV(glm::vec2 in);
    void SetSelectedUVScale();
    void SetSelectedNormalizedUVScale();
    void SetSelectedPosition();
    void SetSelectedRotation();
    void SetSelectedScale();
    void CreateWallObject();

    void saveWorld();
    void loadWorld();

    std::vector<GameObject *> worldObjects;
    std::vector<GameObject *> collideableObjects;

private:
    bool rotateMode;
    bool scaleMode;
    bool translateMode;
    bool precisionMode;
    bool scaleModeAxis;

    GameObject *curObject;
    Texture *no_texture;
    int curObjectIndex;

    void readInGameObject(GameObject *obj, std::ifstream *readFrom);

    void readInWallObject(WallObject *obj, std::ifstream *readFrom);
    
    std::vector<Texture *> textureList;
    std::vector<GameObject *> modelList;
    std::vector<unsigned int> worldObjectIndexesInModelList;
    std::vector<int> worldObjectIndexesInTextureList;
};

#endif