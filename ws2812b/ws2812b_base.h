/*
 * ws2812b_base.h
 *
 *  Created on: Nov 15, 2021
 *      Author: M.Gehrmann
 */

#ifndef WS2812B_BASE_H_
#define WS2812B_BASE_H_

#include <stdint.h>

typedef struct _ws2812bLed_ {
	uint8_t Green;
	uint8_t Red;
	uint8_t Blue;
} ws2812bLed_t;

	void ws2812b_init(void);
	void ws2812b_main(void);

	void ws2812b_transferFrame(ws2812bLed_t ledFrm[], uint16_t frameEntries, uint16_t numOfRepeat);





#endif /* WS2812B_BASE_H_ */
