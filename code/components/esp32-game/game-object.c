#include <string.h>
#include "game-object.h"

int object_collided(GAME_OBJECT object1, GAME_OBJECT object2)
{
    // Check if both objects are valid
    if (object1.useCustomTexture == INVALID_OBJECT || object2.useCustomTexture == INVALID_OBJECT)
    {
        return 0;
    }
    
    // Check if the objects are on the same y position (objects van only be 1 high)
    int y1 = (int) ((object1.position.y) + 0.5 - (object1.position.y < 0));
    int y2 = (int) ((object2.position.y) + 0.5 - (object2.position.y < 0));
    if (y1 != y2)
    {
        return 0;
    }
    
    int length1 = 1;
    int length2 = 1;
    if (!object1.useCustomTexture)
    {
        length1 = strlen(object1.texture.text);
    }
    
    if (!object2.useCustomTexture)
    {
        length2 = strlen(object2.texture.text);
    }

    if (object1.position.x <= object2.position.x && object1.position.x + (length1 - 1) >= object2.position.x)
    {
        return 1;
    } else if (object1.position.x >= object2.position.x && object2.position.x + (length2 - 1) >= object1.position.x)
    {
        return 1;
    } else
    {
        return 0;
    }
}