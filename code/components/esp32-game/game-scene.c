#include <stdlib.h>
#include <string.h>
#include "game-object.h"
#include "game-camera.h"
#include "game-renderer.h"
#include "game-scene.h"

static void mainSceneUpdate();
static void mainSceneRotated(int16_t);

#define TOTAL_SCENES 1
#define MAX_OBJECTS_IN_SCENE 10
#define SCENE_MAIN_ID 0

// The defenition of a scène in the game
typedef struct
{
    int id;
    GAME_OBJECT *objects;
    void (*scene_update)(void);
    void (*scene_userClicked)();
    void (*scene_userHolded)();
    void (*scene_userRotated)(int16_t);
} GAME_SCENE;

// Semaphore for the main loop
static SemaphoreHandle_t mainLoopMutex; 

static i2c_lcd1602_info_t *lcdInfo;

// The current scene 
static int currentSceneId;
// All the scenes in the game
static GAME_SCENE *scenes;


void scene_init(i2c_lcd1602_info_t *newLcdInfo)
{
    lcdInfo = newLcdInfo;

    mainLoopMutex = xSemaphoreCreateMutex();

    renderer_init(lcdInfo);

    scenes = (GAME_SCENE*) malloc(sizeof(GAME_SCENE) * TOTAL_SCENES);

    //TODO scene builder maken
    scenes[SCENE_MAIN_ID].id = SCENE_MAIN_ID;
    scenes[SCENE_MAIN_ID].scene_update = &mainSceneUpdate;
    scenes[SCENE_MAIN_ID].scene_userClicked = NULL;
    scenes[SCENE_MAIN_ID].scene_userHolded = NULL;
    scenes[SCENE_MAIN_ID].scene_userRotated = &mainSceneRotated;
    scenes[SCENE_MAIN_ID].objects = (GAME_OBJECT*) malloc(sizeof(GAME_OBJECT) * MAX_OBJECTS_IN_SCENE);
    scenes[SCENE_MAIN_ID].objects[0].position.x = 3;
    scenes[SCENE_MAIN_ID].objects[0].position.y = 1;
    strcpy(scenes[SCENE_MAIN_ID].objects[0].texture.text, "O");
    scenes[SCENE_MAIN_ID].objects[0].useCustomTexture = 0;
    camera_set(scenes[SCENE_MAIN_ID].objects[0].position);
    scenes[SCENE_MAIN_ID].objects[1].position.x = 0;
    scenes[SCENE_MAIN_ID].objects[1].position.y = 2;
    strcpy(scenes[SCENE_MAIN_ID].objects[1].texture.text, "VV");
    scenes[SCENE_MAIN_ID].objects[1].useCustomTexture = 0;
    scenes[SCENE_MAIN_ID].objects[2].useCustomTexture = INVALID_OBJECT;

    currentSceneId = SCENE_MAIN_ID;
}

void scene_update()
{
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

    if (scenes[currentSceneId].scene_update != NULL)
    {
        scenes[currentSceneId].scene_update();
    }

    xSemaphoreGive(mainLoopMutex);
}

void scene_render()
{
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

    renderer_prepare();

    for (int i = 0; i < MAX_OBJECTS_IN_SCENE; i++)
    {
        if (scenes[currentSceneId].objects[i].useCustomTexture == INVALID_OBJECT)
        {
            break;
        }
        
        renderer_renderObject(scenes[currentSceneId].objects[i], camera_Offset());
    }
    
    xSemaphoreGive(mainLoopMutex);
}

void scene_userRotated(int16_t diff)
{
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

    if (scenes[currentSceneId].scene_userRotated != NULL)
    {
        scenes[currentSceneId].scene_userRotated(diff);
    }

    xSemaphoreGive(mainLoopMutex);
}

void scene_userClicked()
{
    //TODO
}

void scene_userHolded()
{
    //TODO
}


// From here are all the elaborations of the scenes

//Main scene
static void mainSceneUpdate()
{
    camera_follow(scenes[SCENE_MAIN_ID].objects[0]);
}

static void mainSceneRotated(int16_t diff)
{
    if (diff > 0)
    {
        scenes[SCENE_MAIN_ID].objects[0].position.x++;
    } else
    {
        scenes[SCENE_MAIN_ID].objects[0].position.x--;
    }
}