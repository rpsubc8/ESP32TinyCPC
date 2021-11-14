#include "gbConfig.h"
#include "CPCem.h"
#include "GA.h"
#include "CRTC.h"
#include "gbGlobals.h"

int galines;
int gapal[16];
unsigned char gaborder;
unsigned char palchange;
unsigned char scrmode;

//unsigned char *ram;
unsigned char *lorom;
unsigned char *hirom[16];
unsigned char *readarray[4],*writearray[4];
int palentry;
//int ramconfig=0;
unsigned char ramconfig=0;
unsigned char ramconfigBefore=255;


unsigned char ramconfigs[8][4]=
{
 {0,1,2,3},
 {0,1,2,7},
 {4,5,6,7},
 {0,3,2,7},
 {0,4,2,3},
 {0,5,2,3},
 {0,6,2,3},
 {0,7,2,3}
};

#ifdef use_lib_mem_blocks
void updaterambanking()
{//Modo 2 bancos RAM
 #ifdef usb_lib_optimice_checkchange_bankswitch
  if ((ramconfig == ramconfigBefore)&&(curhrom == curhromBefore)) 
   return; 
  else
  { 
   ramconfigBefore = ramconfig;
   curhromBefore = curhrom;
  }
  //printf ("updaterambanking ramconfig:%d curhrom:%d\n",ramconfig,curhrom);
 #endif
 unsigned char idBlock=0; //Bloque 0(0-64KB) 1(64-128KB) RAM
 int offsBlock=0;
 idBlock = (ramconfigs[ramconfig&7][0]*0x4000)/0x10000; //div 65536
 offsBlock = (ramconfigs[ramconfig&7][0]*0x4000)%0x10000; //offset bloque
 
 if (!loromena) readarray[0]= ramArray[idBlock] + offsBlock; 
 else readarray[0]=lorom;
 writearray[0]= ramArray[idBlock] + offsBlock;
 
 idBlock = ((ramconfigs[ramconfig&7][1]*0x4000)-0x4000)/0x10000;
 offsBlock = ((ramconfigs[ramconfig&7][1]*0x4000)-0x4000)%0x10000; 
 readarray[1]= ramArray[idBlock] + offsBlock;
 writearray[1]= ramArray[idBlock] + offsBlock;
 
 idBlock = ((ramconfigs[ramconfig&7][2]*0x4000)-0x8000)/0x10000;
 offsBlock = ((ramconfigs[ramconfig&7][2]*0x4000)-0x8000)%0x10000;
 readarray[2]= ramArray[idBlock] + offsBlock; 
 writearray[2]= ramArray[idBlock] + offsBlock;
        
 if (!hiromena)
 {
  idBlock = ((ramconfigs[ramconfig&7][3]*0x4000)-0xC000)/0x10000;
  offsBlock = ((ramconfigs[ramconfig&7][3]*0x4000)-0xC000)%0x10000;
  readarray[3]= ramArray[idBlock] + offsBlock;
 }
 else
 {
  //if (hirom[curhrom] == NULL)
  // printf ("Soy nulo updaterambanking %d\n",curhrom);
  if (hirom[curhrom] != NULL)
  {//JJ memoria Flash rom
   readarray[3]=hirom[curhrom]-0xC000;
  }
  else
  {
   readarray[3]=hirom[0]-0xC000; //fuerzo a rom0
  }
 }
 idBlock = ((ramconfigs[ramconfig&7][3]*0x4000)-0xC000)/0x10000;
 offsBlock = ((ramconfigs[ramconfig&7][3]*0x4000)-0xC000)%0x10000;
 writearray[3]= ramArray[idBlock] + offsBlock;
}
#else
void updaterambanking()
{
 #ifdef usb_lib_optimice_checkchange_bankswitch
  if ((ramconfig == ramconfigBefore)&&(curhrom == curhromBefore)) 
   return; 
  else
  { 
   ramconfigBefore = ramconfig;
   curhromBefore = curhrom;
  }
  //printf ("updaterambanking ramconfig:%d curhrom:%d\n",ramconfig,curhrom); 
 #endif        
        if (!loromena) readarray[0]=ram+(ramconfigs[ramconfig&7][0]*0x4000);
        else           readarray[0]=lorom;
        writearray[0]=ram+(ramconfigs[ramconfig&7][0]*0x4000);
        readarray[1]=ram+((ramconfigs[ramconfig&7][1]*0x4000)-0x4000);
        writearray[1]=ram+((ramconfigs[ramconfig&7][1]*0x4000)-0x4000);
        readarray[2]=ram+((ramconfigs[ramconfig&7][2]*0x4000)-0x8000);
        writearray[2]=ram+((ramconfigs[ramconfig&7][2]*0x4000)-0x8000);
        if (!hiromena) readarray[3]=ram+((ramconfigs[ramconfig&7][3]*0x4000)-0xC000);
        else
        {
         //if (hirom[curhrom] == NULL)
         // printf ("Soy nulo updaterambanking %d\n",curhrom);
         if (hirom[curhrom] != NULL)
         {//JJ memoria Flash rom
          readarray[3]=hirom[curhrom]-0xC000;
         }
         else
         {
          readarray[3]=hirom[0]-0xC000; //fuerzo a rom0
         }
        }
        writearray[3]=ram+((ramconfigs[ramconfig&7][3]*0x4000)-0xC000);
}
#endif

#ifdef use_lib_mem_blocks
void writega(unsigned short a, unsigned char v)
{     
 switch (v&0xC0)
 {
  case 0x00: //Pen selection
   palentry=v&31;
   break;
  case 0x40: //Pen set
   if (palentry&16) gaborder=(v&31)+1;
   else             gapal[palentry]=(v&31)+1;
   palchange=1;
   break;
  case 0x80: //Screen mode and ROM configuration
   loromena=!(v&4);
   hiromena=!(v&8);
   if ((v&3)!=scrmode)
   {
    palchange=1;
   }
   scrmode=v&3;
   if (v&4)
   {
    if (ramconfig==2)
    {
     readarray[0]= ramArray[1];  //ram+0x10000;
    }
    else
    {
     readarray[0]= ramArray[0]; //ram;
    }
   }
   else
    readarray[0]=lorom;
   if (v&8)
   {
    if (ramconfig && ramconfig<4)
    {
     readarray[3]= ramArray[1];//ram+0x10000;
    }
    else
    {
     readarray[3]= ramArray[0]; //ram;
    }
   }
   else
   {
    //if (hirom[curhrom] == NULL)
    // printf ("Soy nulo writega %d\n",curhrom);                    
    if (hirom[curhrom] != NULL)
    {//JJ flash rom
     readarray[3]=hirom[curhrom]-0xC000;
    }
    else
    {
     readarray[3]= hirom[0]-0xC000; //fuerzo a rom0
    }                 
   }
   if (v&16)
   {
    galines&=~32;
    intreq=0;
   }
   updaterambanking();
   break;
  case 0xC0: //RAM banking
   if (model<2) ramconfig=0;
   else         ramconfig=v&7;
   updaterambanking();
   break;
 }        
}
#else
void writega(unsigned short a, unsigned char v)
{
        switch (v&0xC0)
        {
                case 0x00: /*Pen selection*/
                palentry=v&31;
                break;
                case 0x40: /*Pen set*/
                //printf("pal %i = %i line %i %i lines since vsync disp line %i\n",palentry,v&31,(vc<<3)|sc,linessincevsync,crtcline-16);
                if (palentry&16) gaborder=(v&31)+1;
                else             gapal[palentry]=(v&31)+1;
                palchange=1;
                break;
                case 0x80: /*Screen mode and ROM configuration*/
                loromena=!(v&4);
                hiromena=!(v&8);
                if ((v&3)!=scrmode)
                {
//                        printf("Mode set %i line %i %i %i\n",v&3,vc,linessincevsync,crtcline-16);
                        palchange=1;
                }
                scrmode=v&3;
                if (v&4)
                {
                        if (ramconfig==2) readarray[0]=ram+0x10000;
                        else              readarray[0]=ram;
                }
                else
                   readarray[0]=lorom;
                if (v&8)
                {
                        if (ramconfig && ramconfig<4) readarray[3]=ram+0x10000;
                        else                          readarray[3]=ram;
                }
                else
                {
                 //if (hirom[curhrom] == NULL)
                 // printf ("Soy nulo writega %d\n",curhrom);                    
                 if (hirom[curhrom] != NULL)
                 {//JJ flash rom
                  readarray[3]=hirom[curhrom]-0xC000;
                 }
                 else
                 {
                  readarray[3]= hirom[0]-0xC000; //fuerzo a rom0
                 }                 
                }
                if (v&16)
                {
                        galines&=~32;
                        intreq=0;
//                        printf("gaclear %02X line %i\n",v,(vc<<3)|sc);
                }
                updaterambanking();
//                printf("Ena now %i, cur %i\n",hiromena,curhrom);
                break;
                case 0xC0: /*RAM banking*/
//                printf("RAM config %i\n",v&7);
                if (model<2) ramconfig=0;
                else         ramconfig=v&7;
                updaterambanking();
                break;
        }
}
#endif

void cleargacount()
{
 galines&=~32;
}
