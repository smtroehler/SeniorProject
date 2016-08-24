
#define  WINDOWS false

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Windows.h>
#undef WINDOWS
#define  WINDOWS true
#include <GL/glew.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include  <vector>
#include <memory>
#include "GLSL.h"
#include "Program.h"
#include "Camera.h"
#include "MatrixStack.h"
#include "ShapeObj.h"
#include "PlayerController.h"
#include "Texture.h"
#include "HitBoxes.h"
#include "WorldBuilder.h"
#include "WallObject.h"

#include "SpatialIndexing.h"
#include "GameObject.h"
#include "ShadowMap.h"
#define GLM_FORCE_RADIANS
using namespace std;

bool keyToggles[256] = {false};
bool keyPresses[256] = {false};
int width = 1;
int height = 1;

Program prog;
Program simpleShader;
Program simpleAnimatedShader;
Program texturedAnimatedShader;
Program texturedShader;
Camera camera;
int frame = 0;
int time0 = 0; 


glm::vec3 cameraLoc;
GameObject* trainModel;
GameObject* scoutModel;
vector<std::shared_ptr<GameObject> > objectList;

PlayerController player;
Texture atexture;
QuadTreeLevel *baseQuadTree;
WorldBuilder theWorld;

WallObject testWall;

GameObject *playerModel;

bool toggleBoundingBoxes = false;

ShadowMap shadow_mapper;

#define TEST_OBJS 200

void loadScene()
{

   XZBound toTree;
   toTree.center = glm::vec2(0.0, 0.0);
   toTree.w = 4000;
   toTree.h = 4000;
   baseQuadTree = new QuadTreeLevel(0, toTree);

   
   testWall.load();

   keyToggles['c'] = true;
   prog.setShaderNames("Shaders/phong_animated_vert.glsl", "Shaders/phong_animated_frag.glsl");
   simpleShader.setShaderNames("Shaders/simple_vert.glsl", "Shaders/simple_frag.glsl");
   simpleAnimatedShader.setShaderNames("Shaders/simple_animated_vert.glsl", "Shaders/simple_animated_frag.glsl");
   texturedAnimatedShader.setShaderNames("Shaders/textured_animated_vert.glsl", "Shaders/textured_animated_frag.glsl");
   texturedShader.setShaderNames("Shaders/textured_vert.glsl", "Shaders/textured_frag.glsl");
   //shadow_mapper.setup();
   atexture.setFilename("Materials/DiffuseTextures/gragoyle_stone.tga");
   cameraLoc = glm::vec3(0.0f, 0.0f, 0.0f);
   camera.EnableFixedCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));

   scoutModel = new GameObject;
   scoutModel->load("Models/wizard.smd");
   scoutModel->loadDiffuseTexture("Materials/DiffuseTextures/train_small_diesel_neutral.tga");
   scoutModel->loadHitBox("Models/wizard_bounding_box.smd");

   scoutModel->loadSmdAnim("Animations/wizard_walk.smd");
   scoutModel->AnimationToMatrix(1);
   scoutModel->normalizeObject();
   scoutModel->scale(glm::vec3(1.7, 1.7, 1.7));
   scoutModel->translate(glm::vec3(-5, 0.0, 0.0));
   objectList.push_back(shared_ptr<GameObject>(scoutModel));

   playerModel = new GameObject;
   playerModel->load("Models/robot_player.smd");
   playerModel->loadDiffuseTexture("Materials/DiffuseTextures/robot_texture.tga");
   playerModel->normalizeObject();
   playerModel->scale(glm::vec3(1.7, 1.7, 1.7));
   playerModel->loadSmdAnim("Animations/robot_anims/running.smd");
   playerModel->loadSmdAnim("Animations/robot_anims/test_1Action.smd");
   playerModel->loadSmdAnim("Animations/robot_anims/fallAction.smd");
   playerModel->loadSmdAnim("Animations/robot_anims/fall_end_charging.smd");

}
   


void initGL()
{
   //////////////////////////////////////////////////////
   // Initialize GL for the whole scene
   //////////////////////////////////////////////////////	
   
   // Set background color
   glClearColor(0.05f, 0.05f, 0.09f, 1.0f);
   // Enable z-buffer test
   glEnable(GL_DEPTH_TEST);

   // Accept fragment if it closer to the camera than the former one
   glDepthFunc(GL_LESS);

   // Cull triangles which normal is not towards the camera
   glEnable(GL_CULL_FACE);
   //////////////////////////////////////////////////////
   // Intialize the shapes
   //////////////////////////////////////////////////////
   
  // trainModel->init();
  // scoutModel->init();
   testWall.init();
   atexture.init();
   for (int i = 0; i < objectList.size(); i++)
   {
      objectList.at(i).get()->init();
      
   }
   playerModel->init();
   //////////////////////////////////////////////////////
   // Intialize the shaders
   //////////////////////////////////////////////////////
   
   prog.init();
   prog.addUniform("P");
   prog.addUniform("MV");
   prog.addUniform("UVScale");

   prog.addAttribute("vertPos");
   prog.addAttribute("vertNor");
   prog.addAttribute("vertTex");
  
   prog.addUniform("ka");
   prog.addUniform("kd");
   prog.addUniform("ks");
   prog.addUniform("s");
      
   prog.addUniform("light");
   prog.addUniform("inten");
   prog.addUniform("texture0");

   prog.addAttribute("weights0");
   prog.addAttribute("weights1");
   prog.addAttribute("weights2");
   prog.addAttribute("weights3");

   prog.addAttribute("bones0");
   prog.addAttribute("bones1");
   prog.addAttribute("bones2");
   prog.addAttribute("bones3");

   prog.addAttribute("numWeights");
   prog.addUniform("animM");
   prog.addUniform("bindPose");

   prog.addUniform("highlighted");

   simpleShader.init();
   simpleShader.addAttribute("vertPos");
   simpleShader.addUniform("P");
   simpleShader.addUniform("MV");
   

   simpleAnimatedShader.init();
   simpleAnimatedShader.addUniform("P");
   simpleAnimatedShader.addUniform("MV");

   simpleAnimatedShader.addAttribute("vertPos");

   simpleAnimatedShader.addAttribute("weights0");
   simpleAnimatedShader.addAttribute("weights1");
   simpleAnimatedShader.addAttribute("weights2");
   simpleAnimatedShader.addAttribute("weights3");

   simpleAnimatedShader.addAttribute("bones0");
   simpleAnimatedShader.addAttribute("bones1");
   simpleAnimatedShader.addAttribute("bones2");
   simpleAnimatedShader.addAttribute("bones3");

   simpleAnimatedShader.addAttribute("numWeights");
   simpleAnimatedShader.addUniform("animM");
   simpleAnimatedShader.addUniform("bindPose");

   texturedAnimatedShader.init();
   texturedAnimatedShader.addUniform("P");
   texturedAnimatedShader.addUniform("MV");
   texturedAnimatedShader.addUniform("UVScale");

   texturedAnimatedShader.addAttribute("vertPos");
   texturedAnimatedShader.addAttribute("vertTex");

   texturedAnimatedShader.addAttribute("weights0");
   texturedAnimatedShader.addAttribute("weights1");
   texturedAnimatedShader.addAttribute("weights2");
   texturedAnimatedShader.addAttribute("weights3");

   texturedAnimatedShader.addUniform("texture0");

   texturedAnimatedShader.addAttribute("bones0");
   texturedAnimatedShader.addAttribute("bones1");
   texturedAnimatedShader.addAttribute("bones2");
   texturedAnimatedShader.addAttribute("bones3");

   texturedAnimatedShader.addAttribute("numWeights");
   texturedAnimatedShader.addUniform("animM");
   texturedAnimatedShader.addUniform("bindPose");

   texturedShader.init();
   texturedShader.addUniform("P");
   texturedShader.addUniform("MV");
   texturedShader.addUniform("UVScale");

   texturedShader.addAttribute("vertPos");
   texturedShader.addAttribute("vertTex");
   texturedShader.addUniform("texture0");
   texturedShader.addUniform("shadowMap");
   texturedShader.addUniform("highlighted");
   texturedShader.addUniform("depthBiasMVP");
   

   //shadow_mapper.setup();
   //////////////////////////////////////////////////////
   // Final check for errors
   //////////////////////////////////////////////////////
   GLSL::checkVersion();
   assert(glGetError() == GL_NO_ERROR);
}


void reshapeGL(int w, int h)
{
   // Set view size
   width = w;
   height = h;
   glViewport(0, 0, (GLsizei)w, (GLsizei)h);
   camera.setAspect((float)width/(float)height);
  

}
glm::vec3 ray = glm::vec3(0.0, .01, -10);
glm::vec3 origin = glm::vec3(0, 0, 10);

bool drawPla = true;


float pixels[1024 * 1024];
void drawGL()
{
   // Elapsed time
   float tCurr = 0.001f*glutGet(GLUT_ELAPSED_TIME); // in seconds

   
   // Clear buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   if(keyToggles['c']) {
      glEnable(GL_CULL_FACE);
   } else {
      glDisable(GL_CULL_FACE);
   }
   if(keyToggles['l']) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
   
   //////////////////////////////////////////////////////
   // Create matrix stacks
   //////////////////////////////////////////////////////
   
   MatrixStack P, MV;
  // camera.updateFixedCamera(cameraLoc, glm::vec2(0.0f,0.0f));
   // Apply camera transforms
   P.pushMatrix();
   camera.applyProjectionMatrix(&P);
   MV.pushMatrix();
   camera.applyViewMatrix(&MV);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0, 0, width, height);
   

   prog.bind();
   //shadow_mapper.bindShadowMap(&texturedShader);
   glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, glm::value_ptr(P.topMatrix()));
   for (int i = 0; i < theWorld.worldObjects.size(); i++)
   {
      theWorld.worldObjects.at(i)->draw(MV, &prog);
   }
   prog.unbind();
      
   

   //////////////////////////////////////////////////////
   // Now draw the shape using modern OpenGL
   //////////////////////////////////////////////////////
   // Bind the program
   prog.bind();
   
   
   glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, glm::value_ptr(P.topMatrix()));
   glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));

   glUniform3fv(prog.getUniform("ka"), 1, glm::value_ptr(glm::vec3(0.5, .5, 0.5)));
   glUniform3fv(prog.getUniform("kd"),  1, glm::value_ptr(glm::vec3(0.8,0.8,0.8)));
   glUniform3fv(prog.getUniform("ks"), 1, glm::value_ptr(glm::vec3(.9,.9,.9)));
   glUniform1f(prog.getUniform("s"), 50);

   glUniform1f(prog.getUniform("inten"), .8);
   glUniform3fv(prog.getUniform("light"), 1, glm::value_ptr(glm::vec3(2.0,50.0,2.0)));
   atexture.bind(prog.getUniform("texture0"), 0);
   
   for (int i = 0; i < objectList.size(); i++)
   {
      objectList.at(i).get()->draw(MV, &prog);
   }
   if(drawPla)
      playerModel->draw(MV, &prog);
 //  testWall.draw(MV, &prog);
   
   atexture.unbind();
   // Unbind the program
   prog.unbind();


   texturedAnimatedShader.bind();
   glUniformMatrix4fv(texturedAnimatedShader.getUniform("P"), 1, GL_FALSE, glm::value_ptr(P.topMatrix()));
   glUniformMatrix4fv(texturedAnimatedShader.getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
  // atexture.bind(texturedAnimatedShader.getUniform("texture0"), 0);

   for (int i = 0; i < objectList.size(); i++)
   {
      //std::cout << "in" << std::endl;
     // objectList.at(i).get()->draw(MV, &texturedAnimatedShader);
   }

   texturedAnimatedShader.unbind();
   
   simpleShader.bind();
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glUniformMatrix4fv(simpleShader.getUniform("P"), 1, GL_FALSE, glm::value_ptr(P.topMatrix()));

   if (toggleBoundingBoxes) {
      for (int i = 0; i < objectList.size(); i++)
      {
         objectList.at(i).get()->drawBoundingBox(MV, &prog);
      }
   }
   glDisable(GL_BLEND);
   simpleShader.unbind();

   

   //////////////////////////////////////////////////////
   // Draw hitboxes here with the Simple Animated Shader 
   //////////////////////////////////////////////////////
  
   simpleAnimatedShader.bind();
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glUniformMatrix4fv(simpleAnimatedShader.getUniform("P"), 1, GL_FALSE, glm::value_ptr(P.topMatrix()));
   scoutModel->getHitBox()->drawSimple(MV, &simpleAnimatedShader);
   
   glDisable(GL_BLEND);
   simpleAnimatedShader.unbind();


   //////////////////////////////////////////////////////
   // Draw the vector that shows intersection of Hitboxes
   //////////////////////////////////////////////////////
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadMatrixf(glm::value_ptr(P.topMatrix()));

   // Setup the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadMatrixf(glm::value_ptr(MV.topMatrix()));
   glLineWidth(2.5);
   glColor3f(1.0, 0.0, 0.0);
   glBegin(GL_LINES);
   glVertex3fv(glm::value_ptr(origin));
   glVertex3fv(glm::value_ptr(origin + (ray * 20.0f)));
   glEnd();

   glEnable(GL_PROGRAM_POINT_SIZE);
   glPointSize(10.0);
   glBegin(GL_POINTS);
   std::vector<glm::mat4> n = playerModel->getAnimationMatricesForCurFrame(1);
   for (int i = 0; i < n.size(); i++)
   {
      glVertex3fv(glm::value_ptr(glm::vec3(playerModel->getScale().x * (n.at(i) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))));
   }
   glEnd();
   

   glPopMatrix();
      
   // Pop projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   
   // Pop stacks
   MV.popMatrix();
   P.popMatrix();

   
   // Swap buffer
   glutSwapBuffers();
}


void mouseGL(int button, int state, int x_, int y_)
{
   int modifier = glutGetModifiers();
   bool shift = modifier & GLUT_ACTIVE_SHIFT;
   bool ctrl  = modifier & GLUT_ACTIVE_CTRL;
   bool alt   = modifier & GLUT_ACTIVE_ALT;

   if (button == 3)
   {
      camera.zdist += .05;
   }
   else if (button == 4)
   {
      camera.zdist -= .05;
   }
  // camera.mouseClicked(x_, y_, shift, ctrl, alt);
}

void passiveMotionGL(int x, int y)
{
   player.processMouseControl(x, y);
}

void mouseMotionGL(int x, int y)
{
   //camera.mouseMoved(x, y);
   
}

void keyboardGL(unsigned char key, int x, int y)
{
   keyToggles[key] = !keyToggles[key];
   keyPresses[key] = true;
   switch(key) {
      case 27:
         // ESCAPE
         exit(0);
         break;
      case 't':
         toggleBoundingBoxes = !toggleBoundingBoxes;
         break;
      case 'o':
         theWorld.SelectNextObject();
         break;
      case 'p':
         theWorld.SelectPrevObject();
         break;
      case 'k':
         theWorld.SelectNextFaceOfWall();
         break;
      case 'j':
         theWorld.SelectWholeWall();
         break;
      case 'f':
         origin += glm::vec3(0.0, .1, 0.0);
         break;
      case 'v':
         origin += glm::vec3(0.0, -.1, 0.0);
         break;
      case 'c':
         origin += glm::vec3(-.1, 0.0, 0.0);
         break;
      case 'b':
         origin += glm::vec3(.1, 0.0, 0.0);
         break;
      case '/':
         player.specModeActive = !player.specModeActive;
         break;
      case '0':
         drawPla = !drawPla;

   }
}

void keyboardUp(unsigned char key, int x, int y)
{
   keyPresses[key] = false;
}

void timerGL(int value)
{
   glutPostRedisplay();
   glutTimerFunc(20, timerGL, 0);
}
float time2 = 0;


void idleGL() {

   float time1 = glutGet(GLUT_ELAPSED_TIME);
   float dt = time1 - time0;
   time0 = time1;

   int prevFrame = frame;
   frame = (int)(time1 / 1000.0 * 60);
   
   time2 += dt;
   //update 60 times a second
   if (time2 / 1000 * 60 > 1)
   {
     /* for (int i = 0; i < objectList.size(); i++)
      {
         objectList.at(i)->RefreshBBox();
      }
      scoutModel->getHitBox()->RefreshHitBox();
      scoutModel->getHitBox()->checkVectorCollision(origin, ray);
     // cout << origin.x << " " << origin.y << " " << origin.z << endl;
      if (keyToggles['b']) {
         baseQuadTree->clear();
         for (int i = 0; i < objectList.size(); i++)
         {
            baseQuadTree->insert(objectList.at(i).get());
         }
         for (int i = 0; i < objectList.size(); i++)
         {
            objectList.at(i)->update(1 / 1000.0 * 60);

            std::vector<GameObject *>  toCompare = baseQuadTree->retrieve(objectList.at(i).get());
            for (int j = 0; j < toCompare.size(); j++)
            {
               if (objectList.at(i).get() != toCompare.at(j))
                  objectList.at(i)->checkCollision(toCompare.at(j));
            }
         }
      }
      time2 = 0;*/
   }

   if (dt / 1000 > 0)
   {
      player.processKeyboardControl(keyPresses, dt / 1000);
      for (int i = 0; i < theWorld.worldObjects.size(); i++)
      {

         WallObject *ptr = dynamic_cast <WallObject *> (theWorld.worldObjects.at(i));

         if (ptr != 0)
         {
            glm::vec3 out = ptr->checkPlayerCollision(&player, dt / 1000);

         }

      }

      camera.updateFixedCamera(player.getViewPosition(), player.getViewRotation());

      theWorld.ProcessKeyPresses(keyToggles, keyPresses, dt / 1000);
      scoutModel->animate(frame, 1);
      player.update(dt / 1000);
      playerModel->setPosition(player.worldPos);
      playerModel->setRotation(glm::vec3(0, camera.rotations.x + 3.141592f + player.rotation.y, 0.0f));
      playerModel->animate(frame  , player.playAnimation);
   
   }
}


enum MENU_ITEMS
{
   LOAD_WORLD,
   SAVE_WORLD,
   IMPORT_MODEL,
   CREATE_MODEL,
   IMPORT_TEXTURE,
   APPLY_TEXTURE,
   DUPLICATE_OBJECT,
   NORMALIZE_OBJECT,
   SET_UV_SCALE,
   SET_NORMALIZED_UV_SCALE,
   DELETE_OBJECT,
   SET_SCALE,
   SET_POSITION,
   SET_ROTATION,
   CREATE_WALL,
};

void menu(int item)
{
   switch (item)
   {
      case LOAD_WORLD:
         theWorld.loadWorld();
         break;
      case SAVE_WORLD:
         theWorld.saveWorld();
         break;
      case IMPORT_MODEL:
         theWorld.ImportModelFile();
         break;
      case CREATE_MODEL:
         theWorld.CreateModelFromLib();
         break;
      case IMPORT_TEXTURE:
         theWorld.ImportTexture();
         break;
      case APPLY_TEXTURE:
         theWorld.ApplyTextureFromLib();
         break;
      case DUPLICATE_OBJECT:
         theWorld.DuplicateCurObject();
         break;
      case NORMALIZE_OBJECT:
         theWorld.NormalizeSelectedObject();
         break;
      case DELETE_OBJECT:
         theWorld.DeleteSelectedObject();
         break;
      case CREATE_WALL:
         theWorld.CreateWallObject();
      default:
         {       /* Nothing */       }
         break;
   }

   glutPostRedisplay();

   return;
}

void transformationMenu(int item)
{
   switch (item)
   {
      case SET_UV_SCALE:
         theWorld.SetSelectedUVScale();
         break;
      case SET_POSITION:
         theWorld.SetSelectedPosition();
         break;
      case SET_ROTATION:
         theWorld.SetSelectedRotation();
         break;
      case SET_SCALE:
         theWorld.SetSelectedScale();
         break;
      case SET_NORMALIZED_UV_SCALE:
         theWorld.SetSelectedNormalizedUVScale();
         break;
      default:
      {       /* Nothing */       }
      break;
   }

   glutPostRedisplay();

   return;

}

int main(int argc, char **argv)
{

   glutInit(&argc, argv);
   
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(400, 400);
   glutCreateWindow("Rooftop Hopper");

   #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
   GLenum err = glewInit();

   if (GLEW_OK != err)
   {
      /* Problem: glewInit failed, something is seriously wrong.*/
      printf("WOW SOMETHING IS WRONG\n");
      return 1;
   }
   #endif


   

   int handle = glutCreateMenu(transformationMenu);
   glutAddMenuEntry("Set Position", SET_POSITION);
   glutAddMenuEntry("Set Rotation (in Degrees)", SET_ROTATION);
   glutAddMenuEntry("Set  Scale for x y and z", SET_SCALE);
   glutAddMenuEntry("Set UV scale of selected", SET_UV_SCALE);
   glutAddMenuEntry("Set Normalized UV scale of selected", SET_NORMALIZED_UV_SCALE);
   glutCreateMenu(menu);
   // Add menu items
   glutAddSubMenu("Transformations ", handle);
   glutAddMenuEntry("Load a world file", LOAD_WORLD);
   glutAddMenuEntry("Save the world to a file", SAVE_WORLD);
   glutAddMenuEntry("Import a model from a file", IMPORT_MODEL);
   glutAddMenuEntry("Create a model from imported models library", CREATE_MODEL);
   glutAddMenuEntry("Create Wall Brush", CREATE_WALL);
   glutAddMenuEntry("Duplicate the selected object", DUPLICATE_OBJECT);
   glutAddMenuEntry("Normalize the selected object's scale", NORMALIZE_OBJECT);
   glutAddMenuEntry("Delete the selected object", DELETE_OBJECT);
   glutAddMenuEntry("Import a texture", IMPORT_TEXTURE);
   glutAddMenuEntry("Apply a texture from the library to the selected model", APPLY_TEXTURE);
   
   
   // Associate a mouse button with menu
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   glutMouseFunc(mouseGL);
   glutMotionFunc(mouseMotionGL);
   glutPassiveMotionFunc(passiveMotionGL);
   glutKeyboardFunc(keyboardGL);
   glutReshapeFunc(reshapeGL);
   glutDisplayFunc(drawGL);
   glutTimerFunc(20, timerGL, 0);
   glutKeyboardUpFunc(keyboardUp);
   glutIdleFunc(idleGL);
   
   loadScene();
   initGL();
   glutMainLoop();
   return 0;
}