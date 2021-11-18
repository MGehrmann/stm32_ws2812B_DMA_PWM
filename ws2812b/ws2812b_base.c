/*
 * ws2812b_base.c
 *
 *  Created on: Nov 15, 2021
 *      Author: termy
 */

/* Periode of the Timer 48MHz*1,25us = 60 */

#include "main.h"

#define TIMER_PERIODE 60

extern DMA_HandleTypeDef hdma_tim3_ch4_up;
extern TIM_HandleTypeDef htim3;

typedef struct _ws2812bLed_ {
	uint8_t Green;
	uint8_t Red;
	uint8_t Blue;
} ws2812bLed_t;

// WS2812 framebuffer - buffer for 2 LEDs - two times 24 bits
volatile static uint8_t mg_dma2PwmBitBuffer[24 * 2] = {0};
volatile static ws2812bLed_t Led[10] = {0};
volatile static uint16_t mg_posInBufferToRead = 0;
const static uint16_t c_sizeOfBufferToStripe = sizeof(Led)/sizeof(ws2812bLed_t);
const static uint16_t c_sizeOfBitBuffer = sizeof(mg_dma2PwmBitBuffer)/sizeof(uint8_t);

static const uint8_t c_PulseLogic0 = (10 * TIMER_PERIODE) / 31; /* 0,403 us */
static const uint8_t c_PulseLogic1 = (10 * TIMER_PERIODE) / 15; /* 0,833 us */


void dmaTransferComplete_Callback(DMA_HandleTypeDef *dma_handle);
void dmaTransferHalfComplete_Callback(DMA_HandleTypeDef *dma_handle);

void fillTheFirstHalfOfBuffer(void);
void fillTheSecondHalfOfBuffer(void);

void ws2812b_init (void)
{
	/* DMA Init */
	HAL_DMA_RegisterCallback(&hdma_tim3_ch4_up, HAL_DMA_XFER_CPLT_CB_ID , &dmaTransferComplete_Callback);
	HAL_DMA_RegisterCallback(&hdma_tim3_ch4_up, HAL_DMA_XFER_HALFCPLT_CB_ID, &dmaTransferComplete_Callback);


	HAL_DMA_Start_IT(&hdma_tim3_ch4_up, (uint32_t)mg_dma2PwmBitBuffer, (uint32_t)&htim3.Instance->CCR4, c_sizeOfBitBuffer);
}

void dmaTransferComplete_Callback(DMA_HandleTypeDef *dma_handle)
{

}

void dmaTransferHalfComplete_Callback(DMA_HandleTypeDef *dma_handle)
{


}

void fillTheFirstHalfOfBuffer(void)
{
	if (mg_posInBufferToRead >= c_sizeOfBufferToStripe)
	{
		for (uint8_t i = 0; i < 23; ++i)
		{
			mg_dma2PwmBitBuffer[i] = 0;
		}
	}
	else
	{
		/* Green */
		mg_dma2PwmBitBuffer[0]  = (Led[mg_posInBufferToRead].Green & 0x80) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[1]  = (Led[mg_posInBufferToRead].Green & 0x40) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[2]  = (Led[mg_posInBufferToRead].Green & 0x20) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[3]  = (Led[mg_posInBufferToRead].Green & 0x10) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[4]  = (Led[mg_posInBufferToRead].Green & 0x08) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[5]  = (Led[mg_posInBufferToRead].Green & 0x04) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[6]  = (Led[mg_posInBufferToRead].Green & 0x02) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[7]  = (Led[mg_posInBufferToRead].Green & 0x01) ? c_PulseLogic1 : c_PulseLogic0;
		/* Red */
		mg_dma2PwmBitBuffer[8]  = (Led[mg_posInBufferToRead].Red & 0x80) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[9]  = (Led[mg_posInBufferToRead].Red & 0x40) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[10] = (Led[mg_posInBufferToRead].Red & 0x20) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[11] = (Led[mg_posInBufferToRead].Red & 0x10) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[12] = (Led[mg_posInBufferToRead].Red & 0x08) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[13] = (Led[mg_posInBufferToRead].Red & 0x04) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[14] = (Led[mg_posInBufferToRead].Red & 0x02) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[15] = (Led[mg_posInBufferToRead].Red & 0x01) ? c_PulseLogic1 : c_PulseLogic0;
		/* Blue */
		mg_dma2PwmBitBuffer[16] = (Led[mg_posInBufferToRead].Blue & 0x80) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[17] = (Led[mg_posInBufferToRead].Blue & 0x40) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[18] = (Led[mg_posInBufferToRead].Blue & 0x20) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[19] = (Led[mg_posInBufferToRead].Blue & 0x10) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[20] = (Led[mg_posInBufferToRead].Blue & 0x08) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[21] = (Led[mg_posInBufferToRead].Blue & 0x04) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[22] = (Led[mg_posInBufferToRead].Blue & 0x02) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[23] = (Led[mg_posInBufferToRead].Blue & 0x01) ? c_PulseLogic1 : c_PulseLogic0;
	}


}
void fillTheSecondHalfOfBuffer(void)
{
	if (mg_posInBufferToRead >= c_sizeOfBufferToStripe)
	{
		for (uint8_t i = 27; i < 47; ++i)
		{
			mg_dma2PwmBitBuffer[i] = 0;
		}
	}
	else
	{
		/* Green */
		mg_dma2PwmBitBuffer[24] = (Led[mg_posInBufferToRead].Green & 0x80) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[25] = (Led[mg_posInBufferToRead].Green & 0x40) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[26] = (Led[mg_posInBufferToRead].Green & 0x20) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[27] = (Led[mg_posInBufferToRead].Green & 0x10) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[28] = (Led[mg_posInBufferToRead].Green & 0x08) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[29] = (Led[mg_posInBufferToRead].Green & 0x04) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[30] = (Led[mg_posInBufferToRead].Green & 0x02) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[31] = (Led[mg_posInBufferToRead].Green & 0x01) ? c_PulseLogic1 : c_PulseLogic0;
		/* Red */
		mg_dma2PwmBitBuffer[32] = (Led[mg_posInBufferToRead].Red & 0x80) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[33] = (Led[mg_posInBufferToRead].Red & 0x40) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[34] = (Led[mg_posInBufferToRead].Red & 0x20) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[35] = (Led[mg_posInBufferToRead].Red & 0x10) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[36] = (Led[mg_posInBufferToRead].Red & 0x08) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[37] = (Led[mg_posInBufferToRead].Red & 0x04) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[38] = (Led[mg_posInBufferToRead].Red & 0x02) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[39] = (Led[mg_posInBufferToRead].Red & 0x01) ? c_PulseLogic1 : c_PulseLogic0;
		/* Blue */
		mg_dma2PwmBitBuffer[40] = (Led[mg_posInBufferToRead].Blue & 0x80) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[41] = (Led[mg_posInBufferToRead].Blue & 0x40) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[42] = (Led[mg_posInBufferToRead].Blue & 0x20) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[43] = (Led[mg_posInBufferToRead].Blue & 0x10) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[44] = (Led[mg_posInBufferToRead].Blue & 0x08) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[45] = (Led[mg_posInBufferToRead].Blue & 0x04) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[46] = (Led[mg_posInBufferToRead].Blue & 0x02) ? c_PulseLogic1 : c_PulseLogic0;
		mg_dma2PwmBitBuffer[47] = (Led[mg_posInBufferToRead].Blue & 0x01) ? c_PulseLogic1 : c_PulseLogic0;
	}

}

