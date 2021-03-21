#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

/*
    This component is for rendering game-objects on the lcd
*/

// Error codes for the renderer
#define RENDERER_OKE 0
#define RENDERER_ERROR 1
#define RENDERER_NOT_RENDERED 2

// LCD attributes
#define LCD_NUM_ROWS 4
#define LCD_NUM_COLUMNS 20

/*
    Function to init the game renderer, call this method when starting the application.

    @param 1: A pointer to the lcd info

    @returns: A renderer error code
*/
int renderer_init(i2c_lcd1602_info_t*);

/*
    Function to prepare the rendere to render a scene.
    Call this function on the start of each iteration in your main loop.

    @returns: A renderer error code
*/
int renderer_prepare();

/*
    Function to render a game object on the lcd.

    @param 1: The game object to render

    @returns: A renderer error code
*/
int renderer_renderObject(GAME_OBJECT);

#endif