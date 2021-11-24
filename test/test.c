/*
 * test.c
 *
 *  Created on: 24.11.2021
 *      Author: termy
 */

#include "ws2812b_base.h"
#include <stdbool.h>

static ws2812bLed_t LedFrame[] = {
		[0].Red = 100,
		[0].Green = 0,
		[0].Blue = 0,

		[1].Red = 0,
		[1].Green = 100,
		[1].Blue = 0,

		[2].Red = 0,
		[2].Green = 0,
		[2].Blue = 100,

		[3].Red = 100,
		[3].Green = 100,
		[3].Blue = 0,

		[4].Red = 0,
		[4].Green = 100,
		[4].Blue = 100,

		[5].Red = 100,
		[5].Green = 0,
		[5].Blue = 100,
};

static bool transferNewFrame = false;

void test_main(void)
{

	if (transferNewFrame)
	{
		ws2812b_transferFrame(LedFrame, 6, 2);
		transferNewFrame = false;
	}

}
