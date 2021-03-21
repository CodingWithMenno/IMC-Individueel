#include "i2c-lcd1602.h"
#include "game-object.h"
#include "game-renderer.h"

// Renders text on the lcd
int renderText(char[]);

static i2c_lcd1602_info_t *lcdInfo;

int renderer_init(i2c_lcd1602_info_t *newLcdInfo)
{
    lcdInfo = newLcdInfo;

    // Hides the cursor
    i2c_lcd1602_set_cursor(lcdInfo, false);

    return RENDERER_OKE;
}

int renderer_prepare()
{
    i2c_lcd1602_clear(lcdInfo);
    return RENDERER_OKE;
}

int renderer_renderObject(GAME_OBJECT object)
{
    int x = (int) object.position.x;
    int y = (int) object.position.y;

    i2c_lcd1602_move_cursor(lcdInfo, x, y);
    
    if (object.useCustomTexture)
    {
        // Check if the object is in bounds
        if (x < 0 || y < 0 || x >= LCD_NUM_COLUMNS || y >= LCD_NUM_ROWS)
        {
            return RENDERER_NOT_RENDERED;
        }

        i2c_lcd1602_define_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0, object.texture.customTexture);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
    } else
    {
        return renderText(object.texture.text);
        // i2c_lcd1602_write_string(lcdInfo, object.texture.text);
    }

    return RENDERER_OKE;
}

int renderText(char[])
{

}