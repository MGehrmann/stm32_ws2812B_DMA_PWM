/*
 * ws2812b_base.c
 *
 *  Created on: Nov 15, 2021
 *      Author: termy
 */

/* Periode of the Timer 48MHz*1,25us = 60 */


#include <stdbool.h>
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
volatile static uint16_t mg_numOfRepeats = 5;
volatile static uint16_t mg_numOfRepeatsCnt = 0;
volatile static bool mg_flagSequenzEnd = false;


const static uint16_t c_sizeOfBufferToStripe = sizeof(Led)/sizeof(Led[0]);
const static uint16_t c_sizeOfBitBuffer = sizeof(mg_dma2PwmBitBuffer)/sizeof(uint8_t);

static const uint8_t c_PulseLogic0 = (10 * TIMER_PERIODE) / 31; /* 0,403 us */
static const uint8_t c_PulseLogic1 = (10 * TIMER_PERIODE) / 15; /* 0,833 us */

const uint8_t gammaTable[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};


static void fillTheFirstHalfOfBuffer(void);
static void fillTheSecondHalfOfBuffer(void);

static void finishSequenze(void);

void ws2812b_init (void)
{
	/* DMA Init */
//	HAL_DMA_RegisterCallback(&hdma_tim3_ch4_up, HAL_DMA_XFER_CPLT_CB_ID , &dmaTransferComplete_Callback);
//	HAL_DMA_RegisterCallback(&hdma_tim3_ch4_up, HAL_DMA_XFER_HALFCPLT_CB_ID, &dmaTransferComplete_Callback);

	//Debug
//	for (uint8_t i = 0; i < 10; ++i)
//	{
//		Led[i].Red = 0*i;
//		Led[i].Green = 20*i;
//		Led[i].Blue = 0*i;
//	}
	Led[0].Red = 50;
	Led[1].Green = 50;
	Led[2].Blue = 50;
	Led[3].Red = 100;
	Led[4].Green = 100;
	Led[5].Blue = 100;
	Led[6].Red = 200;
	Led[7].Green = 200;
	Led[8].Blue = 200;

	fillTheFirstHalfOfBuffer();
//	HAL_DMA_Start_IT(&hdma_tim3_ch4_up, (uint32_t)mg_dma2PwmBitBuffer, (uint32_t)&htim3.Instance->CCR4, c_sizeOfBitBuffer);
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t*)mg_dma2PwmBitBuffer, c_sizeOfBitBuffer);
}

void ws2812b_main(void)
{

}


/**
  * @brief  PWM Pulse finished callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim3)
	{
		fillTheSecondHalfOfBuffer();
	}

}

/**
  * @brief  PWM Pulse finished half complete callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim3)
	{
		fillTheFirstHalfOfBuffer();
	}
}

//void dmaTransferComplete_Callback(DMA_HandleTypeDef *dma_handle)
//{
//	fillTheSecondHalfOfBuffer();
//}

//void dmaTransferHalfComplete_Callback(DMA_HandleTypeDef *dma_handle)
//{
//	fillTheFirstHalfOfBuffer();
//}

// wenn die erste hälfte übertragen wurde
void fillTheFirstHalfOfBuffer(void)
{
	if (mg_posInBufferToRead >= c_sizeOfBufferToStripe)
	{
		mg_posInBufferToRead = 0;
		mg_numOfRepeats++;
	}

	if (mg_numOfRepeatsCnt >= mg_numOfRepeats)
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

	mg_posInBufferToRead++;



}

// wenn komplet übertragen wurde
void fillTheSecondHalfOfBuffer(void)
{



	if (mg_posInBufferToRead >= c_sizeOfBufferToStripe)
	{
		mg_posInBufferToRead = 0;
		mg_numOfRepeatsCnt++;
	}

	if (mg_flagSequenzEnd)
	{
		HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_4);
		mg_flagSequenzEnd = false;
		mg_numOfRepeatsCnt = 0;
	}
	else if (mg_numOfRepeatsCnt >= mg_numOfRepeats)
	{
		for (uint8_t i = 0; i < 23; ++i)
		{
			mg_dma2PwmBitBuffer[i] = 0;
		}
		mg_flagSequenzEnd = true;

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

	mg_posInBufferToRead++;
}

