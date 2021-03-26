#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <stdint.h>

/*
    This component is responsible for rendering and updating
    the current displaying scene.
*/


/*
    This function inits all the scenes and variables needed
    for this component.

    @param 1: A pointer to the lcd info
*/
void scene_init(i2c_lcd1602_info_t*);

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