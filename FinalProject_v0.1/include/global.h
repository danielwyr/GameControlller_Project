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
#include "stm32f769i_discovery_sd.h"
#include "stm32f7xx_ll_sdmmc.h"
#include "stm32f769i_discovery_qspi.h"
#include "stm32f769i_discovery_ts.h"
#include "stm32f7xx_it.h"

#include "icon.h"
#include "main.h"



using namespace std;

#ifdef __cplusplus
extern "C"
#endif


typedef enum {
	HEART,
	CONTROLLER,
	HEART_RATE,
	TV_REMOTE,
	TV_VOLUME,
	TV_CHANNEL,
	TV_SOURCE,
	TV_POWER,
	POKEMON,
	BACK_BOTTON,
	PIKACHU_B,
	PIKACHU_F,
	BULBASAUR_B,
	BULBASAUR_F,
	CHARMANDER_B,
	CHARMANDER_F,
	EEVEE_B,
	EEVEE_F,
	SQUIRTLE_B,
	SQUIRTLE_F,
	ZUBAT_B,
	ZUBAT_F,
	BPM,
	HEART_ICON,
	HEART_RATE_LOGO,
	POKE_BALL,
	NON
} PICS;


typedef enum {
	Pikachu,
	Squirtle,
	Charmander,
	Bulbasaur,
	Evee,
	Zubat,
	NON_P
} Pokemon;




typedef enum {
	MAIN_MENU,
	TV_CONTROL_MENU,
	HEART_RATE_MENU,
	GAME_DEMO_MENU,
	CONTROL_DEMO_MENU
} UI_State;


typedef enum {
	Up,
	Down,
	Left,
	Right,
	Up_Right,
	Up_Left,
	Down_Right,
	Down_Left,
	Center
} JS_State;

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


#define CHUNK_SIZE			(512 * 800 * 2 * 4)
#define VFRAM_SIZE			4
#define NUM_OF_BLOCK 		0x1000000
#define BLOCK_START_ADDR	0
#define BLOCK_CLUSTER		1600
#define TIMEOUT				0x00000008

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
void MPU_Config(void);

void SD_init(SD_HandleTypeDef*);
bool SD_isDetected(void);

bool QSPI_Write(uint32_t*, uint32_t, uint32_t);
bool QSPI_Read(uint32_t*, uint32_t, uint32_t);
void LCD_Config(void);

void GPIO_Config(void);
void GPIO_init(GPIO_TypeDef*, GPIO_InitTypeDef*, uint32_t);

void UART_Config(void);
void PB_Config(void);
void ADC_Config(void);

void trackpad_Config(void);
void get_track_pad_state(void);
void Gesture_demo(Cursor&);
void LCD_Texture_config(void);
uint32_t getBitFromPic(PICS pic, int x, int y);
uint16_t getXSizeFromPic(PICS pic);
uint16_t getYSizeFromPic(PICS pic);

void drawBitMap(PICS pic, uint16_t Xpos, uint16_t Ypos);
void drawLayout(void);
void touchScreenTest(void);
void clickDefaultBackButton(uint16_t X, uint16_t Y);
void drawDefaultBackButton(void);
void drawBackButton(uint16_t X, uint16_t Y);
void restore_dir(JS_State jss);
void mark_dir(JS_State jss);



#define PLAYER_1 0
#define PLAYER_2 1
#define START_GAME 0

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

JS_State dir[3][3] = {
		{Down_Left, Down, Down_Right},
		{Left, Center, Right},
		{Up_Left, Up, Up_Right}
};

#define PLR_MP_FRM_X	418
#define PLR_MP_FRM_Y	297
#define PLR_MP_CTT_X	(PLR_MP_FRM_X + 1)
#define PLR_MP_CTT_Y	(PLR_MP_FRM_Y + 1)
#define PLR_HP_FRM_X	418
#define PLR_HP_FRM_Y	275
#define PLR_HP_CTT_X	(PLR_HP_FRM_X + 1)
#define PLR_HP_CTT_Y	(PLR_HP_FRM_Y + 1)
#define ENM_HP_FRM_X	18
#define ENM_HP_FRM_Y	18
#define ENM_HP_CTT_X	(ENM_HP_FRM_X + 1)
#define ENM_HP_CTT_Y	(ENM_HP_FRM_Y + 1)
#define HP_FRM_WIDTH	362
#define HP_BAR_WIDTH	(HP_FRM_WIDTH - 2)
#define HP_FRM_HEIGHT	22
#define HP_BAR_HEIGHT	(HP_FRM_HEIGHT - 2)
#define MP_FRM_WIDTH	HP_FRM_WIDTH
#define MP_BAR_WIDTH	HP_BAR_WIDTH
#define MP_FRM_HEIGHT	HP_FRM_HEIGHT
#define MP_BAR_HEIGHT	HP_BAR_HEIGHT

#define BALL_SIZE		20
#define PLR_BALL_Y		(PLR_HP_FRM_Y - BALL_SIZE - 10)
#define PLR_BALL_X1		(PLR_HP_FRM_X + 34)
#define PLR_BALL_X2		(PLR_BALL_X1 + 53)
#define PLR_BALL_X3		(PLR_BALL_X2 + 53)

#define ENM_BALL_Y		(ENM_HP_FRM_Y + HP_FRM_HEIGHT + 10)
#define ENM_BALL_X1		(ENM_HP_FRM_X + 34)
#define ENM_BALL_X2		(ENM_BALL_X1 + 53)
#define ENM_BALL_X3		(ENM_BALL_X2 + 53)

#endif
