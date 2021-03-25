#include "game-camera.h"

// The position of the camera (offset)
static double xOffset = 0;
static double yOffset = 0;

void camera_set(COORDINATE newPos)
{
    xOffset = newPos.x;
    yOffset = newPos.y;
}

void camera_follow(GAME_OBJECT object)
{
    const double cameraSpeed = 0.3;

    double objectOffsetX = object.position.x - LCD_WIDTH / 2;
    double objectOffsetY = object.position.y - LCD_HEIGHT / 2;

    xOffset > objectOffsetX ? (xOffset -= cameraSpeed) : (xOffset += cameraSpeed);
    yOffset > objectOffsetY ? (yOffset -= cameraSpeed) : (yOffset += cameraSpeed);
}

void camera_move(COORDINATE move)
{
    xOffset += move.x;
    yOffset += move.y;
}

COORDINATE camera_Offset()
{
    COORDINATE cameraPos;
    cameraPos.x = xOffset;
    cameraPos.y = yOffset;
    return cameraPos;
}