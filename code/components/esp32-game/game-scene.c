#include <stdlib.h>
#include <string.h>
#include "game-object.h"
#include "game-camera.h"
#include "game-renderer.h"
#include "game-scene.h"

// Menu scene functions
static int menuSceneInit();
static void menuSceneUpdate();
static void menuSceneClicked();

// Main scene functions
static int mainSceneInit();
static void mainSceneUpdate();
static void mainSceneRotated(int16_t);
static void mainSceneClicked();
static void mainSceneHolded();

// Other functions
int switchScene(int);

#define TOTAL_SCENES 1
#define MAX_OBJECTS_IN_SCENE 10

// The defenition of a scène in the game
typedef struct
{
    int id;
    GAME_OBJECT *objects;
    int (*scene_init)();
    void (*scene_update)();
    void (*scene_userClicked)();
    void (*scene_userHolded)();
    void (*scene_userRotated)(int16_t);
} GAME_SCENE;

// Semaphore for the main loop
static SemaphoreHandle_t mainLoopMutex; 

// The current scene 
static int currentSceneId;
// All the scenes in the game
static GAME_SCENE *scenes;


int scene_init(i2c_lcd1602_info_t *lcdInfo)
{
    mainLoopMutex = xSemaphoreCreateMutex();

    renderer_init(lcdInfo);

    scenes = (GAME_SCENE*) malloc(sizeof(GAME_SCENE) * TOTAL_SCENES);
    if (scenes == NULL)
    {
        return GAME_ERROR;
    }

    // Set the init functions for every scene
    scenes[SCENE_MAIN_ID].scene_init = &mainSceneInit;
    scenes[SCENE_MENU_ID].scene_init = &menuSceneInit;

    // Init the first scene
    currentSceneId = SCENE_MAIN_ID;    
    return scenes[currentSceneId].scene_init();
}

int scene_switch(int newSceneId)
{
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

    int errorCode = switchScene(newSceneId);

    xSemaphoreGive(mainLoopMutex);

    return errorCode;
}

// Intern function for switching the scene, so no deadlock will occur (without semaphore)
int switchScene(int newSceneId)
{
    // Init the new scene (return if failed)
    int errorCode = scenes[newSceneId].scene_init();
    if (errorCode == GAME_ERROR)
    {
        return GAME_ERROR;
    }

    // Trying to free the objects caused a lot of problems
    // free(scenes[currentSceneId].objects);

    // Set the new scene as the current scene
    currentSceneId = newSceneId;
    return GAME_OKE;
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

    // Render every object in the scene
    for (int i = 0; i < MAX_OBJECTS_IN_SCENE; i++)
    {
        // The fill-up item is always the last item
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
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

    if (scenes[currentSceneId].scene_userClicked != NULL)
    {
        scenes[currentSceneId].scene_userClicked();
    }

    xSemaphoreGive(mainLoopMutex);
}

void scene_userHolded()
{
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

    if (scenes[currentSceneId].scene_userHolded != NULL)
    {
        scenes[currentSceneId].scene_userHolded();
    }

    xSemaphoreGive(mainLoopMutex);
}


// From here are all the elaborations of the scenes

// Menu scene
static int menuSceneInit()
{
    static int isInit = 0;
    if (!isInit)
    {
        scenes[SCENE_MENU_ID].id = SCENE_MENU_ID;
        scenes[SCENE_MENU_ID].scene_init = &menuSceneInit;
        scenes[SCENE_MENU_ID].scene_update = &menuSceneUpdate;
        scenes[SCENE_MENU_ID].scene_userClicked = &menuSceneClicked;
        scenes[SCENE_MENU_ID].scene_userHolded = NULL;
        scenes[SCENE_MENU_ID].scene_userRotated = NULL;
        scenes[SCENE_MENU_ID].objects = (GAME_OBJECT*) malloc(sizeof(GAME_OBJECT) * MAX_OBJECTS_IN_SCENE);
        if (scenes[SCENE_MENU_ID].objects == NULL)
        {
            return GAME_ERROR;
        }
    }
    
    // Test UI item
    scenes[SCENE_MENU_ID].objects[0].position.x = 1;
    scenes[SCENE_MENU_ID].objects[0].position.y = 1;
    strcpy(scenes[SCENE_MENU_ID].objects[0].texture.text, "Click to try again");
    scenes[SCENE_MENU_ID].objects[0].useCustomTexture = 0;
    // Fill-up item
    scenes[SCENE_MENU_ID].objects[1].useCustomTexture = INVALID_OBJECT;

    isInit = 1;
    return GAME_OKE;
}

static void menuSceneUpdate()
{
    COORDINATE camPos;
    camPos.x = 0;
    camPos.y = 0;
    camera_set(camPos);
}

static void menuSceneClicked()
{
    switchScene(SCENE_MAIN_ID);
}

// Main scene
static int mainSceneInit()
{
    static int isInit = 0;
    if (!isInit)
    {
        scenes[SCENE_MAIN_ID].id = SCENE_MAIN_ID;
        scenes[SCENE_MAIN_ID].scene_init = &mainSceneInit;
        scenes[SCENE_MAIN_ID].scene_update = &mainSceneUpdate;
        scenes[SCENE_MAIN_ID].scene_userClicked = &mainSceneClicked;
        scenes[SCENE_MAIN_ID].scene_userHolded = &mainSceneHolded;
        scenes[SCENE_MAIN_ID].scene_userRotated = &mainSceneRotated;
        scenes[SCENE_MAIN_ID].objects = (GAME_OBJECT*) malloc(sizeof(GAME_OBJECT) * MAX_OBJECTS_IN_SCENE);
        if (scenes[SCENE_MAIN_ID].objects == NULL)
        {
            return GAME_ERROR;
        }
    }
    
    // Player
    scenes[SCENE_MAIN_ID].objects[0].position.x = 3;
    scenes[SCENE_MAIN_ID].objects[0].position.y = -20;
    strcpy(scenes[SCENE_MAIN_ID].objects[0].texture.text, "O");
    scenes[SCENE_MAIN_ID].objects[0].useCustomTexture = 0;
    camera_set(scenes[SCENE_MAIN_ID].objects[0].position);
    // Obstacle 1
    scenes[SCENE_MAIN_ID].objects[1].position.x = 0;
    scenes[SCENE_MAIN_ID].objects[1].position.y = -18;
    strcpy(scenes[SCENE_MAIN_ID].objects[1].texture.text, "VVVVVV");
    scenes[SCENE_MAIN_ID].objects[1].useCustomTexture = 0;
    // Obstacle 2
    scenes[SCENE_MAIN_ID].objects[2].position.x = 8;
    scenes[SCENE_MAIN_ID].objects[2].position.y = -16;
    strcpy(scenes[SCENE_MAIN_ID].objects[2].texture.text, "VVVVVV");
    scenes[SCENE_MAIN_ID].objects[2].useCustomTexture = 0;
    // Obstacle 3
    scenes[SCENE_MAIN_ID].objects[3].position.x = -7;
    scenes[SCENE_MAIN_ID].objects[3].position.y = -20;
    strcpy(scenes[SCENE_MAIN_ID].objects[3].texture.text, "VVVVVV");
    scenes[SCENE_MAIN_ID].objects[3].useCustomTexture = 0;
    // Fill-up item
    scenes[SCENE_MAIN_ID].objects[4].useCustomTexture = INVALID_OBJECT;

    isInit = 1;
    return GAME_OKE;
}

static void mainSceneUpdate()
{
    // Let the camera follow the player
    camera_follow(scenes[SCENE_MAIN_ID].objects[0]);

    // Apply gravity
    scenes[SCENE_MAIN_ID].objects[0].position.y += OBJECT_GRAVITY;

    // Check collisions
    for (int i = 1; i < MAX_OBJECTS_IN_SCENE; i++)
    {
        if (scenes[currentSceneId].objects[i].useCustomTexture == INVALID_OBJECT)
        {
            break;
        }
        
        if (object_collided(scenes[SCENE_MAIN_ID].objects[0], scenes[currentSceneId].objects[i]))
        {
            // Collided, so place player on top of other object
            scenes[SCENE_MAIN_ID].objects[0].position.y = scenes[SCENE_MAIN_ID].objects[i].position.y - 1;
            break;
        }  
    }

    if (scenes[SCENE_MAIN_ID].objects[0].position.y > -10)
    {
        // Player died
        switchScene(SCENE_MENU_ID);
    }
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

static void mainSceneClicked()
{   
    scenes[SCENE_MAIN_ID].objects[0].position.y -= 3.5;
}

static void mainSceneHolded()
{
    switchScene(SCENE_MENU_ID);
}