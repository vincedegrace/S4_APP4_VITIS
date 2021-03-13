#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "myip_S4e_4reg.h"
//#include "cyclone_tcp/ext_int_driver.h"
#include "xil_io.h"
#include "xil_printf.h"


//Declaration Structures
struct axes;

//Declaration de fonctions
void simTableauChar(unsigned char *tableau, int size);
void charToInt(char *Ctab, int count, int *intTab);
void accelXYZ(int *tab, struct axes *acceleration, int size);
void intToChar(u32 *intTab, int count, unsigned char *Ctab);
void writeMyipReg(struct axes *acceleration, int count);
void createPacket(u32 *moyenne, u32 *somme, u32 *valeur, int Index, int count);
void sendPacket(unsigned char *moyenne, int count, char *Cbuffer);
void affichage(int *tableau, int count);
void affichageChar(unsigned char *tableau, int count);
void affichageXYZ(struct axes *acceleration, int count);
//int getAxe(int axe);
