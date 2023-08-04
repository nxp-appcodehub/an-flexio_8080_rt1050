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

#ifndef _FSL_HX8357_H_
#define _FSL_HX8357_H_

#include "fsl_flexio_mculcd.h"
#include "fsl_flexio_mculcd_edma.h"

/*!
 * @addtogroup hx8357
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief HX8357 command. */
#define HX8357_NOP                      0x00
#define HX8357_SOFT_RESET               0x01
#define HX8357_SET_COLUMN_ADDRESS       0x2A
#define HX8357_SET_PAGE_ADDRESS         0x2B
#define HX8357_WRITE_MEMORY_START       0x2C
#define HX8357_READ_MEMORY_START        0x2E

/*! @brief hx8357 handle. */
typedef struct _hx8357_handle
{
    FLEXIO_MCULCD_Type *base;                    /*!< FLEXIO LCD device structure. */
    flexio_mculcd_edma_handle_t flexioLcdHandle; /*!< FLEXIO LCD EDMA driver handle. */
    uint8_t addrMode;                            /*!< The parameter of set_address_mode and get_address_mode. */
    uint16_t panelWidth;                         /*!< Width of the panel. */
    uint16_t panelHeight;                        /*!< Height of the panel. */
    volatile bool transferCompletedFlag;         /*!< EDMA transfer completed. */
} hx8357_handle_t;

/*! @brief Initailize structure of hx8357 */
typedef struct _hx8357_config
{
    uint32_t pclkFreq_Hz;                      /*!< Pixel clock (LSHIFT clock) frequency in Hz. */
    uint16_t panelWidth;                       /*!< How many pixels per line. */
    uint16_t panelHeight;                      /*!< How many lines per panel. */
} hx8357_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initailize the HX8357.
 *
 * This function configures the HX8357 contoller, and setups the interface
 * between LCD panel. This function does not turn on the display, application
 * could turn on the display after filling the frame buffer.
 *
 * @param handle HX8357 handle structure.
 * @param config Pointer to the HX8357 configuration structure.
 * @param base Pointer to the FLEXIO LCD device.
 * @param txEdmaHandle eDMA handle for FlexIO MCULCD eDMA TX.
 * @param rxEdmaHandle eDMA handle for FlexIO MCULCD eDMA RX.
 * @param srcClock_Hz The external reference clock(XTAL or CLK) frequency in Hz.
 *
 * @retval kStatus_Success Initailize successfully.
 * @retval kStatus_InvalidArgument Initailize failed because of invalid argument.
 */
status_t HX8357_Init(hx8357_handle_t *handle,
                      const hx8357_config_t *config,
                      FLEXIO_MCULCD_Type *base,
                      edma_handle_t *txEdmaHandle,
                      edma_handle_t *rxEdmaHandle);

/*!
 * @brief Deinitailize the HX8357.
 *
 * @param handle HX8357 handle structure.
 */
void HX8357_Deinit(hx8357_handle_t *handle);

/*!
 * @brief Select area to read or write.
 *
 * @param handle HX8357 handle structure.
 * @param startX Start point X coordination.
 * @param startY Start point Y coordination.
 * @param endX End point X coordination.
 * @param endY End point Y coordination.
 */
void HX8357_SelectArea(hx8357_handle_t *handle, uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY);

/*!
 * @brief Read pixel data from the selected area.
 *
 * This function reads pixel data to the area selected by the function
 * @ref HX8357_SelectArea. The pixel data will be read from the start of
 * the area, it could not read the pixels out of the selected area.
 *
 * @param handle HX8357 handle structure.
 * @param pixels Pointer to the memory to save the read pixels.
 * @param length Length of the pixel array @p pixels.
 */
void HX8357_ReadPixels(hx8357_handle_t *handle, uint16_t *pixels, uint32_t length);

/*!
 * @brief Write pixel data to the selected area.
 *
 * This function writes pixel data to the area selected by the function
 * @ref HX8357_SelectArea. The pixel data will be written from the start of
 * the area, if the pixel data written is larger than the area size, the part
 * out of range will be ignored.
 *
 * @param handle HX8357 handle structure.
 * @param pixels Pointer to the pixels to write.
 * @param length Length of the pixel array @p pixels.
 */
void HX8357_WritePixels(hx8357_handle_t *handle, const uint16_t *pixels, uint32_t length);

/*!
 * @brief Write same pixel data to the selected area.
 *
 * This function could be used to clear the window easily. It sends the same pixel
 * color many times.
 *
 * @param handle HX8357 handle structure.
 * @param color The color to send.
 * @param pixelCount How many pixels to set.
 */
void HX8357_WriteSamePixels(hx8357_handle_t *handle, const uint32_t color, uint32_t pixelCount);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_HX8357_H_ */
