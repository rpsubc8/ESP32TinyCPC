#include "gbConfig.h"
#include <stdio.h>
#include "CPCem.h"
#include "PSG.h"
#include "gbGlobals.h"

unsigned char psgdat;
unsigned char psgmode;
unsigned char loromena,hiromena;
unsigned char curhrom=0;
unsigned char curhromBefore=255;

unsigned char psgreg,psgregs[16]; //no necesito sonido
#ifdef use_lib_sound_ay8912
 unsigned char psgvol[4];
 unsigned char psgvolBefore[4];
 //int psglatch[5];
#endif 




void updatepsg()
{
//No necesito sonido aun
 switch (psgmode)
 {
  case 1:
   psgdat=psgregs[psgreg];
   break;
  case 2:
   psgregs[psgreg]=psgdat;
   //JJ if (psgreg==13) psgenv=psgdat&15;// no necesito sonido
   break;
  case 3:
   psgreg=psgdat;
   break;
 }
}

//int cpckeys[16][8]=
unsigned char cpckeys[16][8]=
{//optimizado
 {1,1,1,1,1,1,1,1},
 {1,0xFF,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,1,1},
 {1,1,1,1,1,1,0xFF,1}, //Activado bit 5 raton derecho
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};


void updatekeys(int line)
{
 int c;
 psgregs[14]=0xFF;
 for (c=0;c<8;c++)
 {
  if ((cpckeys[line][c] == 0x00) && (cpckeys[line][c]!=0xFF))
  {
   psgregs[14]&=~(1<<c);
  }  
 } 
 //printf("Updated key line %i %02X\n",line,psgregs[14]);
}