#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

// Includes needed when using this header file
#include "i2c-lcd1602.h"
#include "game-object.h"

/*
    This component is the representation of the camera in the project.
    When rendering a scene/object the camera offsets will get subtracted from their position
*/

/*
    Function to set the position for the camera.
    (the camera will teleport directly to this position)

    @param 1: The coordinate where the camera needs to teleport to
*/
void camera_set(COORDINATE);

/*
    Function to move the camera to the given object.

    @param 1: A game object which the camera will follow
*/
void camera_follow(GAME_OBJECT);

/*
    Function to move the camera. The given coordinate wil get added
    to the camera's position.

    @param 1: A coordinate with a position to add to the camera's position
*/
void camera_move(COORDINATE);

/*
    Function to get the offset (position) of the camera.
    This function is used for rendering scenes/objects

    @returns: A coordinate with the position of the camera
*/
COORDINATE camera_Offset();

#endif