#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_flash_ex.h"
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"



void SystemClock_Config(void);
void Error_Handler(void);
void CPU_CACHE_Enable(void);
void LCD_Config(void);
#endif
