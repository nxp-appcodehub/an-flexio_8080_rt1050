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

#ifndef _FSL_FLEXIO_MCULCD_EDMA_H_
#define _FSL_FLEXIO_MCULCD_EDMA_H_

#include "fsl_edma.h"
#include "fsl_flexio_mculcd.h"

/*!
 * @addtogroup flexio_mculcd_edma
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief  typedef for flexio_mculcd_edma_handle_t in advance. */
typedef struct _flexio_mculcd_edma_handle flexio_mculcd_edma_handle_t;

/*! @brief FlexIO MCULCD master callback for transfer complete.
 *
 * When tranfer finished, the callback function is called and returns the
 * @p status as kStatus_FLEXIO_MCULCD_Idle.
 */
typedef void (*flexio_mculcd_edma_transfer_callback_t)(FLEXIO_MCULCD_Type *base,
                                                       flexio_mculcd_edma_handle_t *handle,
                                                       status_t status,
                                                       void *userData);

/*! @brief FlexIO MCULCD eDMA transfer handle, users should not touch the
 * content of the handle.*/
struct _flexio_mculcd_edma_handle
{
    FLEXIO_MCULCD_Type *base;       /*!< Pointer to the FLEXIO_MCULCD_Type. */
    uint8_t txShifterNum;           /*!< Number of shifters used for TX. */
    uint8_t rxShifterNum;           /*!< Number of shifters used for RX. */
    uint32_t minorLoopBytes;        /*!< eDMA transfer minor loop bytes. */
    edma_modulo_t txEdmaModulo;     /*!< Modulo value for the FlexIO shifter buffer access. */
    edma_modulo_t rxEdmaModulo;     /*!< Modulo value for the FlexIO shifter buffer access. */
    uint32_t dataAddrOrSameValue;   /*!< When sending the same value for many times,
                                         this is the value to send. When writing or
                                         reading array, this is the address of the
                                         data array. */
    size_t dataCount;               /*!< Total count to be transferred. */
    volatile size_t remainingCount; /*!< Remaining count still not transfered. */
    volatile uint32_t state;        /*!< FlexIO MCULCD driver internal state. */
    edma_handle_t *txEdmaHandle;    /*!< DMA handle for MCULCD TX */
    edma_handle_t *rxEdmaHandle;    /*!< DMA handle for MCULCD RX */
    flexio_mculcd_edma_transfer_callback_t completionCallback; /*!< Callback for MCULCD DMA transfer */
    void *userData;                                            /*!< User Data for MCULCD DMA callback */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name eDMA Transactional
 * @{
 */

/*!
 * @brief Initializes the FLEXO MCULCD master eDMA handle.
 *
 * This function initializes the FLEXO MCULCD master eDMA handle which can be
 * used for other FLEXO MCULCD transactional APIs. For a specified FLEXO MCULCD
 * instance, call this API once to get the initialized handle.
 *
 * @param base Pointer to FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 * @param callback MCULCD transfer complete callback, NULL means no callback.
 * @param userData callback function parameter.
 * @param txEdmaHandle User requested eDMA handle for FlexIO MCULCD eDMA TX,
 * the DMA request source of this handle should be the first of TX shifters.
 * @param rxEdmaHandle User requested eDMA handle for FlexIO MCULCD eDMA RX,
 * the DMA request source of this handle should be the last of RX shifters.
 * @retval kStatus_Success Successfully create the handle.
 */
status_t FLEXIO_MCULCD_TransferCreateHandleEDMA(FLEXIO_MCULCD_Type *base,
                                                flexio_mculcd_edma_handle_t *handle,
                                                flexio_mculcd_edma_transfer_callback_t callback,
                                                void *userData,
                                                edma_handle_t *txEdmaHandle,
                                                edma_handle_t *rxEdmaHandle);

/*!
 * @brief Performs a non-blocking FlexIO MCULCD transfer using eDMA.
 *
 * This function returns immediately after transfer initiates. To check whether
 * the tranfer is completed, user could:
 * 1. Use the transfer completed callback;
 * 2. Polling function @ref FLEXIO_MCULCD_GetTransferCountEDMA
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 * @param xfer Pointer to FlexIO MCULCD transfer structure.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_MCULCD_Busy FlexIO MCULCD is not idle, it is running another
 * transfer.
 */
status_t FLEXIO_MCULCD_TransferEDMA(FLEXIO_MCULCD_Type *base,
                                    flexio_mculcd_edma_handle_t *handle,
                                    flexio_mculcd_transfer_t *xfer);

/*!
 * @brief Aborts a FlexIO MCULCD transfer using eDMA.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle FlexIO MCULCD eDMA handle pointer.
 */
void FLEXIO_MCULCD_TransferAbortEDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_edma_handle_t *handle);

/*!
 * @brief Gets the remaining bytes for FlexIO MCULCD eDMA transfer.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle FlexIO MCULCD eDMA handle pointer.
 * @param count Number of count transferred so far by the eDMA transaction.
 * @retval kStatus_Success Get the transferred count Successfully.
 * @retval kStatus_NoTransferInProgress No tranfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCountEDMA(FLEXIO_MCULCD_Type *base,
                                            flexio_mculcd_edma_handle_t *handle,
                                            size_t *count);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _FSL_FLEXIO_MCULCD_EDMA_H_ */
