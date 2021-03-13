/**
 * @file spi_driver.c
 * @brief SPI driver
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.9.0
 **/

//Dependencies
#include "core/net.h"
#include "spi_driver.h"

#include "xspi.h"

//SPI bitrate
#define SPI_BITRATE 10000000


/**
 * @brief SPI driver
 **/

const SpiDriver spiDriver =
{
   spiInit,
   spiSetMode,
   spiSetBitrate,
   spiAssertCs,
   spiDeassertCs,
   spiTransfer
};

XSpi SpiDeviceInst;

/**
 * @brief SPI initialization
 * @return Error code
 **/

error_t spiInit(void)
{

	u32 Status;
	XSpi_Config *ConfigPtr;	/* Pointer to Configuration data */

	ConfigPtr = XSpi_LookupConfig(XPAR_AXI_QUAD_SPI_0_DEVICE_ID);

	Status = XSpi_CfgInitialize(&SpiDeviceInst, ConfigPtr,
				  ConfigPtr->BaseAddress);

	u32 Control;
	//XSpi_WriteReg(SpiDeviceInst.BaseAddr, XSP_SRR_OFFSET, 0x0000000a);

	//usleep(20);

	XSpi_WriteReg(SpiDeviceInst.BaseAddr, XSP_SSR_OFFSET, 0x1);
	//Control = XSpi_ReadReg(SpiDeviceInst.BaseAddr, XSP_CR_OFFSET);
	Control = (XSP_CR_MASTER_MODE_MASK | XSP_CR_MANUAL_SS_MASK | XSP_CR_RXFIFO_RESET_MASK | XSP_CR_TXFIFO_RESET_MASK | XSP_CR_ENABLE_MASK);
	XSpi_WriteReg(SpiDeviceInst.BaseAddr, XSP_CR_OFFSET, Control);

	Status = XSpi_ReadReg(SpiDeviceInst.BaseAddr, XSP_SR_OFFSET) ;
	if(Status & !XSP_SR_SLAVE_MODE_MASK &!XSP_SR_RX_EMPTY_MASK & !XSP_SR_TX_EMPTY_MASK) return (error_t)1;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Set SPI mode
 * @param mode SPI mode (0, 1, 2 or 3)
 **/

error_t spiSetMode(uint_t mode)
{
   //Not implemented
   return ERROR_NOT_IMPLEMENTED;
}


/**
 * @brief Set SPI bitrate
 * @param bitrate Bitrate value
 **/

error_t spiSetBitrate(uint_t bitrate)
{
   //Not implemented
   return ERROR_NOT_IMPLEMENTED;
}


/**
 * @brief Assert CS
 **/

void spiAssertCs(void)
{
   //Assert EXT1_CS
	XSpi_WriteReg(SpiDeviceInst.BaseAddr, XSP_SSR_OFFSET, 0x0);

   //CS setup time
   usleep(1);
}


/**
 * @brief Deassert CS
 **/

void spiDeassertCs(void)
{
	XSpi_WriteReg(SpiDeviceInst.BaseAddr, XSP_SSR_OFFSET, 0x1);

   //CS setup time
   usleep(1);
   //CS hold time
}


/**
 * @brief Transfer a single byte
 * @param[in] data The data to be written
 * @return The data received from the slave device
 **/

uint8_t spiTransfer(uint8_t data)
{
   //Ensure the TX buffer is empty
   while(XSpi_ReadReg(SpiDeviceInst.BaseAddr, XSP_SR_OFFSET) & XSP_SR_TX_FULL_MASK);
   //Start to transfer data
   XSpi_WriteReg(SpiDeviceInst.BaseAddr, XSP_DTR_OFFSET, data);
   //Wait for the operation to complete
   while(XSpi_ReadReg(SpiDeviceInst.BaseAddr, XSP_SR_OFFSET) & XSP_SR_TX_FULL_MASK);

   //Return the received character
   return XSpi_ReadReg(SpiDeviceInst.BaseAddr, XSP_DRR_OFFSET);
}
