#include <packetTask.h>
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
//#include "cyclone_tcp/ext_int_driver.h"
#include "myip_S4e_4reg.h"
#include "xil_io.h"
#include "xil_printf.h"


#define APP_USE_SLAAC DISABLED

#define INTC_DEVICE_ID			XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
#define MY_IP_BASE_ADDRESS_0     	XPAR_CALCUL_MOYENNE_MYIP_S4E_4REG_0_S00_AXI_BASEADDR //Adresse de base, MyIP0 - axe X
#define MY_IP_BASE_ADDRESS_1     	XPAR_CALCUL_MOYENNE_MYIP_S4E_4REG_1_S00_AXI_BASEADDR //Adresse de base, MyIP1 - axe Y
#define MY_IP_BASE_ADDRESS_2     	XPAR_CALCUL_MOYENNE_MYIP_S4E_4REG_2_S00_AXI_BASEADDR //Adresse de base, MyIP2 - axe Z
#define MYIP_REG_0  			MYIP_S4E_4REG_S00_AXI_SLV_REG0_OFFSET
#define MYIP_REG_1  			MYIP_S4E_4REG_S00_AXI_SLV_REG1_OFFSET
#define MYIP_REG_2  			MYIP_S4E_4REG_S00_AXI_SLV_REG2_OFFSET

//Definition de structures
struct axes{
    int X[40];
    int Y[40];
    int Z[40];
};


//Definition de fonctions
void accelXYZ(int *tab, struct axes *acceleration, int size){
    for(int count=0; count<4; count++){
        for(int i=0;i<(size/3);i++){
             if(count==1){
                acceleration->X[i] = tab[i];
            }
            if(count==2){
                acceleration->Y[i] = tab[40+i];
            }
             if(count==3){
                acceleration->Z[i] = tab[80+i];
            }
        }
    }
}

void intToChar(u32 *intTab, int count, unsigned char *Ctab){
    for(int i=0;i<count;i++){
        Ctab[4*i] =     intTab[i] >> 24;
        Ctab[(4*i)+1] = intTab[i] >> 16;
        Ctab[(4*i)+2] = intTab[i] >> 8;
        Ctab[(4*i)+3] = intTab[i] >> 0;
        //printf("%d : %.8X\t%.2X %.2X %.2X %.2X\n\r", i, intTab[i], Ctab[(4*i)], Ctab[(4*i)+1], Ctab[(4*i)+2], Ctab[(4*i)+3]);
    }
}

void charToInt(char *Ctab, int count, int *intTab){
    for(int i=0;i<count;i++){
        intTab[i] = ((int)(Ctab[(4*i)] << 24) + (Ctab[(4*i)+1] << 16) + (Ctab[(4*i)+2] << 8) + Ctab[(4*i)+3]);
        //printf("tab[%d] : %X\n\r", i, intTab[i]);
    }
}

void simTableauChar(unsigned char *tableau, int size){
    unsigned char val = '0';
    for(int i=0; i<size; i++){
        tableau[i] = val;
        val++;
    }
}

void createPacket(u32 *moyenne, u32 *somme, u32 *valeur, int Index, int axe){
		switch(axe){
				case 1:
					valeur[Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_0, MYIP_REG_0);
					somme[Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_0, MYIP_REG_2);
					moyenne[Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_0, MYIP_REG_1); //<---------------------------
					xil_printf("ValeurX = %X\tSommeX = %X\tMoyenneX = %X\n\r",  valeur[Index], somme[Index], moyenne[Index]);
					//xil_printf("valeurX[%d] = %X\t\taxe = %d\n\r", Index, valeur[Index], axe);
					break;
				case 2:
					valeur[40+Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_1, MYIP_REG_0);
					somme[40+Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_1, MYIP_REG_2);
					moyenne[40+Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_1, MYIP_REG_1);
					xil_printf("ValeurY = %X\tSommeY = %X\tMoyenneY = %X\n\r",  valeur[40+Index], somme[40+Index], moyenne[40+Index]);
					//xil_printf("valeurY[%d] = %X\t\taxe = %d\n\r", Index, valeur[40+Index], axe);
					break;
				case 3:
					valeur[80+Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_2, MYIP_REG_0);
					somme[80+Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_2, MYIP_REG_2);
					moyenne[80+Index] =  MYIP_S4E_4REG_mReadReg(MY_IP_BASE_ADDRESS_2, MYIP_REG_1);
					xil_printf("ValeurZ = %X\tSommeZ = %X\tMoyenneZ = %X\n\r",  valeur[80+Index], somme[80+Index], moyenne[80+Index]);
					//xil_printf("valeurZ[%d] = %X\t\taxe = %d\n\r", Index, valeur[80+Index], axe);
					break;
				default:
					xil_printf("Index : %d\t\tErreur le cave\t\taxe : %d\n\r\n\r", Index, axe);
					break;
			}
}

void writeMyipReg(struct axes *acceleration, int count){
	for(int j=0; j<4; j++){
		for(int i=0; i<count/3; i++){
			if(j==1){
				  MYIP_S4E_4REG_mWriteReg(MY_IP_BASE_ADDRESS_0, MYIP_REG_0, acceleration->X[i]);
				  xil_printf("");
			}
			if(j==2){
				  MYIP_S4E_4REG_mWriteReg(MY_IP_BASE_ADDRESS_1, MYIP_REG_0, acceleration->Y[i]);
				  xil_printf("");
			}
			if(j==3){
				  MYIP_S4E_4REG_mWriteReg(MY_IP_BASE_ADDRESS_2, MYIP_REG_0, acceleration->Z[i]);
				  xil_printf("");
			}
		}
	}
}


void sendPacket(unsigned char *moyenne, int count, char *Cbuffer){
	for(int i=0; i<count; i++){
		Cbuffer[i] = moyenne[i];
	}
}


void affichage(int *tableau, int count){
	for(int i=0; i<count; i++){
		xil_printf("tableau[%d] = %.8X\n\r", i, tableau[i]);
	}
}

void affichageChar(unsigned char *tableau, int count){
	for(int i=0; i<count; i++){
		xil_printf("tableau[%d] = %X\n\r", i, tableau[i]);
	}
}

void affichageXYZ(struct axes *acceleration, int count){
	for(int i=0; i<count; i++){
		xil_printf("accelX[%d] = %X\t\taccelY[%d] = %X\t\taccelZ[%d] = %X\n\r", i, acceleration->X[i], i, acceleration->Y[i], i, acceleration->Z[i]);
	}
}

//int getAxe(int axe){
//
//	axe=3;
//	return axe;
//}
