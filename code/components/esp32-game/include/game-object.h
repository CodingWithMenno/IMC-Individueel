#ifndef LCD_OBJECT_H
#define LCD_OBJECT_H

// Includes needed when using this header file
#include "i2c-lcd1602.h"

/*
    This component is for all the functions and other things that are needed for a game object
*/

// LCD attributes
#define LCD_HEIGHT 4
#define LCD_WIDTH 20

// Some constants for the game objects
#define MAX_WIDTH_TEXTURE 20
// Used to define a object as invalid (set the useCustomTexture as INVALID)
#define INVALID_OBJECT 69
// The standard gravity for each object
#define OBJECT_GRAVITY 0.3


// Structure for a position (coordinate) on the lcd
typedef struct
{
	double x;
	double y;
} COORDINATE;

// Structure for the texture of a GAME_OBJECT (this is drawn on the lcd)
typedef union
{
	uint8_t customTexture[8];
	char text[MAX_WIDTH_TEXTURE];
} TEXTURE;

// Structure for the defenition of a object which can be drawn on the lcd
typedef struct
{
	TEXTURE texture;
	int useCustomTexture;
	COORDINATE position;
} GAME_OBJECT;

/*
	Function to check if 2 game objects are colliding

	@param 1: Object 1
	@param 2: Object 2

	@returns: A boolean (1, 0) to see if the objects are colliding
*/
int object_collided(GAME_OBJECT, GAME_OBJECT);

#endif