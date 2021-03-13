/**
 * @file ext_int_driver.c
 * @brief External interrupt line driver
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
#include "drivers/eth/enc624j600_driver.h"
#include "ext_int_driver.h"
#include <stdbool.h>

/**
 * @brief External interrupt line driver
 **/

const ExtIntDriver extIntDriver =
{
   extIntInit,
   extIntEnableIrq,
   extIntDisableIrq
};

XIntc InterruptController;     /* The instance of the Interrupt Controller */


/**
 * @brief EXTI configuration
 * @return Error code
 **/

error_t extIntInit(void)
{
      //Successful processing
   return NO_ERROR;
}

bool IRQenabled = false;

/**
 * @brief Enable external interrupts
 **/

void extIntEnableIrq(void)
{
   //Enable PIOA interrupts
	XIntc_Enable(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_SYSTEM_INT_NIC100_INTR);
	IRQenabled = true;

}


/**
 * @brief Disable external interrupts
 **/

void extIntDisableIrq(void)
{
   //Disable PIOA interrupts
	XIntc_Disable(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_SYSTEM_INT_NIC100_INTR);
	IRQenabled = false;
}


/**
 * @brief External interrupt handler
 **/

void ENC_Int_Handler(void *CallbackRef)
{
   XIntc_Acknowledge(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_SYSTEM_INT_NIC100_INTR);
   bool_t flag;
   //volatile uint32_t status;
   NetInterface *interface;

   //Enter interrupt service routine
   osEnterIsr();

   //Point to the structure describing the network interface
   interface = &netInterface[0];
   //This flag will be set if a higher priority task must be woken
   flag = FALSE;

   flag = enc624j600IrqHandler(interface);

   if(flag)osSetEvent(&netEvent);

   //Leave interrupt service routine
   osExitIsr(flag);
}
