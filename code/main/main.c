#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32/rom/uart.h"

#include "smbus.h"
#include "i2c-lcd1602.h"
#include "game-scene.h"
// #include "game-object.h"
// #include "game-camera.h"
// #include "game-renderer.h"
#include "qwiic_twist.h"

#define TAG "MAIN"

#undef USE_STDIN

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     
#define I2C_MASTER_RX_BUF_LEN    0                     
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        CONFIG_I2C_MASTER_SDA
#define I2C_MASTER_SCL_IO        CONFIG_I2C_MASTER_SCL
#define LCD_NUM_ROWS			 4
#define LCD_NUM_COLUMNS			 40
#define LCD_NUM_VIS_COLUMNS		 20


// I2C functions
static void i2cMasterInit();
static void init();

// rotary encoder functions
static void onEncoderClicked();
static void onEncoderPressed();
static void onEncoderMoved(int16_t);

// Other functions
static void wait(unsigned int);


// Semaphore for the main loop
static SemaphoreHandle_t mainLoopMutex; 

// Boolean and int to check if you holded the rotary encoder for a moment
static bool isHolded = false;
static int clickCounter = 0;

// The player
// static GAME_OBJECT player;

void app_main()
{
    init();

    // player.position.x = 3;
    // player.position.y = 1;
    // strcpy(player.texture.text, "O");
    // player.useCustomTexture = 0;
    // camera_set(player.position);

    // GAME_OBJECT test;
    // test.position.x = 0;
    // test.position.y = 2;
    // strcpy(test.texture.text, "VV");
    // test.useCustomTexture = 0;


    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        xSemaphoreTake(mainLoopMutex, portMAX_DELAY);

        // Update
        // camera_follow(player);


        // Render
        // renderer_prepare();
        // renderer_renderObject(player, camera_Offset());
        // renderer_renderObject(test, camera_Offset());

        xSemaphoreGive(mainLoopMutex);

        // This way every 100 seconds the loop will have another iteration
        vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_RATE_MS));
    }
}

// In this function the i2c, the lcd and the rotary encoder will be setup
static void init() 
{
    // Set up I2C
    i2cMasterInit();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;
    

    // Set up the SMBus
    smbus_info_t *smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // Lcd init
    i2c_lcd1602_info_t *lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);
    i2c_lcd1602_set_cursor(lcd_info, true);
    i2c_lcd1602_move_cursor(lcd_info, 4, 1);
    i2c_lcd1602_write_string(lcd_info, "Starting...");

    // Rotary encoder init
    qwiic_twist_t *qwiic_info = (qwiic_twist_t*) malloc(sizeof(qwiic_twist_t));

    qwiic_info->smbus_info = smbus_info;
    qwiic_info->i2c_addr = QWIIC_TWIST_ADDRESS;
    qwiic_info->port = i2c_num;
    qwiic_info->xMutex = xSemaphoreCreateMutex();
    qwiic_info->task_enabled = true;
    qwiic_info->task_time = 0;
    qwiic_info->onButtonPressed = &onEncoderPressed;
    qwiic_info->onButtonClicked = &onEncoderClicked;
    qwiic_info->onMoved = &onEncoderMoved;
    
    // renderer_init(lcd_info);
    mainLoopMutex = xSemaphoreCreateMutex();

    qwiic_twist_init(qwiic_info);
    qwiic_twist_start_task(qwiic_info);
}

// This function is called while the rotary encoder is pressed
static void onEncoderPressed()
{
    clickCounter++;
    if(clickCounter == 5)
    {
        // Do something

        clickCounter = 0;
        isHolded = true;
    }
}

// This function is called when the rotary encoder is clicked ()
static void onEncoderClicked()
{
    if (!isHolded)
    {
        // Do something
    }

    clickCounter = 0;
    isHolded = false; 
}

// This function is called when the rotary encoder is moved
static void onEncoderMoved(int16_t diff)
{
    xSemaphoreTake(mainLoopMutex, portMAX_DELAY);
    if(diff > 0)
    {
        // Rotary encoder has rotated to the right
        
        // player.position.x += 1;
    } else 
    {
        // Rotary encoder has rotated to the left

        // player.position.x -= 1;
    }
    xSemaphoreGive(mainLoopMutex);
}

// The I2C master is setup in this function
static void i2cMasterInit()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

// Call this function to delay the program with the given parameter (in milliseconds)
static void wait(unsigned int ms)
{
    vTaskDelay(ms / portTICK_RATE_MS);
}