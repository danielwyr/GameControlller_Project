#include "global.h"

static TS_StateTypeDef* ts_state;
__IO uint32_t SDWriteStatus = 0, SDReadStatus = 0;
#define WRITE_READ_ADDR     ((uint32_t)0x0050)
//extern SD_HandleTypeDef uSdHandle;

UI_State ui_state;

int main(void) {
	system_init();
	GPIO_Config();
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetXSize());
	BSP_TS_ITConfig();
	LCD_Config();
	LCD_Texture_config();

	ui_state = MAIN_MENU;
	drawLayout();



	/*
	uint8_t SD_state = MSD_OK;
	SD_state = BSP_SD_Init();
	if (SD_state != MSD_OK) {
		while(1);
	}
	BSP_SD_ITConfig();

	uSdHandle.SdCard.BlockNbr = 0x40000000; // 1GB
	uSdHandle.SdCard.LogBlockNbr = 0x40000000;

	HAL_SD_CardInfoTypeDef card_info;
	BSP_SD_GetCardInfo(&card_info);
	// 8M --> 0x400000
	SD_state = BSP_SD_Erase(BLOCK_START_ADDR, 0x40000000);
	while(BSP_SD_GetCardState() != SD_TRANSFER_OK);
	if (SD_state != MSD_OK) while(1);
	// fill buffer

	//for(int j = 0; j < 800; j++) {
		for(uint32_t i = 0; i < size; i++) {
			txbuffer[i] = i;
			rxbuffer[i] = 0;
		}
		//SCB_CleanInvalidateDCache();
		SCB_CleanDCache();
		SD_state = BSP_SD_WriteBlocks(txbuffer, BLOCK_START_ADDR, 4, timeout);
		while(1);
		while(!SDWriteStatus);
		SDWriteStatus = 0;

		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if (SD_state != MSD_OK) while(1);

		SD_state = BSP_SD_ReadBlocks_DMA(rxbuffer, BLOCK_START_ADDR, 4);
		while(!SDReadStatus);
		SDReadStatus = 0;

		while(BSP_SD_GetCardState() != SD_TRANSFER_OK);
		if (SD_state != MSD_OK) while(1);
	//}*/
		/*
	MainActivity* mainActivity = new MainActivity();
	mainActivity->onCreate();*/
	//Cursor crs((int16_t) BSP_LCD_GetXSize() / 2, (int16_t)  BSP_LCD_GetYSize() / 2, RADIUS);
	while(1) {
		//get_track_pad_state();
		//Gesture_demo(crs);
	}
}

void onClick(void) {
	uint32_t X = (uint32_t) ts_state->touchX[0], Y = (uint32_t)ts_state->touchY[0];
	switch(ui_state) {
	case MAIN_MENU:
	{
		if(X >= 59 && X <= 184 && Y >= 294 && Y <= 419) { // Heart rate
			ui_state = HEART_RATE_MENU;
			drawLayout();
		} else if(X >= 244 && X <= 369 && Y >= 294 && Y <= 419) { // Game Demo
			ui_state = GAME_DEMO_MENU;
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
		clickDefaultBackButton(X, Y);
		break;
	}
	case GAME_DEMO_MENU:
	{
		clickDefaultBackButton(X, Y);
		break;
	}
	case TV_CONTROL_MENU:
	{
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
		drawDefaultBackButton();
		break;
	}
	case GAME_DEMO_MENU: {
		drawDefaultBackButton();
		break;
	}
	case CONTROL_DEMO_MENU: {
		drawDefaultBackButton();
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

/**
  * @brief Tx Transfer completed callbacks
  * @param None
  * @retval None
  */
void BSP_SD_WriteCpltCallback(void)
{
	SDWriteStatus = 1;
}

/**
  * @brief Rx Transfer completed callbacks
  * @param None
  * @retval None
  */
void BSP_SD_ReadCpltCallback(void)
{
	SDReadStatus = 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == GPIO_PIN_13) {
		// touch Screen Handler
		ts_state->touchDetected = 1;
		if(BSP_TS_GetState(ts_state) == TS_OK) {
			onClick();
			//touchScreenTest();

		}
	}
}



void touchScreenTest(void) {
	uint32_t X = (uint32_t) ts_state->touchX[0], Y = (uint32_t)ts_state->touchY[0];

	//Activity* curUI = UI::instance()->getCurUI();
	//if(curUI != NULL) {
		//uint32_t buffer[NUM_OF_BLOCK];
		//uint32_t addr = curUI->allocator_ + X * BLOCKSIZE * VFRAM_SIZE;
		/*if(BSP_SDRAM_ReadData(addr, buffer, 1) != SDRAM_OK) {
			while(1);
		}*/
		/*
		SD_state = BSP_SD_ReadBlocks_DMA(buffer, addr, VFRAM_SIZE);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

		uint32_t widget_index = buffer[Y];
		if(widget_index != 0) {*/
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
			//curUI->widgets.at(widget_index - 1)->onClick();
		//}
	//}
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
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
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
    BSP_LED_On(Led_TypeDef::LED1);
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

/* -- Activity Class Functions -- */
Activity::Activity(Activity* pre) : pre_(pre) {
	back_color_ = LCD_COLOR_WHITE;
	BSP_LCD_SetBackColor(back_color_);
	MMU::instance()->memset((void*) this);
	bitmap_ = MMU::instance()->getBitMapIndex(this);
	allocator_ = MMU::instance()->getAllocatorIndex(this);
}

void Activity::onCreate(void) {
	uint32_t Y = BSP_LCD_GetYSize(), X = BSP_LCD_GetXSize();
	uint32_t buffer[BLOCKSIZE];
	uint8_t SD_state = MSD_OK;
	for(uint32_t i = 0; i < Y; i++) {
		buffer[i] = back_color_;
	}

	for(uint32_t i = bitmap_; i < bitmap_ + X * BLOCKSIZE * VFRAM_SIZE; i += BLOCKSIZE * VFRAM_SIZE) {
		SD_state = BSP_SD_WriteBlocks(buffer, i, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);
	}
	for(uint32_t i = 0; i < Y; i++) {
		buffer[i] = 0;
	}
	for(uint32_t i = allocator_; i < allocator_ + X * BLOCKSIZE * VFRAM_SIZE; i += BLOCKSIZE * VFRAM_SIZE) {
		SD_state = BSP_SD_WriteBlocks(buffer, i, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);
	}
	UI::instance()->setCurUI(this);
}

bool Activity::addWidget(Widget* widget) {
	uint32_t buffer[BLOCKSIZE];
	uint32_t offset = 0, base = allocator_ + widget->x1_ * BLOCKSIZE * VFRAM_SIZE;
	uint8_t SD_state = MSD_OK;

	if((widget->width_ == 0) | (widget->height_ == 0) | (widget->x1_ + widget->width_ >= BSP_LCD_GetXSize()) | (widget->y1_ + widget->height_ >= BSP_LCD_GetYSize())) {
		return false;
	}

	for(uint32_t i = 0; i < widget->width_; i++) {
		SD_state = BSP_SD_ReadBlocks(buffer, base + offset, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

		offset += BLOCKSIZE * VFRAM_SIZE;
		for(uint16_t j = widget->y1_; j < widget->y1_ + widget->height_; j++) {
			if(buffer[j] != 0) return false;
		}
	}
	this->widgets.push_back(widget);
	uint32_t alloc_buf[BLOCKSIZE];
	offset = 0;
	uint32_t bmbase = bitmap_ + widget->x1_ * BLOCKSIZE * VFRAM_SIZE;
	for(int i = 0; i < widget->width_; i++) {
		// fill buffer
		SD_state = BSP_SD_ReadBlocks(alloc_buf, base + offset, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

		for(int j = widget->y1_; j < widget->y1_ + widget->height_; j++) {
			uint32_t byte = getBitFromPic(widget->btmp_, i, j);
			if(byte < 0xFF000000) {
				buffer[j] = back_color_;
				alloc_buf[i] = 0;
			} else {
				buffer[j] = byte;
				alloc_buf[i] = widgets.size();
			}
		}
		SD_state = BSP_SD_WriteBlocks(alloc_buf, base + offset, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

		SD_state = BSP_SD_WriteBlocks(buffer, bmbase + offset, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);
		offset += BLOCKSIZE * VFRAM_SIZE;
	}
	//this->index_map[widget] = this->index_map.size();
	UI::instance()->setCurUI(this);
	return true;
}

void Activity::SetBackColor(uint32_t color) {
	/*this->back_color_ = color;
	uint32_t buffer[BSP_LCD_GetYSize()];
	uint32_t base = this->allocator_;
	for(uint16_t i = 0; i < BSP_LCD_GetXSize(); i++) {
		BSP_SDRAM_ReadData(base + i * BSP_LCD_GetYSize(), buffer, BSP_LCD_GetYSize());
		for(uint16_t j = 0; j < BSP_LCD_GetYSize(); j++) {
			if(allocator_[i][j] == -1) {
				BSP_LCD_DrawPixel(i, j, color);
			}
		}
	}*/
}

void Activity::OnDestroy(void) {
	MMU::instance()->removeAct(this);
}

/* -- Widget Class Functions -- */
Widget::Widget(const uint16_t x1, const uint16_t y1, const uint16_t width, const uint16_t height, const PICS btmp) : x1_(x1), y1_(y1), width_(width), height_(height), btmp_(btmp) {
	//this->btmp_ = (uint32_t**) calloc(1, sizeof(uint32_t*));
}


Widget::Widget(const Widget &copy) {
	this->x1_ = copy.x1_;
	this->y1_ = copy.y1_;
	this->width_ = copy.width_;
	this->height_ = copy.height_;
	this->btmp_ = copy.btmp_;
}


void Widget::onClick(void) {
	// empty function, user override
}

/*
void Widget::draw(void) {
	// empty function, user override
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(this->x1_, this->y1_, this->width_, this->height_);
	BSP_LCD_SetTextColor(LCD_COLOR_TRANSPARENT);
}

void Widget::clear(uint32_t backcolor) {
	for(uint16_t i = this->x1_; i < this->x1_ + this->width_; i++) {
		for(uint16_t j = this->y1_; j < this->y1_ + this->height_; j++) {
			BSP_LCD_DrawPixel(i, j, backcolor);
		}
	}
}*/

/* -- BackArrow Class Functions -- */
/*
BackArrow::BackArrow(Activity* pre, Activity* cur) : Widget(0, 0, 80, 80), pre_(pre), cur_(cur) {

}*/
// @Override
void BackArrow::onClick(void) {
	UI::instance()->setCurUI(cur_->pre_);
	cur_->OnDestroy();
}

/* -- UI Class Functions -- */
void UI::setCurUI(Activity* next) {
	this->curUI_ = next;
	uint32_t bitAddr = next->bitmap_, Y = BSP_LCD_GetYSize(), X = BSP_LCD_GetXSize();
	uint32_t buffer[BLOCKSIZE];
	uint8_t SD_state = MSD_OK;
	for(uint16_t i = 0; i < X; i++) {
		//BSP_SDRAM_ReadData(bitAddr, buffer, Y);
		SD_state = BSP_SD_ReadBlocks(buffer, bitAddr, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

		bitAddr += BLOCKSIZE * VFRAM_SIZE;
		for(uint16_t j = 0; j < Y; j++) {
			BSP_LCD_DrawPixel(i, j, buffer[j]);
		}
	}
}

/* -- HeartDemo Widget Class -- */

HeartDemo::HeartDemo(const uint16_t x1, const uint16_t y1)
		: Widget(x1, y1, 125, 125, HEART_RATE) {
}

/* -- MainActivity Class Functions -- */
MainActivity::MainActivity(void) : Activity((Activity*) 0) {

}

void MainActivity::onCreate(void) {
	Activity::onCreate();
	HeartDemo* ht = new HeartDemo(400, 240);
	this->addWidget((Widget*) ht);
}

bool MainActivity::addWidget(Widget* widget) {
	return Activity::addWidget(widget);
}

/* -- MMU Class Functions -- */
void MMU::removeAct(void* act) {
	int Y = BSP_LCD_GetYSize(), X = BSP_LCD_GetXSize();
	void* last = index_->at(max_);
	uint32_t curAddr = map_->at(act);
	uint32_t lastAddr = map_->at(last);
	uint32_t buffer[BLOCKSIZE];
	uint8_t SD_state = MSD_OK;

	for(uint32_t i = 0; i < CHUNK_SIZE; i += BLOCKSIZE) {
		/*if(BSP_SDRAM_ReadData(lastAddr + i, buffer, Y) != SDRAM_OK) {
			while(1);
		}
		if(BSP_SDRAM_WriteData(curAddr + i, buffer, Y) != SDRAM_OK) {
			while(1);
		}*/
		SD_state = BSP_SD_ReadBlocks(buffer, lastAddr + i, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

		SD_state = BSP_SD_WriteBlocks(buffer, curAddr + i, VFRAM_SIZE, TIMEOUT);
		while((BSP_SD_GetCardState() != SD_TRANSFER_OK));
		if(SD_state != MSD_OK) while(1);

	}
	map_->insert(pair<void*, uint32_t>(last, curAddr));
	index_->insert(pair<uint32_t, void*>(curAddr, last));
	map_->erase(act);
	index_->erase(lastAddr);
	max_ -= CHUNK_SIZE;
}

uint32_t MMU::getBitMapIndex(void* act) {
	return map_->at(act) - CHUNK_SIZE;
}

uint32_t MMU::getAllocatorIndex(void* act) {
	return map_->at(act) - CHUNK_SIZE / 2;
}
