/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "picture.h"

#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_flexio_mculcd_edma.h"
#include "fsl_hx8357.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Macros for LCD EDMA. */
#define FLEXIO_TX_DMA_CHANNEL 0u
#define FLEXIO_RX_DMA_CHANNEL 1u
#define FLEXIO_TX_DMA_REQUEST kDmaRequestMuxFlexIO2Request0Request1
#define FLEXIO_RX_DMA_REQUEST kDmaRequestMuxFlexIO2Request2Request3


/* Select pll3_sw_clk (480 MHz) as FlexIO2 clock source */
#define FLEXIO2_CLOCK_SOURCE_SELECT (3U)
/* Clock pre divider for FlexIO2 clock source */
#define FLEXIO2_CLOCK_SOURCE_PRE_DIVIDER (1U)
/* Clock divider for FlexIO2 clock source */
#define FLEXIO2_CLOCK_SOURCE_DIVIDER (1U)
/* Frequency of FlexIO2 clock: 480MHz/(1+1)/(1+1)=120MHz */
#define FLEXIO2_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk)) / (FLEXIO2_CLOCK_SOURCE_DIVIDER + 1U) /(FLEXIO2_CLOCK_SOURCE_PRE_DIVIDER + 1U))


#define BEAT_RATE     10000000

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_SetCSPin(bool set);
void BOARD_SetRSPin(bool set);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* HX8357 LCD controller handle. */
hx8357_handle_t lcdHandle;

/* The FlexIO MCU LCD device. */
FLEXIO_MCULCD_Type flexioLcdDev = {
    .flexioBase = FLEXIO2,
    .busType = kFLEXIO_MCULCD_8080,
    .dataPinStartIndex = 12u,
    .ENWRPinIndex = 0u,
    .RDPinIndex = 1u,
    .txShifterStartIndex = 0u,
    .txShifterEndIndex = 3u,
    .rxShifterStartIndex = 0u,
    .rxShifterEndIndex = 3u,
    .timerIndex = 0u,
    .setCSPin = BOARD_SetCSPin,
    .setRSPin = BOARD_SetRSPin,
    .setRDWRPin = NULL          /* Not used in 8080 mode. */
};


/*******************************************************************************
 * Code
 ******************************************************************************/
void BOARD_SetCSPin(bool set)
{
    GPIO_PinWrite(LCD_CS_GPIO, LCD_CS_PIN, (uint8_t)set);
}

void BOARD_SetRSPin(bool set)
{
    GPIO_PinWrite(LCD_RS_GPIO, LCD_RS_PIN, (uint8_t)set);
}

void Delay(uint32_t loops)
{
    while (loops--)
    {
        __NOP();
    }
}

void InitDma(void)
{
    edma_config_t edmaConfig;

    EDMA_GetDefaultConfig(&edmaConfig);
    EDMA_Init(DMA0, &edmaConfig);

    DMAMUX_Init(DMAMUX);
}

status_t InitLcd(void)
{
    status_t status;

    /* FlexIO MCU LCD EDMA handle. */
    static edma_handle_t rxEdmaHandle;
    static edma_handle_t txEdmaHandle;

    flexio_mculcd_config_t flexioMcuLcdConfig;

    const hx8357_config_t hx8357Config = {.pclkFreq_Hz = BEAT_RATE,
                                          .panelWidth = PIC_WIDTH,
                                          .panelHeight = PIC_HEIGHT,
                                         };

    /* Initialize the flexio MCU LCD. */
    FLEXIO_MCULCD_GetDefaultConfig(&flexioMcuLcdConfig);

    flexioMcuLcdConfig.beatRate = BEAT_RATE;
    status = FLEXIO_MCULCD_Init(&flexioLcdDev, &flexioMcuLcdConfig, FLEXIO2_CLOCK_FREQUENCY);
    if (kStatus_Success != status)
    {
        return status;
    }

    DMAMUX_SetSource(DMAMUX, FLEXIO_RX_DMA_CHANNEL, FLEXIO_RX_DMA_REQUEST);
    DMAMUX_SetSource(DMAMUX, FLEXIO_TX_DMA_CHANNEL, FLEXIO_TX_DMA_REQUEST);
    DMAMUX_EnableChannel(DMAMUX, FLEXIO_RX_DMA_CHANNEL);
    DMAMUX_EnableChannel(DMAMUX, FLEXIO_TX_DMA_CHANNEL);

    EDMA_CreateHandle(&rxEdmaHandle, DMA0, FLEXIO_RX_DMA_CHANNEL);
    EDMA_CreateHandle(&txEdmaHandle, DMA0, FLEXIO_TX_DMA_CHANNEL);

    status = HX8357_Init(&lcdHandle, &hx8357Config, &flexioLcdDev, &txEdmaHandle, &rxEdmaHandle);

    return status;
}
/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_BootClockRUN();
    BOARD_InitPins();
    BOARD_InitDebugConsole();

    /*Clock setting for FlexIO2*/
    CLOCK_SetMux(kCLOCK_Flexio2Mux, FLEXIO2_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Flexio2PreDiv, FLEXIO2_CLOCK_SOURCE_PRE_DIVIDER);
    CLOCK_SetDiv(kCLOCK_Flexio2Div, FLEXIO2_CLOCK_SOURCE_DIVIDER);

    PRINTF("FlexIO 8080 LCD demo.\r\n");

    /* Initialize DMA. */
    InitDma();

    /* Initialize the LCD controller. */
    status_t status = InitLcd();
    if (kStatus_Success != status)
    {
        while (1)
        {
        }
    }

    /* Send picture. */
    HX8357_SelectArea(&lcdHandle, 0, 0, PIC_WIDTH - 1, PIC_HEIGHT - 1);

    while (1)
    {
        HX8357_WritePixels(&lcdHandle, demoPicture2, PIC_HEIGHT * PIC_WIDTH);
        Delay(100000000);
    }
}
