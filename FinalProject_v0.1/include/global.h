#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_flash_ex.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"


#ifdef __cplusplus
extern "C"
#endif


typedef struct trackPadState {
	bool up;
	bool down;
	bool left;
	bool right;
	bool tap;
} TRACK_PAD_State;

void SystemClock_Config(void);
void Error_Handler(void);
void CPU_CACHE_Enable(void);
void system_init(void);
void LCD_Config(void);

void GPIO_Config(void);
void GPIO_init(GPIO_TypeDef*, GPIO_InitTypeDef*, uint32_t);

void trackpad_Config(void);
void get_track_pad_state(void);
void Gesture_demo(void);
void LCD_Texture_config(void);

TRACK_PAD_State track_pad_state;

GPIO_InitTypeDef d0_init;
GPIO_InitTypeDef d1_init;
GPIO_InitTypeDef d2_init;
GPIO_InitTypeDef d3_init;
GPIO_InitTypeDef d4_init;

#endif
