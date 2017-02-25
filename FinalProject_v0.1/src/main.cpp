#include "global.h"


int main(void) {
	system_init();
	LCD_Config();
	LCD_Texture_config();
	GPIO_Config();

	Cursor crs((int16_t) BSP_LCD_GetXSize() / 2, (int16_t)  BSP_LCD_GetYSize() / 2, RADIUS);
	while(1) {
		get_track_pad_state();
		Gesture_demo(crs);
	}
}

void LCD_Texture_config(void) {
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font24);
}

void LCD_Config(void) {
	/* LCD Initialization */
	BSP_LCD_Init();

	/* LCD Initialization */
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

	/* Enable the LCD */
	BSP_LCD_DisplayOn();

	/* Select the LCD Background Layer  */
	BSP_LCD_SelectLayer(0);

	/* Clear the Background Layer */
	BSP_LCD_Clear(BACK_COLOR);

	/* Configure the transparency for background */
	//BSP_LCD_SetTransparency(0, 100);
}

void GPIO_init(GPIO_TypeDef* gpio_bus, GPIO_InitTypeDef* gpio_typeDef, uint32_t PIN) {
	gpio_typeDef->Pin = PIN;
	gpio_typeDef->Pull = GPIO_PULLDOWN;
	gpio_typeDef->Mode = GPIO_MODE_INPUT;
	//gpio_typeDef->Speed = GPIO_SPEED_HIGH;
	//gpio_typeDef->Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(gpio_bus, gpio_typeDef);
}

void GPIO_Config(void) {
	GPIO_init(GPIOF, &d0_init, GPIO_PIN_7);
	GPIO_init(GPIOJ, &d1_init, GPIO_PIN_4);
	GPIO_init(GPIOJ, &d2_init, GPIO_PIN_1);
	GPIO_init(GPIOF, &d3_init, GPIO_PIN_6);
	GPIO_init(GPIOJ, &d4_init, GPIO_PIN_0);
}


void get_track_pad_state(void)
{
	track_pad_state.up = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7) == GPIO_PIN_SET);
	track_pad_state.down = (HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1) == GPIO_PIN_SET);
	track_pad_state.left = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET);
	track_pad_state.right = (HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_0) == GPIO_PIN_SET);
	track_pad_state.tap = (HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_4) == GPIO_PIN_SET);
}



void Gesture_demo(Cursor& crs) {
	if (track_pad_state.up) {
		crs.moveUp();
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "    UP     ");
	} else if (track_pad_state.down) {
		crs.moveDown();
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "   DOWN    ");
	} else if (track_pad_state.left) {
		crs.moveLeft();
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "   LEFT    ");
	} else if (track_pad_state.right) {
		crs.moveRight();
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "   RIGHT   ");
	} else if(track_pad_state.tap) {
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "    TAP    ");
	} else {
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "NOT TOUCHED");
	}
}

/* -- System Configuration Functions -- */

void system_init(void) {
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();
}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}


void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

void Error_Handler(void)
{
    /* Turn LED1 on */
    BSP_LED_On(LED1);
    while(1)
    {
    }
}

/* -- Cursor Class Functions-- */
Cursor::Cursor(const int16_t x, const int16_t y, const uint16_t rad) : x_(x), y_(y), rad_(rad) {
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle(x, y, rad * 2);
}

void Cursor::CopyFrom(const Cursor &copy) {
	x_ = copy.x_;
	y_ = copy.y_;
	rad_ = copy.rad_;
}

void Cursor::SetLocation(const int16_t x, const int16_t y) {
	if((x > BSP_LCD_GetXSize() - rad_ * 2) || (y > BSP_LCD_GetYSize() - rad_ * 2) || (x < rad_ * 2) || (y < rad_ * 2)) return ;
	BSP_LCD_SetTextColor(BACK_COLOR);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, rad_ * 2);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle((uint16_t) x, (uint16_t) x, rad_ * 2);
	x_ = x;
	y_ = y;
}

void Cursor::moveUp(void) {
	BSP_LCD_SetTextColor(BACK_COLOR);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, RADIUS * 2);
	if(y_ > RADIUS * 2) y_--;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, RADIUS * 2);
}

void Cursor::moveDown(void) {
	BSP_LCD_SetTextColor(BACK_COLOR);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, RADIUS * 2);
	if((uint16_t) y_ < BSP_LCD_GetYSize() - RADIUS * 2) y_++;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, 20);
}

void Cursor::moveLeft(void) {
	BSP_LCD_SetTextColor(BACK_COLOR);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, RADIUS * 2);
	if(x_ > RADIUS * 2) x_--;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, 20);
}

void Cursor::moveRight(void) {
	BSP_LCD_SetTextColor(BACK_COLOR);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, RADIUS * 2);
	if((uint16_t) x_ < BSP_LCD_GetXSize() - RADIUS * 2) x_++;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle((uint16_t) x_, (uint16_t) y_, 20);
}


