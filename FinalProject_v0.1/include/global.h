#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_flash_ex.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery_ts.h"
#include "stm32f7xx_it.h"

#include "icon.h"



using namespace std;

#ifdef __cplusplus
extern "C"
#endif

typedef enum {
	HEART,
	CONTROLLER,
	HEART_RATE,
	TV_REMOTE,
	POKEMON,
	NON
} PICS;

class Cursor {
public:
	int16_t x_;
	int16_t y_;
	uint16_t rad_;

	Cursor(const int16_t x, const int16_t y, const uint16_t rad);
	void CopyFrom(const Cursor &copy);
	void SetLocation(const int16_t x, const int16_t y);
	void moveUp(void);
	void moveDown(void);
	void moveLeft(void);
	void moveRight(void);
private:
	Cursor(const Cursor &copy);
	Cursor &operator=(const Cursor &rhs);
};

class Widget {
public:
	uint16_t x1_;
	uint16_t y1_;
	uint16_t width_;
	uint16_t height_;
	PICS btmp_;

	Widget(const uint16_t x1, const uint16_t y1, const uint16_t width, const uint16_t height, const PICS btmp);
	Widget(const Widget &copy);
	Widget & operator=(const Widget &rhs) {
		if(this != &rhs) {
			/*
			for(uint16_t i = 0; i < width_; i++) {
				free(btmp_[i]);
			}
			free(btmp_);*/
			btmp_ = rhs.btmp_;
			x1_ = rhs.x1_;
			y1_ = rhs.y1_;
			width_ = rhs.width_;
			height_ = rhs.height_;
		}
		return *this;
	}
	void onCreate(void);
	void onClick(void);
	void draw(void);
	void clear(uint32_t backcolor);
	void onDestroy(void);
};

class Activity {
public:
	uint32_t back_color_;
	uint32_t** bitmap_;
	int** allocator_;

	Activity* pre_;
	vector<Widget*> widgets;
	map<Widget*, int> index_map;

	Activity(Activity*);
	bool addWidget(Widget*);
	void OnDestroy(void);
	void SetBackColor(uint32_t);
private:
	Activity(const Activity &copy);
	Activity &operator=(const Activity &rhs);
};

class UI {
public:
	static UI* instance() {
		if(!instance_) instance_ = new UI();

		return instance_;
	}
	Activity* getCurUI(void) {
		return this->curUI_;
	}
	void setCurUI(Activity* next);
private:
	Activity* curUI_;
	static UI* instance_;
	UI(void) {
		curUI_ = NULL;
	}
};

class BackArrow : public Widget {
public:
	Activity *pre_;
	Activity *cur_;
	BackArrow(Activity*, Activity*);
	void onClick(void);
};

class HeartDemo : public Widget {
public:
	HeartDemo(const uint16_t, const uint16_t);
};

class MainActivity : public Activity {
	friend class Activity;
public:
	void onCreate(void);
	MainActivity(void);
	bool addWidget(Widget*);
};

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
void Gesture_demo(Cursor&);
void LCD_Texture_config(void);
uint32_t getBitFromPic(PICS pic, int x, int y);

UI* UI::instance_ = 0;

TRACK_PAD_State track_pad_state;

GPIO_InitTypeDef d0_init;
GPIO_InitTypeDef d1_init;
GPIO_InitTypeDef d2_init;
GPIO_InitTypeDef d3_init;
GPIO_InitTypeDef d4_init;

#define UP 			0x01
#define DOWN		0x02
#define LEFT		0x03
#define RIGHT		0x04
#define TAP			0x05
#define NT			0x06

#define RADIUS		10
#define BACK_COLOR	LCD_COLOR_WHITE

#endif
