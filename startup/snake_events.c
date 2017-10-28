//******************************************************************************
//	snake_events.c
//
//  Author:			Paul Roper, Brigham Young University
//  Revisions:		1.0		11/25/2012	RBX430-1
//
//******************************************************************************
//
#include "msp430.h"
#include <stdlib.h>
#include "RBX430-1.h"
#include "RBX430_lcd.h"
#include "snake.h"

extern volatile uint16 sys_event;			// pending events
extern volatile uint16 move_cnt;			// snake speed

extern volatile uint8 game_mode;			// 0=idle, 1=game in progress
extern volatile uint8 score;				// current score
extern volatile uint8 level;				// current level (1-4)
extern volatile uint8 direction;			// current move direction

extern volatile uint8 head;					// head index into snake array
extern volatile uint8 tail;					// tail index into snake array
extern SNAKE snake[MAX_SNAKE];				// snake segments

extern const uint16 snake_text_image[];
extern const uint16 snake1_image[];

static uint8 move_right(SNAKE* head);		// move snake head right
static uint8 move_up(SNAKE* head);			// move snake head up
static uint8 move_left(SNAKE* head);		// move snake head left
static uint8 move_down(SNAKE* head);		// move snake head down
static void new_snake(uint16 length, uint8 dir);


//-- switch #1 event -----------------------------------------------------------
//
void SWITCH_1_event(void)
{
	if (game_mode == 0)
	{
		level = 1;						// start game @level 1
		move_cnt = WDT_MOVE1;
		sys_event |= START_GAME;
	}
	else
	{
		if (direction != LEFT)			// if not moving left, move right
		{
			if (snake[head].point.x < X_MAX) direction = RIGHT;
		}
	}
	return;
} // end SWITCH_1_event


//-- delete_tail  --------------------------------------------------------------
//
void delete_tail(void)
{
	lcd_point(COL(snake[tail].point.x), ROW(snake[tail].point.y), PENTX_OFF);
	tail = (tail + 1) & (~MAX_SNAKE);
} // end delete_tail


//-- add_head  -----------------------------------------------------------------
//
void add_head(void)
{
	static uint8 (*mFuncs[])(SNAKE*) =	// move head function pointers
	             { move_right, move_up, move_left, move_down };
	uint8 new_head = (head + 1) & (~MAX_SNAKE);
	snake[new_head] = snake[head];		// set new head to previous head
	head = new_head;
	// iterate until valid move
	while ((*mFuncs[direction])(&snake[head]));
} // end add_head


//-- move snake event ----------------------------------------------------------
//
void MOVE_SNAKE_event(void)
{
	if (level > 0)
	{
		add_head();						// add head
		lcd_point(COL(snake[head].point.x), ROW(snake[head].point.y), PENTX);
		delete_tail();					// delete tail
	}
	return;
} // end MOVE_SNAKE_event

/*
void draw_square_food(FOOD* food, uint8 pen)
{
	lcd_square(COL(food->food.point.x), ROW(food->food.point.y), food->size, pen);
}
*/


//-- start game event -----------------------------------------------------------
//
void START_GAME_event(void)
{
	game_mode = 1;						// start game
	level = 2;							// ***demo only***
	move_cnt = WDT_MOVE2;				// level 2, speed 2
	return;
} // end NEW_GAME_event


//-- new game event ------------------------------------------------------------
//
#define START_SCORE	10

void NEW_GAME_event(void)
{
	lcd_clear();						// clear lcd
	lcd_backlight(1);					// turn on backlight
	lcd_wordImage(snake1_image, (159-60)/2, 60, 1);
	lcd_wordImage(snake_text_image, (159-111)/2, 20, 1);

	new_snake(START_SCORE, RIGHT);
	sys_event = START_GAME;
	return;
} // end NEW_GAME_event


//-- new snake -----------------------------------------------------------------
//
void new_snake(uint16 length, uint8 dir)
{
	int i;
	head = 0;
	tail = 0;
	snake[head].point.x = 0;
	snake[head].point.y = 0;
	direction = dir;

	// build snake
	score = length;
	for (i = score - 1; i > 0; --i)
	{
		add_head();
	}
	return;
} // end new_snake


//-- move snake head right -----------------------------------------------------
//
uint8 move_right(SNAKE* head)
{
	if ((head->point.x + 1) < X_MAX)		// room to move right?
	{
		++(head->point.x);					// y, move right
		return FALSE;
	}
	if (level != 2)							// n, right fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.x = 0;					// wrap around
		return FALSE;
	}
	if (head->point.y) direction = DOWN;	// move up/down
	else direction = UP;
	return TRUE;
} // end move_right


//-- move snake head up --------------------------------------------------------
//
uint8 move_up(SNAKE* head)
{
	if ((head->point.y + 1) < Y_MAX)
	{
		++(head->point.y);					// move up
		return FALSE;
	}
	if (level != 2)							// top fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.y = 0;					// wrap around
		return FALSE;
	}
	if (head->point.x) direction = LEFT;	// move left/right
	else direction = RIGHT;
	return TRUE;
} // end move_up


//-- move snake head left ------------------------------------------------------
//
uint8 move_left(SNAKE* head)
{
	if (head->point.x)
	{
		--(head->point.x);					// move left
		return FALSE;
	}
	if (level != 2)							// left fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.x = X_MAX - 1;			// wrap around
		return FALSE;
	}
	if (head->point.y) direction = DOWN;	// move down/up
	else direction = UP;
	return TRUE;
} // end move_left


//-- move snake head down ------------------------------------------------------
//
uint8 move_down(SNAKE* head)
{
	if (head->point.y)
	{
		--(head->point.y);					// move down
		return FALSE;
	}
	if (level != 2)							// bottom fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.y = Y_MAX - 1;			// wrap around
		return FALSE;
	}
	if (head->point.x) direction = LEFT;	// move left/right
	else direction = RIGHT;
	return TRUE;
} // end move_down
