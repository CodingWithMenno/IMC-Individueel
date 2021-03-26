#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <stdint.h>

/*
    This component is responsible for rendering and updating
    the current displaying scene.
*/

// The defenition of a scène in the game
typedef struct
{
    int id;
    void (*scene_update)(void);
    void (*scene_render)(void);
    void (*scene_userClicked)();
    void (*scene_userHolded)();
    void (*scene_userRotatedRight)(int16_t);
    void (*scene_userRotatedLeft)(int16_t);
} GAME_SCENE;

/*
    This function inits all the scenes and variables needed
    for this component.
*/
void scene_init();

/*
    This function updates the current displaying scene.
*/
void scene_update();

/*
    This function renders the current displaying scene.
*/
void scene_render();

/*
    This function needs to be called when the user presses
    on a button (the rotary encoder).
*/
void scene_userClicked();

/*
    This function needs to be called when the user
    holds a button (the rotary encoder).
*/
void scene_userHolded();

/*
    This function needs to be called when the user
    rotates a knob (the rotary encoder).

    @param 1: The amount the knob has rotated (negative = left, positive = right)
*/
void scene_userRotated(int16_t);

#endif