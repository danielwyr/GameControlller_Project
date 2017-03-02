#include "global.h"

static TS_StateTypeDef* ts_state;

int main(void) {
	system_init();
	LCD_Config();
	LCD_Texture_config();
	GPIO_Config();
	//BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetXSize());
	//BSP_TS_ITConfig();

	MainActivity* mainActivity = new MainActivity();
	mainActivity->onCreate();

	//Cursor crs((int16_t) BSP_LCD_GetXSize() / 2, (int16_t)  BSP_LCD_GetYSize() / 2, RADIUS);
	while(1) {
		//get_track_pad_state();
		//Gesture_demo(crs);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == GPIO_PIN_13) {
		// touch Screen Handler
		ts_state->touchDetected = 1;

		if(BSP_TS_GetState(ts_state) == TS_OK) {
			uint16_t X = ts_state->touchX[0], Y = ts_state->touchY[0];
			// TODO: check allocate screen

			Activity* curUI = UI::instance()->getCurUI();
			if(curUI != NULL) {
				uint32_t widget_index = curUI->allocator_[X][Y];
				curUI->widgets.at(widget_index)->onClick();
			}
			string s = "";
			char x_index[10];
			char y_index[10];
			itoa((int) X, x_index, 10);
			itoa((int) Y, y_index, 10);
			string X_index(x_index);
			string Y_index(y_index);


			s.append("X = ").append(X_index).append(" ").append("Y = ").append(Y_index);
			const char* cc = s.c_str();
			BSP_LCD_DisplayStringAtLine(1, (uint8_t*) cc);
		}
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
	HAL_NVIC_SetPriority((IRQn_Type) SysTick_IRQn, 0, 0);
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

uint32_t getBitFromPic(PICS pic, int x, int y) {
	switch(pic) {
	case HEART: return heart[x][y];
	case HEART_RATE: return heart_rate_icon[x][y];
	case TV_REMOTE: return tv_remote_icon[x][y];
	case CONTROLLER: return controller_icon[x][y];
	case POKEMON: return pokemon_icon[x][y];
	default: return 0;
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
	bitmap_ = (uint32_t **) calloc(BSP_LCD_GetXSize(), sizeof(uint32_t*));
	allocator_ = (int **) calloc(BSP_LCD_GetXSize(), sizeof(int*));
	for(uint32_t i = 0; i < BSP_LCD_GetXSize(); i++) {
		bitmap_[i] = (uint32_t *) calloc(BSP_LCD_GetYSize(), sizeof(uint32_t));
		if(bitmap_[i] == NULL) while(1);
		allocator_[i] = (int *) calloc(BSP_LCD_GetYSize(), sizeof(int));
		if(allocator_[i]) while(1);
	}
	for(uint16_t i = 0; i < BSP_LCD_GetXSize(); i++) {
		for(uint16_t j = 0; j < BSP_LCD_GetYSize(); j++) {
			BSP_LCD_DrawPixel(i, j, back_color_);
			bitmap_[i][j] = back_color_;
			allocator_[i][j] = -1;
		}
	}
}

bool Activity::addWidget(Widget* widget) {
	if((widget->width_ == 0) | (widget->height_ == 0) | (widget->x1_ + widget->width_ >= BSP_LCD_GetXSize()) | (widget->y1_ + widget->height_ >= BSP_LCD_GetYSize())) {
			return false;
	}
	for(uint16_t i = widget->x1_; i < widget->x1_ + widget->width_; i++) {
		for(uint16_t j = widget->y1_; j < widget->y1_ + widget->height_; j++) {
			if(this->allocator_[i][j] != -1) return false;
		}
	}
	for(int i = widget->x1_; i < widget->x1_ + widget->width_; i++) {
		for(int j = widget->y1_; j < widget->y1_ + widget->height_; j++) {
			this->allocator_[i][j] = this->widgets.size();
			int x = i - widget->x1_;
			int y = j - widget->y1_;
			if(x < 0 || x >= widget->width_ || y < 0 || y >= widget->height_) {
				while(1);
			}
			uint32_t bit = getBitFromPic(widget->btmp_, x, y);
			this->bitmap_[i][j] = bit;
		}
	}
	//this->index_map[widget] = this->index_map.size();
	this->widgets.push_back(widget);
	UI::instance()->setCurUI(this);
	return true;
}

void Activity::SetBackColor(uint32_t color) {
	this->back_color_ = color;
	for(uint16_t i = 0; i < BSP_LCD_GetXSize(); i++) {
		for(uint16_t j = 0; j < BSP_LCD_GetYSize(); j++) {
			if(allocator_[i][j] == -1) {
				BSP_LCD_DrawPixel(i, j, color);
			}
		}
	}
}

void Activity::OnDestroy(void) {
	for(uint32_t i = 0; i < BSP_LCD_GetYSize(); i++) {
		free(bitmap_[i]);
		free(allocator_[i]);
	}
	free(bitmap_);
	free(allocator_);
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
}

/* -- BackArrow Class Functions -- */
/*
BackArrow::BackArrow(Activity* pre, Activity* cur) : Widget(0, 0, 80, 80), pre_(pre), cur_(cur) {

}*/
// @Override
void BackArrow::onClick(void) {
	// TODO: update current UI
	UI::instance()->setCurUI(cur_->pre_);
	cur_->OnDestroy();
}

/* -- UI Class Functions -- */
void UI::setCurUI(Activity* next) {
	this->curUI_ = next;
	for(uint16_t i = 0; i < BSP_LCD_GetXSize(); i++) {
		for(uint16_t j = 0; j < BSP_LCD_GetYSize(); j++) {
			BSP_LCD_DrawPixel(i, j, this->curUI_->bitmap_[i][j]);
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
	HeartDemo* ht = new HeartDemo(40, 40);
	this->addWidget(ht);
}

bool MainActivity::addWidget(Widget* widget) {
	return Activity::addWidget(widget);
}


