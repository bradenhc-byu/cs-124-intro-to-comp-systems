//******************************************************************************
//  Lab 10c - Snake
//
//  Description:
//
//	"Write a snake game program in C that scores points by eating randomly
//	placed food on the display. There are four levels of play, each with
//	increasing difficulty. The game ends when the snake runs into an obstacle,
//	itself, or time expires. The direction of the snake�s head is turned to
//	horizontal and vertical paths using the push buttons and the body of the
//	snake follows in the head's path. The snake is always moving and the
//	snake moves faster at each level. The tail grows by one segment every
//	time the snake eats a food."
//
//
//  Author:			Paul Roper, Brigham Young University
//  Revisions:		1.0		11/25/2012		RBX430-1
//
//  Built with Code Composer Studio Version: 5.2.0.00090
//******************************************************************************
//******************************************************************************
//	Levels:

//	Level 1
//		"Cyclical" play area (enter a wall and appear on opposite side).
//		10 foods each worth 1 point.
//		Always 10 foods - reappear in random position as eaten.
//		Slow speed (250 ms/move).
//		Add some rocks to make the level more interesting.
//		30 second time limit to eat 10 foods.
//
//	Level 2
//		4 non-electric perimeter walls - snake makes a 90 degree direction change.
//		10 foods appear at beginning of level but disappear as eaten.
//		Each food eaten is worth 2 points.
//		Medium speed (125 ms/move).
//		30 second time limit to eat all 10 foods.
//
//	Level 3
//		4 electric perimeter walls - snake dies if the head runs into a wall.
//		1 food at a time appears, each is worth 3 points when eaten.
//		Medium fast speed (62 ms/move).
//		45 second time limit to eat 10 foods.
//
//	Level 4
//		4 electric perimeter walls - snake dies if the head runs into a wall.
//		Electric obstacles (rocks) are randomly placed on display.
//		1 food at a time appears, each is worth 4 points when eaten.
//		Fast speed (31 ms/move).
//		1 minute time limit to eat 10 foods.
//
//	King Snake!
//		If a player completes the 4th level, they are crowned the grand
//		champion having achieved the maximum score of 100 points!
//
#include "msp430.h"
#include <stdlib.h>
#include "RBX430-1.h"
#include "RBX430_lcd.h"
#include "snake.h"

volatile uint16 WDT_cps_cnt;			// WDT count per second
volatile uint16 WDT_move_cnt;			// counter to move snake event
volatile uint16 WDT_debounce_cnt;		// switch debounce counter
volatile uint32 WDT_delay;				// WDT delay counter

volatile uint16 sys_event;				// pending events
volatile uint16 move_cnt;				// snake speed

volatile uint8 game_mode;				// 0=idle, 1=game in progress
volatile uint8 score;					// current score
volatile uint8 level;					// current level (1-4)
volatile uint8 direction;				// current move direction

volatile uint8 head;					// head index into snake array
volatile uint8 tail;					// tail index into snake array
SNAKE snake[MAX_SNAKE];					// snake segments

void port1_init(void);					// initialize switches
void watchdogtimer_init(uint16);		// initialize watchdog

//------------------------------------------------------------------------------
//-- main ----------------------------------------------------------------------
//
void main(void)
{
	ERROR2(RBX430_init(CLOCK));			// init RBX430-1 board
	ERROR2(lcd_init());					// init LCD & interrupts
	port1_init();						// init port 1 (switches)
	watchdogtimer_init(WDT_CTL);		// init watchdog timer
	game_mode = 0;						// idle mode
	sys_event = NEW_GAME;				// new game

	//--------------------------------------------------------------------------
	//	event service routine loop
	//
	while (1)
	{
		// disable interrupts before check sys_event
		_disable_interrupts();

		if (sys_event)
		{
			// if there's something pending, enable interrupts before servicing
			_enable_interrupt();
		}
		else
		{
			// otherwise, enable interrupts and goto sleep (LPM0)
			__bis_SR_register(LPM0_bits + GIE);
		}

		//----------------------------------------------------------------------
		//	I'm AWAKE!!!  There must be something to do
		//----------------------------------------------------------------------

		if (sys_event & MOVE_SNAKE)				// move snake event
		{
			sys_event &= ~MOVE_SNAKE;			// clear move event
			MOVE_SNAKE_event();					// move snake
		}

		else if (sys_event & SWITCH_1)			// switch #1 event
		{
			sys_event &= ~SWITCH_1;				// clear switch #1 event
			SWITCH_1_event();					// process switch #1 event
		}
		else if (sys_event & START_GAME)		// start game event
		{
			sys_event &= ~START_GAME;			// clear start game event
			START_GAME_event();					// start game
		}
		else if (sys_event & NEW_GAME)			// new game event
		{
			sys_event &= ~NEW_GAME;				// clear new game event
			NEW_GAME_event();					// new game
		}
		else									// ????
		{
			ERROR2(10);							// unrecognized event
		}
	}
} // end main


//-- port1_init ----------------------------------------------------------------
//
void port1_init(void)
{
	// configure P1 switches and ADXL345 INT1 for interrupt
	P1SEL &= ~0x0f;						// select GPIO
	P1DIR &= ~0x0f;						// Configure P1.0-3 as Inputs
	P1OUT |= 0x0f;						// use pull-ups
	P1IES |= 0x0f;						// high to low transition
	P1REN |= 0x0f;						// Enable pull-ups
	P1IE |= 0x0f;						// P1.0-3 interrupt enabled
	P1IFG &= ~0x0f;						// P1.0-3 IFG cleared
	return;
} // end port1_init


//-- watchdogtimer_init --------------------------------------------------------
//
void watchdogtimer_init(uint16 wd_ctl)
{
	WDTCTL = wd_ctl;					// set Watchdog interval
	IE1 |= WDTIE;						// enable WDT interrupt
	WDT_cps_cnt = WDT_CPS;				// set WD 1 second counter
	WDT_move_cnt = WDT_MOVE1;			// move update rate
	WDT_debounce_cnt = 0;				// switch debounce
	WDT_delay = 0;						// init time counter
	return;
} // end watchdogtimer_init
