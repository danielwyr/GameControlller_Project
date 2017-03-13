#include "global.h"

static TS_StateTypeDef* ts_state;

/* -- Game Function Prototype & Typedef-- */
typedef struct {
	string name;
	int8_t currentMon;
	int8_t mon[3];
	int8_t tg;
	int8_t mg;
	int8_t mem[60];
} player;

typedef struct {
	int8_t currentHP;
	int8_t currentMP;
	int8_t	maxHP;
	int8_t maxMP;
	int8_t life;
	int8_t currentEHP;
	int8_t currentEMP;
	int8_t	enemyMaxHP;
	int8_t elife;
	int8_t round;
	int8_t dmg, cst, tangoA, mangoA;
	int8_t healH, healM;
	int8_t inDmg, eHeal; // enemy's damadge, heal
	int8_t key;
} game_state;

int8_t keyDecoder(int8_t);
void memAlloc(player*); // allocate memory
int8_t getAddress(int8_t, int8_t, int8_t);
int8_t getValue(player*, int8_t);
void setValue(player*, int8_t, int8_t);
void initPlayer(player*, int8_t, int8_t, int8_t);
int8_t pokemonSkill(int8_t, int8_t, int8_t);
int8_t items(int8_t, int8_t);
int8_t pokemonProp (int8_t, int8_t);
int8_t hpmp (int8_t);
Pokemon singlePokeDecoder(int8_t);
void game_Config(void);
void game_Logic(void);
void drawBalls(int8_t cnt, bool isSelf);
void drawPokemon(uint16_t X, uint16_t Y, Pokemon pokemon, bool is_plr);

player JunJun;
player JunE;


/* -- UART -- */
__IO ITStatus UartReady = RESET;
UART_HandleTypeDef UartHandle;
uint8_t txbuffer[1], rxbuffer[1];

/* -- ADC -- */
ADC_HandleTypeDef    VERT;
ADC_HandleTypeDef	 HORZ;

__IO int32_t VERT_Val = 0;
__IO int32_t HORZ_Val = 0;

/* -- UI -- */
UI_State ui_state;

/* -- Game -- */
uint8_t userin;
game_state gs;
uint8_t endgame = 0;
static bool end_;

bool PB_A, PB_B;
#define PB_X		599
#define PB_Y		239
#define PB_x_offset	60
#define PB_y_offset	60
#define PB_rad		50
#define PB_A_X		(PB_X + PB_x_offset)
#define PB_A_Y		(PB_Y - PB_y_offset)
#define PB_B_X		(PB_X - PB_x_offset)
#define PB_B_Y		(PB_Y + PB_y_offset)
#define JS_Width	80
#define JS_PB_Y		(239 - JS_Width / 2)
#define JS_PB_X		(200 - JS_Width / 2)


#define PLR_POK_X	614
#define PLR_POK_Y	150
#define ENM_POK_X	213
#define ENM_POK_Y	58

bool heart_rate_menu = false;

int main(void) {
	system_init();
	GPIO_Config();
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetXSize());
	BSP_TS_ITConfig();
	LCD_Config();
	LCD_Texture_config();

	ui_state = MAIN_MENU;
	drawLayout();

	PB_Config();

	ADC_Config();
	HAL_ADC_Start(&VERT);
	HAL_ADC_Start(&HORZ);

	UART_Config();


	/*for(uint8_t i = 0; i < size; i++) {
		txbuffer[i] = i;
		rxbuffer[i] = 0;
	}*/
	/*
	txbuffer[0] = 'a';
	if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)txbuffer, size)!= HAL_OK)
	{
		while(1);
	}

	while (UartReady != SET);

	UartReady = RESET;


	if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)rxbuffer, size) != HAL_OK)
	{
		while(1);
	}

	while (UartReady != SET);

	UartReady = RESET;*/

	//Cursor crs((int16_t) BSP_LCD_GetXSize() / 2, (int16_t)  BSP_LCD_GetYSize() / 2, RADIUS);
	volatile uint32_t vert = 0;
	volatile uint32_t horz = 0;
	JS_State cur = Center;
	JS_State pre = Center;

	//asm("MainLoop:");
	while(1) {
		//get_track_pad_state();
		//Gesture_demo(crs);
		if(ui_state == CONTROL_DEMO_MENU) {
			if(HAL_ADC_PollForConversion(&VERT, 1000) == HAL_OK) {
				vert = HAL_ADC_GetValue(&VERT);
			}
			if(HAL_ADC_PollForConversion(&HORZ, 1000) == HAL_OK) {
				horz = HAL_ADC_GetValue(&HORZ);
			}
			cur = dir[horz / 342][vert / 342];
			if(cur != pre) {
				restore_dir(pre);
				mark_dir(cur);
				pre = cur;
			}
		} else if(ui_state == GAME_DEMO_MENU) {
			game_Logic();
		} else if(ui_state == HEART_RATE_MENU) {
			while(HAL_UART_Receive(&UartHandle, (uint8_t *)rxbuffer, 1, 10) != HAL_OK && heart_rate_menu);

			/*while (UartReady != SET);

			UartReady = RESET;*/
			BSP_LCD_ClearStringLine(0);
			char val[10];
			itoa((int) rxbuffer[0], val, 10);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			if(heart_rate_menu) BSP_LCD_FillRect(371, 229, 90, 125);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			if(heart_rate_menu) BSP_LCD_DisplayStringAt(371, 229, (uint8_t*) val, LEFT_MODE);
		}
	}
}


void ADC_Config(void) {
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpio;
	gpio.Pin       = GPIO_PIN_4;
	gpio.Mode      = GPIO_MODE_ANALOG;
	gpio.Pull      = GPIO_NOPULL;
	gpio.Speed	   = GPIO_SPEED_LOW;
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
	HAL_GPIO_Init(GPIOA, &gpio);

	__HAL_RCC_ADC2_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	gpio.Pin       = GPIO_PIN_2;
	gpio.Mode      = GPIO_MODE_ANALOG;
	gpio.Pull      = GPIO_NOPULL;
	gpio.Speed	   = GPIO_SPEED_LOW;
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);
	HAL_GPIO_Init(GPIOC, &gpio);

	ADC_ChannelConfTypeDef VERT_Channel;
	ADC_ChannelConfTypeDef HORZ_Channel;

	/* Configure the ADC peripheral */
	VERT.Instance          = ADC2;

	VERT.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
	VERT.Init.Resolution            = ADC_RESOLUTION_10B;
	VERT.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	VERT.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled to have continuous conversion */
	VERT.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	VERT.Init.NbrOfDiscConversion   = 0;
	VERT.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start not trigged by an external event */
	VERT.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	VERT.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	VERT.Init.NbrOfConversion       = 1;
	VERT.Init.DMAContinuousRequests = ENABLE;
	VERT.Init.EOCSelection          = DISABLE;

	if (HAL_ADC_Init(&VERT) != HAL_OK)
	{
		while(1);
	}

	VERT_Channel.Channel = ADC_CHANNEL_4;
	VERT_Channel.Rank = 1;
	VERT_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	VERT_Channel.Offset = 0;

	if (HAL_ADC_ConfigChannel(&VERT, &VERT_Channel) != HAL_OK)
	{
		asm("bkpt 255");
	}

	HORZ.Instance          = ADC1;

	HORZ.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
	HORZ.Init.Resolution            = ADC_RESOLUTION_10B;
	HORZ.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	HORZ.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled to have continuous conversion */
	HORZ.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	HORZ.Init.NbrOfDiscConversion   = 0;
	HORZ.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start not trigged by an external event */
	HORZ.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	HORZ.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	HORZ.Init.NbrOfConversion       = 1;
	HORZ.Init.DMAContinuousRequests = ENABLE;
	HORZ.Init.EOCSelection          = DISABLE;

	if (HAL_ADC_Init(&HORZ) != HAL_OK)
	{
		while(1);
	}

	HORZ_Channel.Channel = ADC_CHANNEL_12;
	HORZ_Channel.Rank = 1;
	HORZ_Channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HORZ_Channel.Offset = 0;

	if (HAL_ADC_ConfigChannel(&HORZ, &HORZ_Channel) != HAL_OK)
	{
		asm("bkpt 255");
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle) {
	if(AdcHandle->Instance == ADC1) {
		VERT_Val = HAL_ADC_GetValue(AdcHandle);
	} else {
		HORZ_Val = HAL_ADC_GetValue(AdcHandle);
	}
}

void PB_Config(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpio;
	gpio.Pin       = GPIO_PIN_8;
	gpio.Mode      = GPIO_MODE_IT_RISING;
	gpio.Pull      = GPIO_NOPULL;
	gpio.Speed	   = GPIO_SPEED_FAST;
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Pin 	   = GPIO_PIN_9;
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Pin 	   = GPIO_PIN_12;
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);
	HAL_GPIO_Init(GPIOA, &gpio);

	/* Enable and set EXTI line 0 Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type) EXTI9_5_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ((IRQn_Type) EXTI9_5_IRQn);
}


void UART_Config(void) {
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
	  - Word Length = 8 Bits
	  - Stop Bit = One Stop bit
	  - Parity = None
	  - BaudRate = 9600 baud
	  - Hardware flow control disabled (RTS and CTS signals) */
	UartHandle.Instance        = USARTx;

	UartHandle.Init.BaudRate   = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
	{
	Error_Handler();
	}
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
	Error_Handler();
	}
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
  UartReady = SET;


}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
  UartReady = SET;


}

void onClick(void) {
	uint32_t X = (uint32_t) ts_state->touchX[0], Y = (uint32_t)ts_state->touchY[0];
	switch(ui_state) {
	case MAIN_MENU:
	{
		if(X >= 59 && X <= 184 && Y >= 294 && Y <= 419) { // Heart rate
			ui_state = HEART_RATE_MENU;
			heart_rate_menu = true;
			drawLayout();
			txbuffer[0] = 'h';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				//while(1);
			}
		} else if(X >= 244 && X <= 369 && Y >= 294 && Y <= 419) { // Game Demo
			ui_state = GAME_DEMO_MENU;
			end_ = false;
			endgame = 0;
			game_Config();
			drawLayout();
		} else if(X >= 429 && X <= 554 && Y >= 294 && Y <= 419) { // TV remote
			ui_state = TV_CONTROL_MENU;
			drawLayout();
		} else if(X >= 614 && X <= 739 && Y >= 294 && Y <= 419) { // control demo
			ui_state = CONTROL_DEMO_MENU;
			drawLayout();
		}
		break;
	}
	case HEART_RATE_MENU:
	{
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		if(X >= 174 && X <= 274 && Y >= 409 && Y <= 459) {
			ui_state = MAIN_MENU;
			heart_rate_menu = false;
			drawLayout();
			txbuffer[0] = 'q';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				//while(1);
			}
		}
		break;
	}
	case GAME_DEMO_MENU:
	{
		if(X >= 404 && X < 534 && Y >= 319 && Y < 399) {
			userin = 1;
		} else if(X >= 534 && X < 664 && Y >= 319 && Y < 399) {
			userin = 2;
		} else if(X >= 664 && X < 794 && Y >= 319 && Y < 399) {
			userin = 5;
		} else if(X >= 404 && X < 534 && Y >= 399 && Y < 480) {
			userin = 3;
		} else if(X >= 534 && X < 664 && Y >= 399 && Y < 480) {
			userin = 4;
		} else if(X >= 664 && X < 794 && Y >= 399 && Y < 480) {
			userin = 6;
		} else if(X >= 174 && X <= 274 && Y >= 409 && Y <= 459) {
			end_= true;
			ui_state = MAIN_MENU;
			drawLayout();
		}
		break;
	}
	case TV_CONTROL_MENU:
	{
		if(X >= 449 && X <= 574 && Y >= 49 && Y <= 174) {
			// Volume up
			txbuffer[0] = 'b';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				while(1);
			}
		} else if(X >= 449 && X <= 574 && Y >= 299 && Y <= 424) {
			// Volume down
			txbuffer[0] = 'c';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				while(1);
			}
		} else if(X >= 624 && X <= 749 && Y >= 49 && Y <= 174) {
			// Channel up
			txbuffer[0] = 'd';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				while(1);
			}
		} else if(X >= 624 && X <= 749 && Y >= 299 && Y <= 424) {
			// Channel down
			txbuffer[0] = 'e';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				while(1);
			}
		} else if(X >= 162 && X <= 287 && Y >= 49 && Y <= 174) {
			// Power
			txbuffer[0] = 'a';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				while(1);
			}
		} else if(X >= 162 && X <= 287 && Y >= 342 && Y <= 467) {
			// Source
			txbuffer[0] = 'f';
			if(HAL_UART_Transmit(&UartHandle, (uint8_t*)txbuffer, 1, 1000) != HAL_OK)
			{
				while(1);
			}
		}
		clickDefaultBackButton(X, Y);
		break;
	}
	case CONTROL_DEMO_MENU:
	{
		clickDefaultBackButton(X, Y);
		break;
	}

	default: break;
	}
}

void clickDefaultBackButton(uint16_t X, uint16_t Y) {
	if(X >= 174 && X <= 274 && Y >= 409 && Y <= 459) {
		ui_state = MAIN_MENU;
		drawLayout();
		//asm("b MainLoop");
	}
}

void drawDefaultBackButton(void) {
	drawBackButton(174, 409);
}

void drawBackButton(uint16_t X, uint16_t Y) {
	drawBitMap(BACK_BOTTON, X, Y);
}

void drawLayout(void) {
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	switch(ui_state) {
	case MAIN_MENU:
	{
		uint16_t icon_y = 294;
		drawBitMap(HEART_RATE, 60, icon_y);
		drawBitMap(POKEMON, 245, icon_y);
		drawBitMap(TV_REMOTE, 430, icon_y);
		drawBitMap(CONTROLLER, 615, icon_y);
		break;
	}
	case TV_CONTROL_MENU:
	{
		uint16_t vol_ch_y = 49;
		uint16_t vol_x = 449, ch_x = 624;
		uint16_t src_x = 162, src_y = 229;
		uint16_t pwr_x = 162, pwr_y = 49;
		drawBitMap(TV_VOLUME, vol_x, vol_ch_y);
		drawBitMap(TV_CHANNEL, ch_x, vol_ch_y);
		drawBitMap(TV_SOURCE, src_x, src_y);
		drawBitMap(TV_POWER, pwr_x, pwr_y);
		drawDefaultBackButton();
		break;
	}
	case HEART_RATE_MENU:
	{
		drawBitMap(HEART_RATE_LOGO, 149, 0);
		drawBitMap(BPM, 269, 200);
		drawBitMap(HEART_ICON, 99, 149);
		drawDefaultBackButton();
		break;
	}
	case GAME_DEMO_MENU: {
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_DrawRect(399, 319, 400, 160); // menu frame


		BSP_LCD_SetFont(&Font24);
		//BSP_LCD_FillRect(409, 339, 120, 40);
		BSP_LCD_DisplayStringAt(409, 339, (uint8_t*) "Tackle", LEFT_MODE);
		//BSP_LCD_FillRect(539, 339, 120, 40);
		BSP_LCD_DisplayStringAt(539, 339, (uint8_t*) "Kick", LEFT_MODE);
		//BSP_LCD_FillRect(669, 339, 120, 40);
		BSP_LCD_DisplayStringAt(669, 339, (uint8_t*) "Potion", LEFT_MODE);
		//BSP_LCD_FillRect(409, 419, 120, 40);
		BSP_LCD_DisplayStringAt(409, 419, (uint8_t*) "Punch", LEFT_MODE);
		//BSP_LCD_FillRect(539, 419, 120, 40);
		BSP_LCD_DisplayStringAt(539, 419, (uint8_t*) "Bite", LEFT_MODE);
		//BSP_LCD_FillRect(669, 419, 120, 40);
		BSP_LCD_DisplayStringAt(669, 419, (uint8_t*) "PP", LEFT_MODE);

		// Pokemons
		/*BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		BSP_LCD_FillRect(PLR_POK_X, PLR_POK_Y, 125, 125);
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_FillRect(ENM_POK_X, ENM_POK_Y, 125, 125);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);*/

		// lives
		drawBalls(0x07, true);
		drawBalls(0x07, false);

		// Menu
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_DrawRect(PLR_MP_FRM_X, PLR_MP_FRM_Y, MP_FRM_WIDTH, MP_FRM_HEIGHT); // player MP frame
		BSP_LCD_DrawRect(PLR_HP_FRM_X, PLR_HP_FRM_Y, HP_FRM_WIDTH, HP_FRM_HEIGHT); // player HP frame
		BSP_LCD_DrawRect(ENM_HP_FRM_X, ENM_HP_FRM_Y, HP_FRM_WIDTH, HP_FRM_HEIGHT); // enemy HP frame
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_FillRect(PLR_MP_CTT_X, PLR_MP_CTT_Y, MP_BAR_WIDTH, MP_BAR_HEIGHT); // player MP content
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_FillRect(PLR_HP_CTT_X, PLR_HP_CTT_Y, HP_BAR_WIDTH, HP_BAR_HEIGHT); // player HP content
		BSP_LCD_FillRect(ENM_HP_CTT_X, ENM_HP_CTT_Y, HP_BAR_WIDTH, HP_BAR_HEIGHT); // enemy HP content
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		drawDefaultBackButton();
		break;
	}
	case CONTROL_DEMO_MENU: {
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		PB_A = true;
		PB_B = true;
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_FillCircle(PB_A_X, PB_A_Y, PB_rad);
		BSP_LCD_FillCircle(PB_B_X, PB_B_Y, PB_rad);

#ifndef os_

#define os_ 10

#endif

		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y - JS_Width - os_, JS_Width, JS_Width); 	// up left
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y				   , JS_Width, JS_Width);	// left
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down left
		BSP_LCD_FillRect(JS_PB_X				 , JS_PB_Y - JS_Width - os_, JS_Width, JS_Width);	// up
		BSP_LCD_FillRect(JS_PB_X				 , JS_PB_Y				   , JS_Width, JS_Width);	// center
		BSP_LCD_FillRect(JS_PB_X			     , JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y - JS_Width - os_, JS_Width, JS_Width);	// up right
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y				   , JS_Width, JS_Width);	// right
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down right
		drawDefaultBackButton();
		break;
	}
	default: break;
	}
}



void restore_dir(JS_State jss) {
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	switch(jss) {
	case Down_Right:
	{
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down right
		break;
	}
	case Right:
	{
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y				   , JS_Width, JS_Width);	// right
		break;
	}
	case Up_Right:
	{
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y - JS_Width - os_, JS_Width, JS_Width);	// up right
		break;
	}
	case Down:
	{
		BSP_LCD_FillRect(JS_PB_X			     , JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down
		break;
	}
	case Center: break;
	case Up:
	{
		BSP_LCD_FillRect(JS_PB_X				 , JS_PB_Y - JS_Width - os_, JS_Width, JS_Width);	// up
		break;
	}
	case Down_Left:
	{
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down left
		break;
	}
	case Left:
	{
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y				   , JS_Width, JS_Width);	// left
		break;
	}
	case Up_Left:
	{
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y - JS_Width - os_, JS_Width, JS_Width); 	// up left
		break;
	}
	default: break;
	}
}

void mark_dir(JS_State jss) {
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	switch(jss) {
	case Down_Right:
	{
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down right
		break;
	}
	case Right:
	{
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y				   , JS_Width, JS_Width);	// right
		break;
	}
	case Up_Right:
	{
		BSP_LCD_FillRect(JS_PB_X + JS_Width + os_, JS_PB_Y - JS_Width - os_, JS_Width, JS_Width);	// up right
		break;
	}
	case Down:
	{
		BSP_LCD_FillRect(JS_PB_X			     , JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down
		break;
	}
	case Center: break;
	case Up:
	{
		BSP_LCD_FillRect(JS_PB_X				 , JS_PB_Y - JS_Width - os_, JS_Width, JS_Width);	// up
		break;
	}
	case Down_Left:
	{
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y + JS_Width + os_, JS_Width, JS_Width);	// down left
		break;
	}
	case Left:
	{
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y				   , JS_Width, JS_Width);	// left
		break;
	}
	case Up_Left:
	{
		BSP_LCD_FillRect(JS_PB_X - JS_Width - os_, JS_PB_Y - JS_Width - os_, JS_Width, JS_Width); 	// up left
		break;
	}
	default: break;
	}
}

uint32_t getBitFromPic(PICS pic, int x, int y) {
	switch(pic) {
	case HEART: return heart[x][y];
	case HEART_RATE: return heart_rate_icon[x][y];
	case TV_REMOTE: return tv_remote_icon[x][y];
	case TV_VOLUME: return tv_volume_icon[x][y];
	case TV_CHANNEL: return tv_channel_icon[x][y];
	case TV_SOURCE: return tv_source_icon[x][y];
	case TV_POWER: return tv_power_icon[x][y];
	case CONTROLLER: return controller_icon[x][y];
	case POKEMON: return pokemon_icon[x][y];
	case BACK_BOTTON: return backButton_icon[x][y];
	case PIKACHU_B: return pikachu_back_icon[x][y];
	case PIKACHU_F: return pikachu_front_icon[x][y];
	case BULBASAUR_B: return bulbasaur_back_icon[x][y];
	case BULBASAUR_F: return bulbasaur_front_icon[x][y];
	case CHARMANDER_B: return charmander_back_icon[x][y];
	case CHARMANDER_F: return charmander_front_icon[x][y];
	case EEVEE_B: return eevee_back_icon[x][y];
	case EEVEE_F: return eevee_front_icon[x][y];
	case SQUIRTLE_B: return squirtle_back_icon[x][y];
	case SQUIRTLE_F: return squirtle_front_icon[x][y];
	case ZUBAT_B: return zubat_back_icon[x][y];
	case ZUBAT_F: return zubat_front_icon[x][y];
	case BPM: return bpm_icon[x][y];
	case HEART_ICON: return heart_icon[x][y];
	case HEART_RATE_LOGO: return heart_rate_logo_icon[x][y];
	case POKE_BALL: return poketball_icon[x][y];
	default: return 0;
	}
}

uint16_t getXSizeFromPic(PICS pic) {
	switch(pic) {
	case HEART: return 40;
	case HEART_RATE: return 125;
	case TV_REMOTE: return 125;
	case TV_VOLUME: return 125;
	case TV_CHANNEL: return 125;
	case TV_SOURCE: return 125;
	case TV_POWER: return 125;
	case CONTROLLER: return 125;
	case POKEMON: return 125;
	case BACK_BOTTON: return 100;
	case PIKACHU_B: return 125;
	case PIKACHU_F: return 125;
	case BULBASAUR_B: return 125;
	case BULBASAUR_F: return 125;
	case CHARMANDER_B: return 125;
	case CHARMANDER_F: return 125;
	case EEVEE_B: return 125;
	case EEVEE_F: return 125;
	case SQUIRTLE_B: return 125;
	case SQUIRTLE_F: return 125;
	case ZUBAT_B: return 125;
	case ZUBAT_F: return 125;
	case BPM: return 100;
	case HEART_ICON: return 150;
	case HEART_RATE_LOGO: return 500;
	case POKE_BALL: return 20;
	default: return 0;
	}
}

uint16_t getYSizeFromPic(PICS pic) {
	switch(pic) {
	case HEART: return 40;
	case HEART_RATE: return 125;
	case TV_REMOTE: return 125;
	case TV_VOLUME: return 375;
	case TV_CHANNEL: return 375;
	case TV_SOURCE: return 125;
	case TV_POWER: return 125;
	case CONTROLLER: return 125;
	case POKEMON: return 125;
	case BACK_BOTTON: return 50;
	case PIKACHU_B: return 125;
	case PIKACHU_F: return 125;
	case BULBASAUR_B: return 125;
	case BULBASAUR_F: return 125;
	case CHARMANDER_B: return 125;
	case CHARMANDER_F: return 125;
	case EEVEE_B: return 125;
	case EEVEE_F: return 125;
	case SQUIRTLE_B: return 125;
	case SQUIRTLE_F: return 125;
	case ZUBAT_B: return 125;
	case ZUBAT_F: return 125;
	case BPM: return 60;
	case HEART_ICON: return 106;
	case HEART_RATE_LOGO: return 92;
	case POKE_BALL: return 20;
	default: return 0;
	}
}

void drawBitMap(PICS pic, uint16_t Xpos, uint16_t Ypos) {
	uint16_t XSize = getXSizeFromPic(pic);
	uint16_t YSize = getYSizeFromPic(pic);
	for(uint16_t i = 0; i < XSize; i++) {
		for(uint16_t j = 0; j < YSize; j++) {
			uint32_t pixel = getBitFromPic(pic, i, j);
			if(pixel >= 0xFF000000) {
				BSP_LCD_DrawPixel(Xpos + i, Ypos + j, pixel);
			}
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == GPIO_PIN_13) {
		// touch Screen Handler
		ts_state->touchDetected = 1;
		if(BSP_TS_GetState(ts_state) == TS_OK) {
			onClick();
			//touchScreenTest();
		}
	} else if(GPIO_Pin == GPIO_PIN_8) {
		if(ui_state == CONTROL_DEMO_MENU) {
			if(!PB_A) {
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillCircle(PB_A_X, PB_A_Y, PB_rad);
			} else {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_FillCircle(PB_A_X, PB_A_Y, PB_rad);
			}
			PB_A = !PB_A;
		}
	} else if(GPIO_Pin == GPIO_PIN_9) {
		if(ui_state == CONTROL_DEMO_MENU) {
			if(!PB_B) {
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillCircle(PB_B_X, PB_B_Y, PB_rad);
			} else {
				BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
				BSP_LCD_FillCircle(PB_B_X, PB_B_Y, PB_rad);
			}
			PB_B = !PB_B;
		}
	} else if(GPIO_Pin == GPIO_PIN_12) {
		if(ui_state == CONTROL_DEMO_MENU) {
			if(!PB_B) {
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(JS_PB_X, JS_PB_Y, JS_Width, JS_Width);
			} else {
				BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
				BSP_LCD_FillRect(JS_PB_X, JS_PB_Y, JS_Width, JS_Width);
			}
			PB_B = !PB_B;
		}
	}
}



void touchScreenTest(void) {
	uint32_t X = (uint32_t) ts_state->touchX[0], Y = (uint32_t)ts_state->touchY[0];
	string s = "";
	char x_index[10];
	char y_index[10];
	itoa((int) X, x_index, 10);
	itoa((int) Y, y_index, 10);
	string X_index(x_index);
	string Y_index(y_index);
	s.append("X = ").append(X_index).append(" ").append("Y = ").append(Y_index);
	const char* cc = s.c_str();
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAtLine(1, (uint8_t*) cc);

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
	HAL_GPIO_Init(gpio_bus, gpio_typeDef);
}

void GPIO_Config(void) {
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOJ_CLK_ENABLE();
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


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
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
	RCC_OscInitStruct.PLL.PLLQ = 9;
	RCC_OscInitStruct.PLL.PLLR = 7;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	/* Activate the OverDrive to reach the 216 MHz Frequency */
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

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority((IRQn_Type) SysTick_IRQn, 0x0F, 0);
}


/**
  * @brief  Configure the MPU attributes as Write Through for Internal SRAM1/2.
  * @note   The Base Address is 0x20020000 since this memory interface is the AXI.
  *         The Configured Region Size is 512KB because the internal SRAM1/2
  *         memory size is 384KB.
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* Disable the MPU */
	HAL_MPU_Disable();

	/* Configure the MPU attributes as WT for SRAM */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x20020000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
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
    //BSP_LED_On(Led_TypeDef::LED1);
    while(1)
    {
    	BSP_LED_Toggle(Led_TypeDef::LED2);
    	HAL_Delay(1000);
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



/* -- Game Logic -- */
void game_Config(void) {
	gs.round = START_GAME;

	JunJun.name = "JUNJUN";
	JunJun.currentMon = 0;
	initPlayer(&JunJun, 0, 1, 2);


	gs.key = 0;

	userin = 0;

	JunE.name = "JUNE";
	JunE.currentMon = 0;
	initPlayer(&JunE, 3, 4, 5);

	//alt_printf("%x, %x, %x, %x, %x, %x\n", JunJun.mon[0], JunJun.mon[1], JunJun.mon[2], JunE.mon[0], JunE.mon[1], JunE.mon[2]);
	//setValue(busyAddr + 60, 1);
	//uint8_t i;
	// change

	gs.maxHP = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'h'));
	gs.maxMP = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'm'));
	gs.enemyMaxHP = getValue(&JunE, getAddress(JunE.currentMon, 0, 'h'));

	gs.round = 0; // set the round to attack
}


void game_Logic(void)
{
	  //uint8_t round; // 0 for attack, 1 for incoming
	  //uint8_t dmg, cst, tangoA, mangoA; // damage, heal, cost
	  //uint8_t healH, healM;
	  //uint8_t inDmg, eHeal; // enemy's damadge, heal

	  // initialize pokemon database
	  //init();
	  //usleep(5000000);

	  if((endgame == 0) && !end_) {

		  // reset variables
		  gs.key = 0;
		  gs.cst = 0;
		  gs.dmg = 0;
		  gs.healH = 0;
		  gs.healM = 0;

		  // update current state
		  gs.currentHP = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'h'));
		  gs.currentMP = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'm'));
		  gs.currentEHP = getValue(&JunE, getAddress(JunE.currentMon, 0, 'h'));

		  // output current state to screen
		  double ratio = (double) gs.currentHP / (double) gs.maxHP;
		  uint16_t curhp = (uint16_t) ((double) HP_BAR_WIDTH * ratio);

		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  if(!end_) BSP_LCD_FillRect(PLR_HP_CTT_X, PLR_HP_CTT_Y, HP_BAR_WIDTH, HP_BAR_HEIGHT);
		  BSP_LCD_SetTextColor(LCD_COLOR_RED);
		  if(!end_ && (gs.currentHP > 0)) BSP_LCD_FillRect(PLR_HP_CTT_X, PLR_HP_CTT_Y, min((const int) curhp, (const int) HP_BAR_WIDTH), HP_BAR_HEIGHT);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		  ratio = (double) gs.currentMP / (double) gs.maxMP;
		  uint16_t curmp = (uint16_t) ((double) MP_BAR_WIDTH * ratio);


		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  if(!end_) BSP_LCD_FillRect(PLR_MP_CTT_X, PLR_MP_CTT_Y, MP_BAR_WIDTH, MP_BAR_HEIGHT);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		  if(!end_ && (gs.currentMP > 0)) BSP_LCD_FillRect(PLR_MP_CTT_X, PLR_MP_CTT_Y, min((const int) curmp, (const int) MP_BAR_WIDTH), MP_BAR_HEIGHT);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

		  ratio = (double) gs.currentEHP / (double) gs.enemyMaxHP;
		  uint16_t curehp = (uint16_t) ((double) HP_BAR_WIDTH * ratio);

		  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  if(!end_) BSP_LCD_FillRect(ENM_HP_CTT_X, ENM_HP_CTT_Y, HP_BAR_WIDTH, HP_BAR_HEIGHT);
		  BSP_LCD_SetTextColor(LCD_COLOR_RED);
		  if(!end_ && gs.currentEHP > 0) BSP_LCD_FillRect(ENM_HP_CTT_X, ENM_HP_CTT_Y, min((const int) curehp, (const int) HP_BAR_WIDTH), HP_BAR_HEIGHT);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);


		  //*currentmp = *gs.currentMP;
		  // *currentehp = *gs.currentEHP;
		  // *life = 0b111 >> JunJun.currentMon;
		  drawBalls((int8_t) (0x07 >> JunJun.currentMon), true);
		  drawBalls((int8_t) (0x07 >> JunE.currentMon), false);
		  drawPokemon(614, 150, singlePokeDecoder(JunJun.mon[JunJun.currentMon]), true);
		  drawPokemon(213, 58, singlePokeDecoder(JunE.mon[JunE.currentMon]), false);



		  /*
		  usleep(100);
		  alt_printf("myHP = %x\n", currentHP);
		  alt_printf("myMP = %x\n", currentMP);
		  alt_printf("enemyHP = %x\n", currentEHP);*/
		  if(0 == gs.round) { // attack
			  while((0 == gs.key) && !end_) {
				  gs.key |= userin;
				  userin = 0;
				  gs.key = keyDecoder(gs.key);
			  }
			  if(gs.key > 0 && gs.key <= 4) { // use skills
				  gs.healH = 0;
				  gs.healM = 0;
				  gs.cst = getValue(&JunJun, getAddress(JunJun.currentMon, gs.key, 'c'));
				  if(gs.cst <= gs.currentMP) { // have enough mana
					  gs.dmg = getValue(&JunJun, getAddress(JunJun.currentMon, gs.key, 'd'));
					  gs.currentMP -= gs.cst;
					  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'm'), gs.currentMP); // Update mp
				  } else { // don't have enough mana
					  gs.dmg = 0;
					  gs.healH = 0;
					  gs.healM = 0;
				  }
			  } else if(5 == gs.key) { // use tango
				  gs.dmg = 0;
				  gs.healM = 0;
				  gs.tangoA = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'l')); // get tango's amount
				  if(gs.tangoA > 0) { // have enough tango
					  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'l'), gs.tangoA - 1); // decrement tango's amount
					  //usleep(1000);
					  gs.healH = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 't')); // get tango's heal

				  } else {
					  // alt_printf("Don't have enough tango\n");
					  gs.healH = 0;
					  gs.healM = 0;
					  gs.dmg = 0;
				  }
			  } else if(6 == gs.key) { // use mango
				  gs.dmg = 0;
				  gs.healH = 0;
				  gs.mangoA = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'k')); // get mango's amount
				  if(gs.mangoA > 0) { // have enough mango
					  gs.healM = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'g')); // get mango's heal
					  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'k'), gs.mangoA - 1); // decrement mango's amount
					  //currentMP += healM; // heal

				  } else {
					  // alt_printf("Don't have enough mango\n");
					  gs.healH = 0;
					  gs.healM = 0;
					  gs.dmg = 0;
				  }
			  } else {
				  gs.dmg = 0;
				  gs.healH = 0;
				  gs.healM = 0;
			  }

			  if((gs.dmg != 0) || (gs.healH != 0) || (gs.healM != 0)) {
				  if(gs.currentHP + gs.healH > gs.maxHP) { // reach max HP
					  gs.healH = gs.maxHP - gs.currentHP;
				  }
				  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'h'), gs.currentHP + gs.healH); // set currentHP


				  if(gs.currentMP + gs.healM > gs.maxMP) { // reach max MP
					  gs.healM = gs.maxMP - gs.currentMP;
				  }
				  //alt_printf("healM = %x\n", healM);
				  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'm'), gs.currentMP + gs.healM);


				  //write(dmg); // make Damage
				  //write(healH); // acknowledge heal

				  //endgame = makeMove(&JunE, gs.dmg, &JunJun, gs.healH);

				  gs.round = 1; // switch round
				  if(gs.dmg != 0) {
					  gs.currentEHP -= gs.dmg;
					  if(gs.currentEHP <= 0 && JunE.currentMon != 2) {
						  gs.currentEHP = 0;
						  setValue(&JunJun, getAddress(JunE.currentMon, 0, 'h'), gs.currentEHP);
						  JunE.currentMon++;
						  gs.enemyMaxHP = getValue(&JunJun, getAddress(JunE.currentMon, 0, 'h')); // get new Enemy's max HP
						  //alt_printf("You killed enemy's pokemon.\n");
					  } else if(gs.currentEHP <= 0 && JunE.currentMon == 2) {
						  //*enable2 = 0;
					  	  drawPokemon(ENM_POK_X, ENM_POK_Y, NON_P, false);
					  	  drawBalls(0, false);
						  //alt_printf("You win\n");
						  endgame = 1;
					  } else {
						  setValue(&JunE, getAddress(JunE.currentMon, 0, 'h'), gs.currentEHP);
					  }
				  }
				  //alt_printf("dmg = %x, healH = %x, healM = %x\n", dmg, healH, healM);
			  }
		  } else { // incoming
			  gs.inDmg = 5;/*read();*/ // get damage
			  gs.eHeal = 0; /*read();*/ // get enemy's heal
			  //alt_printf("inDmg = %x, eHeal = %x\n", inDmg, eHeal);
			  if((gs.inDmg != 0) && (gs.eHeal == 0)) {
				  gs.currentHP -= gs.inDmg;
				  if(gs.currentHP <= 0 && JunJun.currentMon != 2){ // my pokemon is dead
					  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'h'), 0); // set current mon's hp to 0
					  JunJun.currentMon++; // increment currentMon
					  gs.maxHP = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'h'));
					  gs.maxMP = getValue(&JunJun, getAddress(JunJun.currentMon, 0, 'm'));
					  //*enable = pokemonDecoder(JunJun, JunE);
					  //alt_printf("Your pokemon is killed.\n");
				  } else if(gs.currentHP <= 0 && JunJun.currentMon == 2){
					  //*enable1 = 0;
					  drawPokemon(PLR_POK_X, PLR_POK_Y, NON_P, true);
					  drawBalls(0, true);
					  //*life = 0;
					  //alt_printf("You lost\n");
					  endgame = 1;
				  } else {
					  setValue(&JunJun, getAddress(JunJun.currentMon, 0, 'h'), gs.currentHP);
				  }
				  gs.round = 0;
			  } else if((gs.inDmg == 0) && (gs.eHeal != 0)) {
				  setValue(&JunE, getAddress(JunE.currentMon, 0, 'h'), gs.currentEHP + gs.eHeal); // enemy uses tango
				  gs.round = 0;
			  } else { // enemy uses mango
				  gs.round = 0;
			  }
		  }

	  }
	  //alt_printf("Game over\n");
}

void drawBalls(int8_t cnt, bool isSelf) {
	if(!end_) {
		if(isSelf) {
			if(cnt == 0x07) {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				drawBitMap(POKE_BALL, PLR_BALL_X1, PLR_BALL_Y);
				drawBitMap(POKE_BALL, PLR_BALL_X2, PLR_BALL_Y);
				drawBitMap(POKE_BALL, PLR_BALL_X3, PLR_BALL_Y);
				//BSP_LCD_FillRect(PLR_BALL_X1, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(PLR_BALL_X2, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(PLR_BALL_X3, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
			} else if(cnt == 0x03) {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				drawBitMap(POKE_BALL, PLR_BALL_X1, PLR_BALL_Y);
				drawBitMap(POKE_BALL, PLR_BALL_X2, PLR_BALL_Y);
				//BSP_LCD_FillRect(PLR_BALL_X1, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(PLR_BALL_X2, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawBitMap(POKE_BALL, PLR_BALL_X3, PLR_BALL_Y);
				//BSP_LCD_FillRect(PLR_BALL_X3, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
			} else if(cnt == 0x01) {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				drawBitMap(POKE_BALL, PLR_BALL_X1, PLR_BALL_Y);
				//BSP_LCD_FillRect(PLR_BALL_X1, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawBitMap(POKE_BALL, PLR_BALL_X2, PLR_BALL_Y);
				drawBitMap(POKE_BALL, PLR_BALL_X3, PLR_BALL_Y);
				//BSP_LCD_FillRect(PLR_BALL_X2, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(PLR_BALL_X3, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
			} else {
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawBitMap(POKE_BALL, PLR_BALL_X1, PLR_BALL_Y);
				drawBitMap(POKE_BALL, PLR_BALL_X2, PLR_BALL_Y);
				drawBitMap(POKE_BALL, PLR_BALL_X3, PLR_BALL_Y);
				//BSP_LCD_FillRect(PLR_BALL_X1, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(PLR_BALL_X2, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(PLR_BALL_X3, PLR_BALL_Y, BALL_SIZE, BALL_SIZE);
			}
		} else {
			if(cnt == 0x07) {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				drawBitMap(POKE_BALL, ENM_BALL_X1, ENM_BALL_Y);
				drawBitMap(POKE_BALL, ENM_BALL_X2, ENM_BALL_Y);
				drawBitMap(POKE_BALL, ENM_BALL_X3, ENM_BALL_Y);
				//BSP_LCD_FillRect(ENM_BALL_X1, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(ENM_BALL_X2, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(ENM_BALL_X3, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
			} else if(cnt == 0x03) {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				drawBitMap(POKE_BALL, ENM_BALL_X1, ENM_BALL_Y);
				drawBitMap(POKE_BALL, ENM_BALL_X2, ENM_BALL_Y);
				//BSP_LCD_FillRect(ENM_BALL_X1, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(ENM_BALL_X2, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawBitMap(POKE_BALL, ENM_BALL_X3, ENM_BALL_Y);
				//BSP_LCD_FillRect(ENM_BALL_X3, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
			} else if(cnt == 0x01) {
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				drawBitMap(POKE_BALL, ENM_BALL_X1, ENM_BALL_Y);
				//BSP_LCD_FillRect(ENM_BALL_X1, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawBitMap(POKE_BALL, ENM_BALL_X2, ENM_BALL_Y);
				drawBitMap(POKE_BALL, ENM_BALL_X3, ENM_BALL_Y);
				//BSP_LCD_FillRect(ENM_BALL_X2, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				//BSP_LCD_FillRect(ENM_BALL_X3, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
			} else {
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_FillRect(ENM_BALL_X1, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				BSP_LCD_FillRect(ENM_BALL_X2, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
				BSP_LCD_FillRect(ENM_BALL_X3, ENM_BALL_Y, BALL_SIZE, BALL_SIZE);
			}
		}
	}
}

void drawPokemon(uint16_t X, uint16_t Y, Pokemon pokemon, bool is_plr) {
	if(!end_) {
		switch(pokemon) {
			case Pikachu:
			{
				if(is_plr) drawBitMap(PIKACHU_B, X, Y);
				else drawBitMap(PIKACHU_F, X, Y);
				break;
			}
			case Squirtle:
			{
				if(is_plr) drawBitMap(SQUIRTLE_B, X, Y);
				else drawBitMap(SQUIRTLE_F, X, Y);
				break;
			}
			case Charmander:
			{
				if(is_plr) drawBitMap(CHARMANDER_B, X, Y);
				else drawBitMap(CHARMANDER_F, X, Y);
				break;
			}
			case Evee:
			{
				if(is_plr) drawBitMap(EEVEE_B, X, Y);
				else drawBitMap(EEVEE_F, X, Y);
				break;
			}
			case Bulbasaur:
			{
				if(is_plr) drawBitMap(BULBASAUR_B, X, Y);
				else drawBitMap(BULBASAUR_F, X, Y);
				break;
			}
			case Zubat:
			{
				if(is_plr) drawBitMap(ZUBAT_B, X, Y);
				else drawBitMap(ZUBAT_F, X, Y);
				break;
			}
			default:
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_FillRect(X, Y, 125, 125);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				break;
			}
		}
	}
}

Pokemon singlePokeDecoder(int8_t currentMon) {
	//char enableBus = 0;
	switch(currentMon) {
		case 0:
			return Pikachu;
		case 1:
			return Squirtle;
		case 2:
			return Charmander;
		case 3:
			return Evee;
		case 4:
			return Bulbasaur;
		case 5:
			return Zubat;
		default: return NON_P;
	}

}

// decode the key input
int8_t keyDecoder(int8_t key) {
	switch(key) {
		case 0b00000001:
			return 1;
		case 0b00000010:
			return 2;
		case 0b00000100:
			return 3;
		case 0b00001000:
			return 4;
		case 0b00010000:
			return 5; // tango
		case 0b00100000:
			return 6; // mango
		default :
			return 0;
	}
}




void setValue(player* plr, int8_t addr, int8_t data) {
	plr->mem[addr] = data;
}

// damage located at an address of currentMon * 20 + skillKey * 4 + 1
// cost located at an address of currentMon * 20 + skillKey * 4 + 2
// accuracy located at an address of currentMon * 20 + skillKey * 4 + 3
// pokemon No. located at an address of currentMon * 20 + 16
// pokemon's hp located at an address of currentMon * 20 + 17
// pokemon's mp located at an address of currentMon * 20 + 18

// notation brief:
// damage -- d
// cost -- c
// critical -- a
// pokemon's No. -- n
// pokemon's hp -- h
// pokemon's mp -- m
// tango's heal -- t
// tango's amount -- l
// mango's heal -- g
// mango's amount -- k
int8_t getAddress(int8_t currentMon, int8_t skill, int8_t type) {
	int offset = 0;
	int isSkill= 0;
	skill = skill - 1;
	if('d' == type) { // damage
		offset = 1;
		isSkill = 1;
	} else if('c' == type) { // cost
		offset = 2;
		isSkill = 1;
	} else if('a' == type) { // critical
		offset = 3;
		isSkill = 1;
	} else if('h' == type) { // hp
		offset = 16;
		isSkill = 0;
	} else if('m' == type) { //mp
		offset = 17;
		isSkill = 0;
	} else if('t' == type) { // tango's heal
		offset = 56;
		isSkill = 0;
	} else if('l' == type) { // tango's amount
		offset = 55;
		isSkill = 0;
	} else if('g' == type) { // mango's heal
		offset = 58;
		isSkill = 0;
	} else if('k' == type) { // mango's amount
		offset = 57;
		isSkill = 0;
	}

	return currentMon * 18 + isSkill * skill * 4 + offset;
}

int8_t getValue(player* plr, int8_t addr) {
    return plr->mem[addr];
}


int8_t pokemonSkill(int8_t pokemonNum, int8_t skillNum, int8_t type) {
	int8_t skill[6][4][3] = {
			{{5, 0, 1}, {25, 25, 5}, {15, 10, 3}, {30, 25, 3}}, // pokemon 0
			{{5, 0, 1}, {40, 50, 4}, {30, 25, 5}, {20, 15, 5}}, // pokemon 1
			{{5, 0, 1}, {20, 15, 5}, {20, 20, 3}, {20, 25, 4}},  // pokemon 2
			{{5, 0, 1}, {30, 30, 3}, {25, 25, 3}, {15, 15, 2}}, // pokemon 3
			{{10, 0, 2}, {35, 35, 3}, {20, 20, 2}, {25, 30, 4}}, // pokemon 4
			{{10, 0, 2}, {35, 30, 4}, {25, 30, 2}, {30, 35, 3}} // pokemon 5
	};
	return skill[pokemonNum][skillNum][type];
}

/*
 * modelValue(&pokemonA, 4, 3);
   modelValue(&pokemonB, 3, 4);
   modelValue(&pokemonC, 6, 1);
   modelValue(&pokemonD, 5, 2);
   modelValue(&pokemonE, 2, 5);
   modelValue(&pokemonF, 2, 6);
 * pokemon->hp = 60 + pokemonModel.strength * 10;
 * pokemon->mp = 60 + pokemonModel.intelligence * 10;
*/
// prop: 0 -> hp, 1 -> mp
int8_t pokemonProp (int8_t pokemonNum, int8_t prop) {
	int8_t pokemonProperty[6][2] = {
			{100, 90},
			{hpmp(3), hpmp(4)},
			{hpmp(6), hpmp(1)},
			{hpmp(5), hpmp(2)},
			{hpmp(2), hpmp(5)},
			{hpmp(2), hpmp(6)}
	};
	return pokemonProperty[pokemonNum][prop];
}

int8_t hpmp (int8_t factor) {
	return 60 + factor * 10;
}

int8_t items(int8_t type, int8_t prop) {
	char item[2][2] = {
					  {2, 50}, // tango
 					  {2, 50}  // mango
					  };
	return item[type][prop];
}

/*
typedef struct {
	char* name;
	int currentMon;
	int mon[3];
	int tg[2];
	int mg[2];
} player;
*/

void initPlayer(player* plr, int8_t mon0, int8_t mon1, int8_t mon2) {
	plr->mon[0] = mon0;
	plr->mon[1] = mon1;
	plr->mon[2] = mon2;
	plr->currentMon = 0;
	memAlloc(plr);
}


// damage located at an address of currentMon * 20 + skillKey * 4 + 1
// cost located at an address of currentMon * 20 + skillKey * 4 + 2
// accuracy located at an address of currentMon * 20 + skillKey * 4 + 3
// pokemon No. located at an address of currentMon * 20 + 16
// pokemon's hp located at an address of currentMon * 20 + 17
// pokemon's mp located at an address of currentMon * 20 + 18
void memAlloc(player* plr) {
	uint8_t i, a;
	// pokemon attributes memory allocation
	for(i = 0; i < 3; i++) {
		// skill memory allocation
		for(a = 0; a < 4; a++) {
			setValue(plr, i * 18 + a * 4, 0); // pokemon No.
			setValue(plr, i * 18 + a * 4 + 1, pokemonSkill(plr->mon[i], a, 0)); // damage
			setValue(plr, i * 18 + a * 4 + 2, pokemonSkill(plr->mon[i], a, 1)); // cost
			setValue(plr, i * 18 + a * 4 + 3, pokemonSkill(plr->mon[i], a, 2)); // critical
		}
		// tango memory allocation

		setValue(plr, i * 18 + 16, pokemonProp(plr->mon[i], 0)); // hp
		setValue(plr, i * 18 + 17, pokemonProp(plr->mon[i], 1)); // mp
	}
	setValue(plr, 54, plr->currentMon); // current pokemon memory

	// tango
	setValue(plr, 55, items(0, 0)); // amount
	setValue(plr, 56, items(0, 1)); // heal


	// mango
	setValue(plr, 57, items(1, 0)); // amount
	setValue(plr, 58, items(1, 1)); // heal
}
