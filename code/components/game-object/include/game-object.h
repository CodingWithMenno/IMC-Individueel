#ifndef LCD_OBJECT_H
#define LCD_OBJECT_H

/*
    This component is for all the functions and other things that are needed for a game object
*/

// Some constants for the game objects
#define MAX_WIDTH_TEXTURE 5


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

#endif