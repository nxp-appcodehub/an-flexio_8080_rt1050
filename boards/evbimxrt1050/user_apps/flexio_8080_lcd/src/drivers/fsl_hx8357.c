/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
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
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include "fsl_hx8357.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Use loop to delay.
 *
 * @param loops Number of the loops.
 */
static void HX8357_Delay(uint32_t loops);

/*!
 * @brief HX8357 EDMA transfer complete callback function.
 */
void HX8357_TransferCompletedCallback(FLEXIO_MCULCD_Type *base,
                                       flexio_mculcd_edma_handle_t *handle,
                                       status_t status,
                                       void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void HX8357_Delay(uint32_t loops)
{
    while (loops--)
    {
        __NOP();
    }
}

void HX8357_TransferCompletedCallback(FLEXIO_MCULCD_Type *base,
                                       flexio_mculcd_edma_handle_t *handle,
                                       status_t status,
                                       void *userData)
{
    if (kStatus_FLEXIO_MCULCD_Idle == status)
    {
        ((hx8357_handle_t *)userData)->transferCompletedFlag = true;
    }
}

status_t HX8357_Init(hx8357_handle_t *handle,
                      const hx8357_config_t *config,
                      FLEXIO_MCULCD_Type *base,
                      edma_handle_t *txEdmaHandle,
                      edma_handle_t *rxEdmaHandle)
{
    assert(handle);
    assert(config);

#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    uint8_t commandParam[20];
#else
    uint16_t commandParam[20];
#endif

    status_t status;

    /* Initialize the handle. */
    memset(handle, 0, sizeof(hx8357_handle_t));

    status = FLEXIO_MCULCD_TransferCreateHandleEDMA(base, &handle->flexioLcdHandle, HX8357_TransferCompletedCallback,
                                                    handle, txEdmaHandle, rxEdmaHandle);
    if (kStatus_Success != status)
    {
        return status;
    }

    handle->base = base;
    handle->panelWidth = config->panelWidth;
    handle->panelHeight = config->panelHeight;

    /* Configure LCD panel. */
    commandParam[0U] = 0x00;
    commandParam[1U] = 0x00;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xB0);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 2U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x02;
    commandParam[1U] = 0x00;
    commandParam[2U] = 0x00;
    commandParam[3U] = 0x00;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xB3);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 4U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x00;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xB4);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 1U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x13;
    commandParam[1U] = 0x3B;
    commandParam[2U] = 0x00;
    commandParam[3U] = 0x03;
    commandParam[4U] = 0x00;
    commandParam[5U] = 0x01;
    commandParam[6U] = 0x00;
    commandParam[7U] = 0x43;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xC0);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 8U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x08;
    commandParam[1U] = 0x12;
    commandParam[2U] = 0x08;
    commandParam[3U] = 0x08;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xC1);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 4U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x11;
    commandParam[1U] = 0x07;
    commandParam[2U] = 0x03;
    commandParam[3U] = 0x03;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xC4);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 4U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x04;
    commandParam[1U] = 0x09;
    commandParam[2U] = 0x16;
    commandParam[3U] = 0x5A;
    commandParam[4U] = 0x02;
    commandParam[5U] = 0x0A;
    commandParam[6U] = 0x16;
    commandParam[7U] = 0x05;
    commandParam[8U] = 0x00;
    commandParam[9U] = 0x32;
    commandParam[10U] = 0x05;
    commandParam[11U] = 0x16;
    commandParam[12U] = 0x0A;
    commandParam[13U] = 0x53;
    commandParam[14U] = 0x08;
    commandParam[15U] = 0x16;
    commandParam[16U] = 0x09;
    commandParam[17U] = 0x04;
    commandParam[18U] = 0x32;
    commandParam[19U] = 0x00;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xC8);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 20U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x00;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x35);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 1U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x55;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x3A);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 1U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x00;
    commandParam[1U] = 0x01;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x44);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 2U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x11);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(150000U);

    commandParam[0U] = 0x07;
    commandParam[1U] = 0x07;
    commandParam[2U] = 0x1E;
    commandParam[3U] = 0x07;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xD0);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 4U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x03;
    commandParam[1U] = 0x52;
    commandParam[2U] = 0x10;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xD1);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 3U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] = 0x03;
    commandParam[1U] = 0x24;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xD2);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 2U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    commandParam[0U] =0x00;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0xB0);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 1U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(2000);

    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x29);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(10000U);

    commandParam[0U] =0x55;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x3A);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 1U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(100000U);

    commandParam[0U] =0x28;
    FLEXIO_MCULCD_StartTransfer(base);
    FLEXIO_MCULCD_WriteCommandBlocking(base, 0x36);
    FLEXIO_MCULCD_WriteDataArrayBlocking(base, commandParam, 1U);
    FLEXIO_MCULCD_StopTransfer(base);
    HX8357_Delay(100000U);

    return kStatus_Success;
}

void HX8357_Deinit(hx8357_handle_t *handle)
{
    assert(handle);

    memset(handle, 0, sizeof(hx8357_handle_t));
}

void HX8357_SelectArea(hx8357_handle_t *handle, uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY)
{
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    uint8_t commandParam[4];
#else
    uint16_t commandParam[4];
#endif

    /* Send the set_page_address command. */
    commandParam[0] = (startY & 0xFF00U) >> 8U;
    commandParam[1] = startY & 0xFFU;
    commandParam[2] = (endY & 0xFF00U) >> 8U;
    commandParam[3] = endY & 0xFFU;

    FLEXIO_MCULCD_StartTransfer(handle->base);
    FLEXIO_MCULCD_WriteCommandBlocking(handle->base, HX8357_SET_PAGE_ADDRESS);
    FLEXIO_MCULCD_WriteDataArrayBlocking(handle->base, commandParam, 4);
    FLEXIO_MCULCD_StopTransfer(handle->base);
    HX8357_Delay(2000);

    /* Send the set_column_address command. */
    commandParam[0] = (startX & 0xFF00U) >> 8U;
    commandParam[1] = startX & 0xFFU;
    commandParam[2] = (endX & 0xFF00U) >> 8U;
    commandParam[3] = endX & 0xFFU;

    FLEXIO_MCULCD_StartTransfer(handle->base);
    FLEXIO_MCULCD_WriteCommandBlocking(handle->base, HX8357_SET_COLUMN_ADDRESS);
    FLEXIO_MCULCD_WriteDataArrayBlocking(handle->base, commandParam, 4);
    FLEXIO_MCULCD_StopTransfer(handle->base);
    HX8357_Delay(2000);
}

void HX8357_WritePixels(hx8357_handle_t *handle, const uint16_t *pixels, uint32_t length)
{
    flexio_mculcd_transfer_t xfer;

    xfer.command = HX8357_WRITE_MEMORY_START;
    xfer.mode = kFLEXIO_MCULCD_WriteArray;
    xfer.dataAddrOrSameValue = (uint32_t)pixels;
    xfer.dataSize = length * 2;

    handle->transferCompletedFlag = false;
    FLEXIO_MCULCD_TransferEDMA(handle->base, &handle->flexioLcdHandle, &xfer);
    while (!handle->transferCompletedFlag)
    {
    }
}

void HX8357_ReadPixels(hx8357_handle_t *handle, uint16_t *pixels, uint32_t length)
{
    flexio_mculcd_transfer_t xfer;

    xfer.command = HX8357_READ_MEMORY_START;
    xfer.mode = kFLEXIO_MCULCD_ReadArray;
    xfer.dataAddrOrSameValue = (uint32_t)pixels;
    xfer.dataSize = length * 2;

    handle->transferCompletedFlag = false;
    FLEXIO_MCULCD_TransferEDMA(handle->base, &handle->flexioLcdHandle, &xfer);
    while (!handle->transferCompletedFlag)
    {
    }
}

void HX8357_WriteSamePixels(hx8357_handle_t *handle, const uint32_t color, uint32_t pixelCount)
{
    flexio_mculcd_transfer_t xfer;

    xfer.command = HX8357_WRITE_MEMORY_START;
    xfer.mode = kFLEXIO_MCULCD_WriteSameValue;
    xfer.dataAddrOrSameValue = color;
    xfer.dataSize = pixelCount * 2;

    handle->transferCompletedFlag = false;
    FLEXIO_MCULCD_TransferEDMA(handle->base, &handle->flexioLcdHandle, &xfer);
    while (!handle->transferCompletedFlag)
    {
    }
}

