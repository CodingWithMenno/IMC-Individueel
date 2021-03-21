#include "string.h"
#include "i2c-lcd1602.h"
#include "game-object.h"
#include "game-renderer.h"

// Renders text on the lcd (with out of bounds detection)
static int renderText(GAME_OBJECT);

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
    // Rounds the double to the nearest int
    int x = (int) (object.position.x + 0.5 - (object.position.x < 0));
    int y = (int) (object.position.y + 0.5 - (object.position.y < 0));

    // Object should not render
    if (x >= LCD_NUM_COLUMNS || y >= LCD_NUM_ROWS)
    {
        return RENDERER_NOT_RENDERED;
    }
    

    i2c_lcd1602_move_cursor(lcdInfo, x, y);
    
    if (object.useCustomTexture)
    {
        // Check if the object is in bounds
        if (x < 0 || y < 0)
        {
            return RENDERER_NOT_RENDERED;
        }

        // Render custom char
        i2c_lcd1602_define_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0, object.texture.customTexture);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        return RENDERER_OKE;
    } else
    {
        // Render text
        return renderText(object);
    }
}

static int renderText(GAME_OBJECT object)
{
    int x = (int) (object.position.x + 0.5 - (object.position.x < 0));
    int y = (int) (object.position.y + 0.5 - (object.position.y < 0));

    if (x >= LCD_NUM_COLUMNS || y >= LCD_NUM_ROWS)
    {
        return RENDERER_NOT_RENDERED;
    }
    
    int length = strlen(object.texture.text);
    // Calculate how much the text is out of bounds on each side
    int rightOutOfBounds = x + (length) - LCD_NUM_COLUMNS;
    int leftOutOfBounds = -x;

    if (rightOutOfBounds <= 0 && leftOutOfBounds <= 0)
    {
        // No text out of bounds
        i2c_lcd1602_write_string(lcdInfo, object.texture.text);
        return RENDERER_OKE;
    }
    

    if (x >= (LCD_NUM_COLUMNS - MAX_WIDTH_TEXTURE))
    {
        // Watch out for the right border

        int end = length - rightOutOfBounds;
        for (int i = 0; i < end; i++)
        {
            i2c_lcd1602_write_char(lcdInfo, object.texture.text[i]);
        }
    } else
    {
        // Watch out for the left border

        i2c_lcd1602_move_cursor(lcdInfo, x + leftOutOfBounds, y);
        
        for (int i = leftOutOfBounds; i < length; i++)
        {
            i2c_lcd1602_write_char(lcdInfo, object.texture.text[i]);
        }
    }

    return RENDERER_OKE;
}