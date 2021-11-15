/*
 * ws2812b_base.c
 *
 *  Created on: Nov 15, 2021
 *      Author: termy
 */

/* Periode of the Timer 48MHz*1,25us = 60 */

#include "main.h"

#define TIMER_PERIIODE 60

// WS2812 framebuffer - buffer for 2 LEDs - two times 24 bits
uint8_t dma2PwmBitBuffer[24 * 2] = {0};
#define BUFFER_SIZE		(sizeof(dma_bit_buffer)/sizeof(uint8_t))

static const uint8_t PulseLogic0 = (10 * TIMER_PERIIODE) / 31; /* 0,403 us */
static const uint8_t PulseLogic1 = (10 * TIMER_PERIIODE) / 15; /* 0,833 us */


void dmaTransferComplete_Callback(void);
void dmaTransferHalfComplete_Callback(void);

void ws2812b_init (void)
{
	/* DMA Init */

	HAL_DMA_RegisterCallback(hdma_tim3_ch4_up, HAL_DMA_XFER_CPLT_CB_ID , &dmaTransferComplete_Callback);
	HAL_DMA_RegisterCallback(hdma_tim3_ch4_up, HAL_DMA_XFER_HALFCPLT_CB_ID, &dmaTransferComplete_Callback);

	HAL_DMA_Start_IT(&hdma_tim3_ch4_up, (uint32_t)dma2PwmBitBuffer, (uint32_t)&htim3.Instance->CCR4, BUFFER_SIZE);
}

void dmaTransferComplete_Callback(void)
{

}

void dmaTransferHalfComplete_Callback(void)
{

}

