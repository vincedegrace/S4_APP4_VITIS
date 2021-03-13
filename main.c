/*
 *
 * @file main.c
 * @brief Main routine
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
#include <cyclone_tcp/std_services/accel.h>
#include <packetTask.h>
#include <stdlib.h>
#include "xparameters.h"
#include <stdio.h>
#include "xil_io.h"
#include "common/os_port.h"
#include "cyclone_tcp/core/net.h"
#include "cyclone_tcp/net_config.h"
#include "cyclone_tcp/drivers/eth/enc624j600_driver.h"
#include "cyclone_tcp/core/udp.h"
#include "common/error.h"
#include "xgpio.h"
#include "xuartlite.h"
#include "xspi.h"
#include "myip_S4e_4reg.h"
#include "cyclone_tcp/spi_driver.h"
#include "cyclone_tcp/ext_int_driver.h"
#include <stdbool.h>


//Application configuration
#define APP_USE_DEFAULT_MAC_ADDR ENABLED
#define APP_MAC_ADDR "00-AB-CD-EF-07-95"

#define APP_USE_DHCP DISABLED
#define APP_IPV4_HOST_ADDR "192.168.13.2"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.13.1"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC DISABLED

#define INTC_DEVICE_ID			XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
//#define MY_IP_BASE_ADDRESS_0     	XPAR_CALCUL_MOYENNE_MYIP_S4E_4REG_0_S00_AXI_BASEADDR //Adresse de base, MyIP0 - axe X
//#define MY_IP_BASE_ADDRESS_1     	XPAR_CALCUL_MOYENNE_MYIP_S4E_4REG_1_S00_AXI_BASEADDR //Adresse de base, MyIP1 - axe Y
//#define MY_IP_BASE_ADDRESS_2     	XPAR_CALCUL_MOYENNE_MYIP_S4E_4REG_2_S00_AXI_BASEADDR //Adresse de base, MyIP2 - axe Z
//#define MYIP_REG_0  			MYIP_S4E_4REG_S00_AXI_SLV_REG0_OFFSET
//#define MYIP_REG_1  			MYIP_S4E_4REG_S00_AXI_SLV_REG1_OFFSET
//#define MYIP_REG_2  			MYIP_S4E_4REG_S00_AXI_SLV_REG2_OFFSET



/****************************************************************************************
//Global variables
****************************************************************************************/
u32 counter = 0;
XGpio outputLED;


unsigned char fakePacket[480];



struct axes{
    int X[40];
    int Y[40];
    int Z[40];
};
struct axes acceleration;
int bufferEntiers[120];
unsigned char bufferChar[480];

int newPacket = 0;
int packetReady = 0;
int calculsFinis = 0;

int startWriting = 0;
int Index = 0;
int axe = 1;

u32 valeur[120];
u32 moyenne[120];
u32 somme[120];


int accelX = 25;
int accelY = 150;
int accelZ = 255;

int etatM1 = 0;
int etatM2 = 0;

unsigned int ReceivedCount = 0;


/***************************************************************************************/

/**
 * @brief System initialization
 **/

Ipv4Addr ipv4Addr;

void systemInit(void)
{
   error_t error;
   MacAddr macAddr;

   //Initialize kernel
   osInitKernel();

   //TCP/IP stack initialization
   error = netInit();

   //Configure the first Ethernet interface

   //Set interface name
   netSetInterfaceName(&netInterface[0], "eth0");
   //Set host name
   netSetHostname(&netInterface[0], "UDPAccel");
   //Select the relevant network adapter
   netSetDriver(&netInterface[0], &enc624j600Driver);
   //netSetPhyDriver(interface, &lan8720PhyDriver);

   netSetSpiDriver(&netInterface[0], &spiDriver);
   netSetExtIntDriver(&netInterface[0], &extIntDriver);

#if (APP_USE_DEFAULT_MAC_ADDR == ENABLED)
   //Use the factory preprogrammed MAC address
   macStringToAddr("00-00-00-00-00-00", &macAddr);
   netSetMacAddr(&netInterface[0], &macAddr);
#endif

   //Initialize network interface
   error = netConfigInterface(&netInterface[0]);
   //Any error to report?

#if (IPV4_SUPPORT == ENABLED)

   //Set IPv4 host address
   ipv4StringToAddr(APP_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(&netInterface[0], ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(&netInterface[0], ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(&netInterface[0], ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(&netInterface[0], 0, ipv4Addr);
   ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(&netInterface[0], 1, ipv4Addr);
#endif

   //init_platform();
}

/*
 Pogne la valeur qui a été shouté dans les régistres de MYIP pis les print
 */
void MyIP_InterruptHandler(void *CallbackRef)
{
	XIntc_Acknowledge(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_CALCUL_MOYENNE_UTIL_VECTOR_LOGIC_0_RES_INTR);
	int axeSize = 40;
	if(Index<axeSize){
		createPacket(moyenne,somme,valeur, Index, axe);
		Index++;
	}
	if(Index >= axeSize){
		Index = 0;
		if(axe<3){
			axe++;
		}
		else
		{
			xil_printf("CALCULS COMPLETES\n\r");
			axe = 1;
		}
	}
}


int SetupInterruptSystem()
{

	int Status;

	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_CALCUL_MOYENNE_UTIL_VECTOR_LOGIC_0_RES_INTR,
			   (XInterruptHandler)MyIP_InterruptHandler,
			   (void *)0);

	Status = XIntc_Connect(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_SYSTEM_INT_NIC100_INTR,
			   (XInterruptHandler)ENC_Int_Handler,
			   (void *)0);


	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);

	XIntc_Enable(&InterruptController, XPAR_MICROBLAZE_0_AXI_INTC_CALCUL_MOYENNE_UTIL_VECTOR_LOGIC_0_RES_INTR);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler)XIntc_InterruptHandler,
				&InterruptController);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}




/**
 * @brief LED blinking task
 **/


void blinkTask()
{
	if(counter++>500000)
	{
		XGpio_DiscreteWrite(&outputLED, 2, XGpio_DiscreteRead(&outputLED, 2) ^ 0x2);
		counter = 0;
	}
}


XUartLite UartLite;
#define TEST_BUFFER_SIZE 16
//u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

void uartInit(){
	int Status;

	Status = XUartLite_Initialize(&UartLite, XPAR_AXI_UARTLITE_0_DEVICE_ID);
	Status = XUartLite_SelfTest(&UartLite);
}


void uartTask(){
		ReceivedCount += XUartLite_Recv(&UartLite,
					   RecvBuffer + ReceivedCount,
					   TEST_BUFFER_SIZE - ReceivedCount);

}

/**
 * @brief Main entry point
 * @return Unused value
 **/
//const char* allo = "Hi type somtin\n\r";


bool udpSocketUp = false;

void machine1(){
	switch(etatM1){
		case 0:
		{
			//print("etatM1 = 0\n\r");
			if(newPacket==1){
				etatM1 = 1;
			    newPacket = 0;
			    packetReady=0;
			}
			break;
		}
		case 1:
		{
			print("etatM1 = 1\n\r");
			charToInt(context.buffer, sizeof(context.buffer)/4, bufferEntiers);
	    	accelXYZ(bufferEntiers, &acceleration, sizeof(bufferEntiers)/4);
	    	startWriting = 1;
	    	etatM1 = 2;
			break;
		}
		case 2:
		{
			print("etatM1 = 2\n\r");
	    	if(calculsFinis==1){
	    		print("\t\tCalculs FINIS\n\r");
	    		packetReady = 1;
	    		calculsFinis = 0;
	    		etatM1 = 3;
	    	}
	    	else
	    	{
	    		print("\t\tCalculs PAS FINIS\n\r");
	    		packetReady=0;
	    		etatM1=2;
	    	}
	    	break;
		}
		case 3:
		{
			print("etatM1 = 3\n\r");
			print("\t\tEnvoie Packet\n\r");
			sendPacket(bufferChar, sizeof(bufferChar), context.buffer);
			//affichageChar(context.buffer, sizeof(bufferChar));
			etatM1=0;
			break;

		}
		default:
			xil_printf("machine1:\tWallah ca va mal sa mere mon ami\n\r");
			break;
	}
}

void machine2(void){
	//int c[120];
	switch(etatM2){
		case 0:
		{
			if(startWriting==1)
			{
				//print("etatM2 = 0\n\r");
				etatM2=1;
				startWriting = 0;
			}
			break;
		}
		case 1:
		{
			print("etatM2 = 1\n\r");
			writeMyipReg(&acceleration, sizeof(bufferEntiers)/4);
			etatM2 = 2;
			break;
		}
		case 2:
		{
			print("etatM2 = 2\n\r");
			intToChar(moyenne, sizeof(moyenne)/4, bufferChar);
			calculsFinis = 1;
			etatM2 = 0;
			break;
		}
		default:
			xil_printf("machine2:\tWallah ca va mal sa mere mon ami\n\r");
			break;
	}
}


int_t main(void)
{

   XGpio_Initialize(&outputLED, XPAR_AXI_GPIO_0_DEVICE_ID);
   XGpio_SetDataDirection(&outputLED, 2, 0x0); //Fixer la direction du port 1 de l'AXI_GPIO_1 comme output

   uartInit();

   context.socket = 0;

   //Create user task
   print("\n\rSup dude\n\r");
   SetupInterruptSystem();
   systemInit();
   netTaskInit();
   simTableauChar(context.buffer, sizeof(bufferEntiers));

   while(1){

	   blinkTask();
	   uartTask();
	   machine1();
	   machine2();
	   //affichageChar(context.buffer, sizeof(bufferChar));


	   //caller deballer paquet() et convertir en X ;
	   	   //faire un array de X, Y et Z fuck le reste

	   if(ReceivedCount){	//Changer le triggered RecievedCount par RecievedPAQUET
		   XUartLite_Send(&UartLite, /*RecvBuffer*/"" , ReceivedCount);
		   newPacket = 1;
		   print("\t\tnewPacket = 1\n\r");

		   /*simTableauChar(context.buffer, sizeof(bufferEntiers));
		   charToInt(context.buffer, sizeof(bufferEntiers)/4, bufferEntiers);
		   accelXYZ(bufferEntiers, &acceleration, sizeof(bufferEntiers)/4);
		   writeMyipReg(&acceleration, sizeof(bufferEntiers)/4);*/

		   ReceivedCount = 0;
	   }

	   netTask();
	   if(netInterface[0].linkState == 1){
		   if(!udpSocketUp){
			   	   udpAccelStart();
				   udpSocketUp = true;
		   }
		   else udpAccelTask(newPacket, packetReady);
	   }
	   else
	   {
		   if(udpSocketUp){
			   udpSocketUp = false;
		   }
	   }
   }
   //This function should never return
   return 0;
}
