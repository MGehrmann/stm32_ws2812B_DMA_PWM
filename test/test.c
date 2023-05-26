/*
 * test.c
 *
 *  Created on: 24.11.2021
 *      Author: M.Gehrmann
 */

#include "ws2812b_base.h"
#include <stdbool.h>
#include "main.h"


typedef struct _SimpleTimer_ms_
{
	uint32_t startTick;
	uint32_t delay;
}SimpleTimer_ms_t;

static ws2812bLed_t LedFrame[300] = {
//		[0].Red = 100,
//		[0].Green = 0,
//		[0].Blue = 0,
//
//		[1].Red = 0,
//		[1].Green = 100,
//		[1].Blue = 0,
//
//		[2].Red = 0,
//		[2].Green = 0,
//		[2].Blue = 100,
//
//		[3].Red = 100,
//		[3].Green = 100,
//		[3].Blue = 0,
//
//		[4].Red = 0,
//		[4].Green = 100,
//		[4].Blue = 100,
//
//		[5].Red = 100,
//		[5].Green = 0,
//		[5].Blue = 100,
};

static const uint16_t c_frameSize = sizeof(LedFrame)/sizeof(LedFrame[0]);

static bool transferNewFrame = true;
static uint16_t DelayFrame_ms = 26;

void clearFrame(void);
void startTimer(SimpleTimer_ms_t *timer, uint32_t delay);
bool IsTimerExpired(SimpleTimer_ms_t *timer);

void clearFrame(void)
{
	for (uint16_t i = 0; i< c_frameSize; ++i)
	{
		LedFrame[i].Red = 0;
		LedFrame[i].Green = 0;
		LedFrame[i].Blue = 0;
	}
}

void startTimer(SimpleTimer_ms_t *timer, uint32_t delay)
{
	timer->startTick = HAL_GetTick();
	timer->delay = delay;

}

bool IsTimerExpired(SimpleTimer_ms_t *timer)
{
	if ((HAL_GetTick() - timer->startTick) < timer->delay)
	{
		return (false);
	}
	else
	{
		return (true);
	}
}


void SimpleExample1(void)
{
	static uint8_t StepCnt = 0;
	static uint16_t exampleFrameSize = 20;

	if (transferNewFrame)
	{
		clearFrame();
		LedFrame[StepCnt].Red = 100;
		uint16_t tempGreenDot = StepCnt+3;
		while (tempGreenDot >= exampleFrameSize)
		{
			tempGreenDot -= exampleFrameSize;
		}
		LedFrame[tempGreenDot].Green = 100;


		ws2812b_transferFrame(LedFrame, exampleFrameSize, 20);


		StepCnt++;

		if (StepCnt >= exampleFrameSize)
		{
			StepCnt = 0;
		}
	}
	HAL_Delay(DelayFrame_ms);
}

static uint16_t mg_timeDelayDotRed = 100;
static uint16_t mg_timeDelayDotGreen = 38;

void SimpleExample2(void)
{
	static bool init = true;

	static uint16_t exampleFrameSize = 15;

	static SimpleTimer_ms_t TimerRed;
	static SimpleTimer_ms_t TimerGreen;

	static uint8_t PosDotGreen = 0;
	static uint8_t PosDotRed = 0;

	if (init)
	{
		startTimer(&TimerRed, mg_timeDelayDotRed);
		startTimer(&TimerGreen, mg_timeDelayDotGreen);
		init = false;
	}

	if (IsTimerExpired(&TimerRed))
	{
		PosDotRed++;
		startTimer(&TimerRed, mg_timeDelayDotRed);
	}

	if (PosDotRed >= exampleFrameSize)
	{
		PosDotRed = 0;
	}

	if (IsTimerExpired(&TimerGreen))
	{
		PosDotGreen++;
		startTimer(&TimerGreen, mg_timeDelayDotGreen);
	}

	if (PosDotGreen >= exampleFrameSize)
	{
		PosDotGreen = 0;
	}


	clearFrame();
	LedFrame[PosDotRed].Red = 150;
	LedFrame[PosDotGreen].Green = 75;

	ws2812b_transferFrame(LedFrame, exampleFrameSize, 40);

	HAL_Delay(20);
}

void SimpleExample3(void)
{

static uint16_t numOfPixel  =  300;   /* the number of WS2812B LEDs in our strand */
static uint16_t exampleDelayTime = 41;


	static uint8_t rgb[3];  /* declare an array of 8-bit integers to hold our color values */

		rgb[0] = 255;
		rgb[1] = 0;
		rgb[2] = 0;

		/*
		 * loop through each combination of
		 *     RED   /  GREEN    (dC = 0, iC = 1)
		 *     GREEN /  BLUE     (dC = 1, iC = 2)
		 *     BLUE  /  RED      (dC = 2, iC = 0)
		 */
		for (int dC = 0; dC < 3; dC++)
		{

			/*
			 * make the incrementing color (iC) be one more than
			 * the decrementing color (dC) or, if the iC is 2 (blue),
			 * then the dC should wrap around to 0 (red).
			 */
			int iC = dC == 2 ? 0 : dC + 1;

			/*
			 * now that our incrementing and decrementing colors have
			 * been identified, increment and decrement respectively
			 * each one by a value of 1 for 255 times (the range of
			 * possible values for each color)
			 */
			for (int i = 0; i < 255; i++)
			{

				rgb[dC]--;
				rgb[iC]++;

				LedFrame[0].Red = rgb[0];
				LedFrame[0].Green = rgb[1];
				LedFrame[0].Blue = rgb[2];

				ws2812b_transferFrame(LedFrame, 1, numOfPixel);

				HAL_Delay(exampleDelayTime);

			}
		}

	}

void SimpleExample4(void)
{

	static uint16_t numOfPixel = 100; /* the number of LEDs for the effect */
	static uint16_t numOfRepeat = 1;
	static uint16_t exampleDelayTime = 41;
	static bool initBuffer = true;
	static uint8_t rgb[3]; /* declare an array of 8-bit integers to hold our color values */
	
	if (initBuffer) /* init, fill the buffer with the rainbow values */
	{
		uint16_t bufferPos = 0;
		float stepSize = (255*3)/numOfPixel;
		rgb[0] = 255;
		rgb[1] = 0;
		rgb[2] = 0;
		for (int dC = 0; dC < 3; dC++)
		{
			int iC = dC == 2 ? 0 : dC + 1;
			for (int i = 0; i < 255; i++)
			{

				rgb[dC]--;
				rgb[iC]++;
				// todo: check the function
				if (0);//if (((i*dC)%stepSize) == 0)
				{
					/* set the Values to Frame */
					LedFrame[bufferPos].Red = rgb[0];
					LedFrame[bufferPos].Green = rgb[1];
					LedFrame[bufferPos].Blue = rgb[2];
					bufferPos++;
				}
			}
		}
		initBuffer = false;
	}
	else /* normal operation, shift one LED */
	{
		ws2812bLed_t tempFirstLed;
		tempFirstLed = LedFrame[0];
		for (uint16_t i = 0; i < numOfPixel-1; ++i)
		{
			LedFrame[i]=LedFrame[i+1];
		}
		LedFrame[numOfPixel-1] = tempFirstLed;

	}
	
	ws2812b_transferFrame(LedFrame, numOfPixel, numOfRepeat);
	HAL_Delay(exampleDelayTime);

}

void test_main(void)
{
	static bool initTestMain = true;
	if (initTestMain)
	{
		clearFrame();
		ws2812b_transferFrame(LedFrame, c_frameSize, 1);
		initTestMain = false;
	}
	
	
	SimpleExample2();





}
