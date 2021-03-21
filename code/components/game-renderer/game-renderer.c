#include "game-renderer.h"

static i2c_lcd1602_info_t *lcdInfo;

int renderer_init(i2c_lcd1602_info_t *newLcdInfo)
{
    lcdInfo = newLcdInfo;

    // Hides the cursor
    i2c_lcd1602_set_cursor(lcdInfo, false);

    return RENDERER_OKE;
}

int renderer_renderObject(GAME_OBJECT object);
{
    i2c_lcd1602_move_cursor(lcdInfo, object.position.x, object.position.y);

    if (object.useCustomTexture)
    {
        i2c_lcd1602_define_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0, object.texture.customTexture);
        i2c_lcd1602_write_char(lcdInfo, object.texture.customTexture);
    } else
    {
        i2c_lcd1602_write_string(lcdInfo, object.texture.text)
    }

    return RENDERER_OKE;
}