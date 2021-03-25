#include "string.h"
#include "game-renderer.h"

// Renders text on the lcd (text can be drawn half on the screen with this function)
static int renderText(GAME_OBJECT, int, int);

static SemaphoreHandle_t renderMutex; 
static i2c_lcd1602_info_t *lcdInfo;


int renderer_init(i2c_lcd1602_info_t *newLcdInfo)
{
    lcdInfo = newLcdInfo;

    // Init the semaphore for the renderer
    renderMutex = xSemaphoreCreateMutex();

    // Hides the cursor
    i2c_lcd1602_set_cursor(lcdInfo, false);

    return RENDERER_OKE;
}

int renderer_prepare()
{
    // Clear the lcd
    i2c_lcd1602_clear(lcdInfo);
    return RENDERER_OKE;
}

int renderer_renderObject(GAME_OBJECT object, COORDINATE cameraPos)
{    
    // Rounds the double to the nearest int
    int x = (int) (object.position.x + 0.5 - (object.position.x < 0)) - (int) (cameraPos.x + 0.5 - (cameraPos.x < 0));
    int y = (int) (object.position.y + 0.5 - (object.position.y < 0)) - (int) (cameraPos.y + 0.5 - (cameraPos.y < 0));

    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
    {
        // Object is out of bounds
        return RENDERER_NOT_RENDERED;
    }
    
    xSemaphoreTake(renderMutex, portMAX_DELAY);

    i2c_lcd1602_move_cursor(lcdInfo, x, y);
    
    int errorCode = RENDERER_OKE;
    if (object.useCustomTexture)
    {
        // Check if the object is in bounds
        if (x < 0 || y < 0)
        {
            return RENDERER_NOT_RENDERED;
        }

        // Render custom texture
        i2c_lcd1602_define_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0, object.texture.customTexture);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
    } else
    {
        // Render text
        errorCode = renderText(object, x, y);
    }

    xSemaphoreGive(renderMutex);
    return errorCode;
}

static int renderText(GAME_OBJECT object, int x, int y)
{
    // Rounds the double to the nearest int
    // int x = (int) (object.position.x + 0.5 - (object.position.x < 0));
    // int y = (int) (object.position.y + 0.5 - (object.position.y < 0));

    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
    {
        // Text is out of bounds
        return RENDERER_NOT_RENDERED;
    }
    
    int length = strlen(object.texture.text);
    // Calculate how many chars are out of bounds on each side
    int rightOutOfBounds = x + (length) - LCD_WIDTH;
    int leftOutOfBounds = -x;

    if (rightOutOfBounds <= 0 && leftOutOfBounds <= 0)
    {
        // No chars out of bounds
        i2c_lcd1602_write_string(lcdInfo, object.texture.text);
        return RENDERER_OKE;
    }
    

    if (x >= (LCD_WIDTH - MAX_WIDTH_TEXTURE))
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