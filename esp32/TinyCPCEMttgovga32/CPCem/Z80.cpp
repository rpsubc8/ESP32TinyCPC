//offset 703 + 45056 = B2BF
#include "gbConfig.h"


#include <stdlib.h>
#include <stdio.h>
#include "CPCem.h"
#include "DAA.h"
#include "Z80.h"
#include <string.h>
#include "FDC.h"
#include "CRTC.h"
#include "GA.h"
#include "CRTC.h"
#include "8255.h"
#include "gbGlobals.h"
#include "dataFlash/gbrom.h"

z80reg af,bc,de,hl,ix,iy,ir,saf,sbc,sde,shl;
unsigned short pc=0;
unsigned short sp;
int iff1,iff2;
int intreq;
int im;


int c8d1=0;

//JJ int model;
int rhigh=0;
//JJ int vc,sc;
unsigned short ooopc,oopc,opc;

int output=0;
int cycles=0;
int tempc;
int ins=0;

//JJ unsigned char znptable[256];
//JJ unsigned char znptable16[65536];



#ifndef use_lib_lookup_znptable
 //********************************************************
 inline unsigned char Get_no_lookup_znptable(unsigned char c)
 {
  unsigned char d,g;
  unsigned char f= 0;
  if (c == 0)
   return 0x44;
  d= (c&0xA8);
  for (g=0;g<8;g++)
  {
   if (c&1) f++;
   c>>= 1;
  }
  if (!(f&1))
   d|= 0x04;
  return d;
 }
#endif

#ifndef use_lib_lookup_znptable16
 //********************************************************
 inline unsigned char Get_no_lookup_znptable16(unsigned short int c)
 {
  unsigned char d= 0;
  unsigned char f= 0;
  unsigned char g;
  if (c == 0)
   return 0x44;       
  if (c&0x8000) d= 0x80;
  for (g=0;g<16;g++)
  {
   if (c&1) f++;
   c>>=1;
  }
  if (!(f&1))
   d|= 0x04;
  return d; 
 }
#endif

//void makeznptable()
//{
// int c,d,e,f,g;
// for (c=0;c<256;c++)
// {
//  d=0;
//  d|=(c&0xA8);
//  e=c;
//  f=0;
//  for (g=0;g<8;g++)
//  {
//   if (e&1) f++;
//   e>>=1;
//  }
//  if (!(f&1))
//   d|=4;
//  znptable[c]=d;
// }
// znptable[0]|=0x40;
// for (c=0;c<65536;c++)
// {
//  d=0;
//  if (c&0x8000) d|=0x80;
//  e=c;
//  f=0;
//  for (g=0;g<16;g++)
//  {
//   if (e&1) f++;
//   e>>=1;
//  }
//  if (!(f&1))
//   d|=4;
//  znptable16[c]=d;
// }
// znptable16[0]|=0x40;
//}

void dumpram()
{
/* FILE *f=fopen("ram.dmp","wb");
 fwrite(ram,65536,1,f);
 fclose(f);
 f=fopen("ram2.dmp","wb");
 fwrite(ram+0x10000,65536,1,f);
 fclose(f);*/
}

void rebuildmem()
{
 #ifdef use_lib_mem_blocks
  readarray[0]=lorom;
  readarray[1]=ramArray[0];
  readarray[2]=ramArray[0];
  readarray[3]=ramArray[0];
  writearray[0]=ramArray[0];
  writearray[1]=ramArray[0];
  writearray[2]=ramArray[0];
  writearray[3]=ramArray[0]; 
 #else        
  readarray[0]=lorom;
  readarray[1]=ram;
  readarray[2]=ram;
  readarray[3]=ram;
  writearray[0]=ram;
  writearray[1]=ram;
  writearray[2]=ram;
  writearray[3]=ram;
 #endif 
}

/*void ImprimeTablaPrecalculada()
{
 int cont=0;
 printf("Table znptable\n");
 for (int i=0;i<256;i++)
 {
  printf("0x%02x,",znptable[i]);
  cont++;
  if (cont>=16)
  {
   cont=0;
   printf("\n");
  }
 }
 cont=0;
 printf("Table znptable16\n");
 for (int i=0;i<65536;i++)
 {
  printf("0x%02x,",znptable16[i]);
  cont++;
  if (cont>=16)
  {
   cont=0;
   printf("\n");
  }     
 }
}*/

void initz80()
{
 int c;
 //atexit(dumpram);
 //makeznptable();
 //ImprimeTablaPrecalculada();
 //Para la rom en memoria dinamica
 //JJ lorom=(unsigned char *)calloc(0x4000,1);
 for (c=0;c<16;c++)
 {
  //hirom[c]=(unsigned char *)calloc(0x4000,1);
  //memset(hirom[c],0,16384);  
  //hirom[c]= NULL;            
  hirom[c]= (unsigned char*)gb_rom_464_1;
 }
 #ifdef use_lib_mem_blocks  
  for (c=0;c<2;c++)
   memset(ramArray[c],1,0x10000);  
 #else
  #ifdef use_lib_cheat_128k
   memset(ram,1,0x10000);
  #else
   #ifdef use_lib_fix_psram_128k
    memset(ram,1,0x10000); //Modo 128K psram
   #else
    memset(ram,1,0x10000); //Modo 64K
   #endif 
  #endif 
 #endif 
 //JJ ram=(unsigned char *)calloc(0x20000,1); //Creado en Setup
 rebuildmem();
}

//Lee desde Flash todas las roms
void loadroms2FlashModel()
{
 //printf("Modelo %d\n",aModel);
 if (model == 2)
 {
  #if defined(use_lib_128k) || defined(use_lib_cheat_128k) || defined(use_lib_fix_psram_128k)
   lorom = (unsigned char*)gb_rom_6128_0; //16KB primeros 
   hirom[0]= (unsigned char*)gb_rom_6128_1;
  #endif 
  //memcpy(lorom,gb_rom_6128,16384);
  //memcpy(hirom[0],&gb_rom_6128[16384],16384);
 }
 else 
 {
  if (model == 1)
  {   
   lorom = (unsigned char*)gb_rom_664_0;
   hirom[0] = (unsigned char*)gb_rom_664_1;
   //memcpy(lorom,gb_rom_664,16384);
   //memcpy(hirom[0],&gb_rom_664[16384],16384);   
  }
  else
  {
   lorom = (unsigned char*)gb_rom_464_0;
   hirom[0] = (unsigned char*)gb_rom_464_1;
   //memcpy(lorom,gb_rom_464,16384);
   //memcpy(hirom[0],gb_rom_464_1,16384);      
  }
 }
 hirom[7] = (unsigned char *)gb_rom_amsdos;
 //memcpy(hirom[7],gb_rom_amsdos,16384);
 ramconfigBefore= 255; //force bank switch
 curhromBefore= 255; 
}

/*void loadroms()
{
        FILE *f;
        if (model==2)      f=fopen("6128.rom","rb");
        else if (model==1) f=fopen("664.rom","rb");
        else               f=fopen("464.rom","rb");
        fread(lorom,16384,1,f);
        fread(hirom[0],16384,1,f);
        fclose(f);
        f=fopen("amsdos.rom","rb");
        fread(hirom[7],16384,1,f);
        fclose(f);
}*/

void resetz80()
{
        pc=0;
        iff1=iff2=0;
        #ifdef use_lib_mem_blocks
         for (int i=0;i<2;i++)
          memset(ramArray[i],0,0x10000); //65536 64 KB
         ram = ramArray[0]; //Inicio
        #else
         #ifdef use_lib_cheat_128k
          memset(ram,0,0x10000); //truco 122880 bytes
         #else
          #ifdef use_lib_fix_psram_128k
           memset(ram,0,0x10000); //128K psram
          #else
           memset(ram,0,0x10000); //hay que setear memoria 64K
          #endif
         #endif
        #endif
        loromena=1;
        hiromena=0;
        galines=0;
        writega(0,0xC0);
}

//if (a==0xBE60 && v==0xC3) { dumpregs(); output=1; } 
#define readmem(a)    readarray[(a&0xFFFF)>>14][a&0xFFFF]
#define writemem(a,v) writearray[(a&0xFFFF)>>14][a&0xFFFF]=v;
#define push(v) { sp--; writearray[sp>>14][sp]=v; }

unsigned char readmemf(unsigned short a)
{
        return readarray[(a&0xFFFF)>>14][a&0xFFFF];
}
unsigned char readmemf2(unsigned short a)
{
        return writearray[(a&0xFFFF)>>14][a&0xFFFF];
}

inline unsigned char pull()
{
        unsigned char temp=readarray[sp>>14][sp];
        sp++;
        return temp;
}

inline unsigned char z80in(unsigned short a)
{
        if (!(a&0x4000)) return readcrtc(a);
        if (!(a&0x800)) return read8255(a);
        if (!(a&0x480)) return readfdc(a);
        return 0xFF;
}

inline void z80out(unsigned short a, unsigned char v)
{
        if (!(a&0x8000)) writega(a,v);
        if (!(a&0x4000)) writecrtc(a,v);
        if (!(a&0x2000))
        {
                curhrom=v&15;
//                printf("Current hirom = %i, ena %i\n",hiromena,curhrom);
                if (hiromena)
                {
                 //if (hirom[curhrom] == NULL)
                 // printf ("Soy nulo z80out %d\n",curhrom);                             
                 if (hirom[curhrom] != NULL)
                 {//JJ flash rom
                  readarray[3]=hirom[curhrom]-0xC000;
                 }
                 else
                 {
                  readarray[3]= hirom[0]-0xC000; //fuerzo a rom existente 0
                 }
                }
        }
        if (!(a&0x800)) write8255(a,v);
        if (!(a&0x480)) writefdc(a,v);
}

inline void setzn(unsigned char v)
{
 af.b.l&=~(N_FLAG|Z_FLAG|V_FLAG|0x28|C_FLAG);
 #ifdef use_lib_lookup_znptable
  af.b.l|=znptable[v];
 #else
  af.b.l|= Get_no_lookup_znptable(v);
 #endif 
}

inline void setznc(unsigned char v)
{
 af.b.l&=~(N_FLAG|Z_FLAG|V_FLAG|0x28);
 #ifdef use_lib_lookup_znptable
  af.b.l|=znptable[v];
 #else
  af.b.l|= Get_no_lookup_znptable(v);
 #endif 
}
signed short temps;

inline void setadd(unsigned char a, unsigned char b)
{
 #ifdef use_lib_lookup_znptable        
  af.b.l=znptable[(a+b)&0xFF];
 #else
  af.b.l= Get_no_lookup_znptable(((a+b)&0xFF));
 #endif 
 af.b.l&=~(C_FLAG|V_FLAG);
 if ((a+b)&0x100) af.b.l|=C_FLAG;
 temps=(signed short)(signed char)a+(signed short)(signed char)b;
 if (temps<-128 || temps>127) af.b.l|=V_FLAG;
 if (((a&0xF)+(b&0xF))&0x10) af.b.l|=H_FLAG;
}
       
inline void setadc(unsigned char a, unsigned char b)
{
 #ifdef use_lib_lookup_znptable
  af.b.l=znptable[(a+b+tempc)&0xFF];
 #else
  af.b.l= Get_no_lookup_znptable(((a+b+tempc)&0xFF));
 #endif 
 af.b.l&=~V_FLAG;
 if ((a+b+tempc)&0x100) af.b.l|=C_FLAG;
 temps=(signed short)(signed char)a+(signed short)(signed char)b;
 if (temps<-128 || temps>127) af.b.l|=V_FLAG;
 if (((a&0xF)+(b&0xF))&0x10) af.b.l|=H_FLAG;
}

inline void setadc16(unsigned short a, unsigned short b)
{
 signed long temps2;
 #ifdef use_lib_lookup_znptable16
  af.b.l=znptable16[(a+b+tempc)&0xFFFF];
 #else
  af.b.l= Get_no_lookup_znptable16(((a+b+tempc)&0xFFFF));
 #endif 
 af.b.l&=~V_FLAG;
 temps2=(signed short)a+(signed short)b;
 if (temps2<-32768 || temps2>32767) af.b.l|=V_FLAG;
 if (((a&0xFFF)+(b&0xFFF))&0x1000) af.b.l|=H_FLAG;
 if ((a+b+tempc)&0x10000) af.b.l|=C_FLAG;
}

inline void setadd16(unsigned short a, unsigned short b)
{
        af.b.l&=~(C_FLAG|S_FLAG|H_FLAG);
        if ((a+b)&0x10000) af.b.l|=C_FLAG;
        if ((a&0x1800)==0x800 && ((a+b)&0x1800)==0x1000) af.b.l|=H_FLAG;
}

inline void setcp(unsigned char a, unsigned char b)
{
 #ifdef use_lib_lookup_znptable
  af.b.l=znptable[(a-b)&0xFF]|S_FLAG;
 #else
  af.b.l= Get_no_lookup_znptable(((a-b)&0xFF))|S_FLAG;
 #endif 
 af.b.l&=~V_FLAG;
 temps=(signed short)(signed char)a-(signed short)(signed char)b;
 if (temps<-128 || temps>127) af.b.l|=V_FLAG;
 if (!(a&8) && ((a+b)&8)) af.b.l|=H_FLAG;
 if (a<b) af.b.l|=C_FLAG;
}

inline void setdec(unsigned char v)
{
 af.b.l&=1;
 #ifdef use_lib_lookup_znptable
  af.b.l|=znptable[v]|S_FLAG;
 #else
  af.b.l|= Get_no_lookup_znptable(v)|S_FLAG;
 #endif 
 if (v==0x80) af.b.l|=P_FLAG;
 else         af.b.l&=~P_FLAG;
 if (((v-1)&0x10) && !(v&0x10)) af.b.l|=H_FLAG;
}

inline void setinc(unsigned char v)
{
 af.b.l&=0x29;
 #ifdef use_lib_lookup_znptable
  af.b.l|=znptable[v];
 #else
  af.b.l|= Get_no_lookup_znptable(v);
 #endif 
 if (v==0x7F) af.b.l|=P_FLAG;
 else         af.b.l&=~P_FLAG;
 if (((v-1)&0x8) && !(v&0x8)) af.b.l|=H_FLAG;
}

inline void setsub(unsigned char a, unsigned char b)
{
 #ifdef use_lib_lookup_znptable
  af.b.l=znptable[(a-b)&0xFF]|S_FLAG;
 #else
  af.b.l= Get_no_lookup_znptable(((a-b)&0xFF))|S_FLAG;
 #endif 
 af.b.l&=~V_FLAG;
 temps=(signed short)(signed char)a-(signed short)(signed char)b;
 if (temps<-128 || temps>127) af.b.l|=V_FLAG;
 if (!(a&8) && ((a-b)&8)) af.b.l|=H_FLAG;
 if (a<b) af.b.l|=C_FLAG;
}

inline void setsbc(unsigned char a, unsigned char b)
{
 #ifdef use_lib_lookup_znptable
  af.b.l=znptable[(a-(b+tempc))&0xFF]|S_FLAG;
 #else
  af.b.l= Get_no_lookup_znptable(((a-(b+tempc))&0xFF))|S_FLAG;
 #endif 
 af.b.l&=~V_FLAG;
 temps=(signed short)(signed char)a-(signed short)(signed char)b;
 if (temps<-128 || temps>127) af.b.l|=V_FLAG;
 if (!(a&8) && ((a-b)&8)) af.b.l|=H_FLAG;
 if (a<(b+tempc)) af.b.l|=C_FLAG;
}

inline void setsbc16(unsigned short a, unsigned short b)
{
 signed long temps2;
 b+=(af.b.l&C_FLAG);
 af.b.l&=0x28;
 #ifdef use_lib_lookup_znptable16
  af.b.l|=znptable16[(a-b)&0xFFFF]|S_FLAG;
 #else
  af.b.l|= Get_no_lookup_znptable16(((a-b)&0xFFFF))|S_FLAG;
 #endif 
 af.b.l&=~V_FLAG;
 temps2=(signed long)(signed short)a-(signed long)(signed short)b;
 if (temps2<-32768 || temps2>32767) af.b.l|=V_FLAG;
 if (!(a&0x800) && ((a-b)&0x800)) af.b.l|=H_FLAG;
 if (a<b) af.b.l|=C_FLAG;
}

void dumpregs()
{
/*JJ
        printf("AF =%04X BC =%04X DE =%04X HL =%04X IX=%04X IY=%04X\n",af.w,bc.w,de.w,hl.w,ix.w,iy.w);
        printf("AF'=%04X BC'=%04X DE'=%04X HL'=%04X IR=%04X\n",saf.w,sbc.w,sde.w,shl.w,ir.w);
        printf("%c%c%c%c%c%c   PC =%04X SP =%04X %i ins IFF1=%i IFF2=%i\n",(af.b.l&N_FLAG)?'N':' ',(af.b.l&Z_FLAG)?'Z':' ',(af.b.l&H_FLAG)?'H':' ',(af.b.l&V_FLAG)?'V':' ',(af.b.l&S_FLAG)?'S':' ',(af.b.l&C_FLAG)?'C':' ',pc,sp,ins,iff1,iff2);
        printf("%04X %04X %04X\n",opc,oopc,ooopc);
        */
}


//JJ unsigned short opc,oopc;

int a92c=0;
//JJunsigned short opc,oldpc;
unsigned short oldpc;

void execz80()
{
        int c;
        unsigned char opcode;
        unsigned short addr,tempw;
        unsigned char temp,temp2;
        signed char offset;
        for (c=0;c<312;c++)
        {
//                if ((c&63)==63) intreq=1;
                pollline();
                cpuline++;
                cycles+=256;
                while (cycles>0)
                {
                        oldpc=opc;
                        opc=pc;
                        opcode=readmem(pc); pc++;
                        tempc=af.b.l&C_FLAG;
                        switch (opcode)
                        {
                                // t 4
                                case 0x00: /*NOP*/
                                cycles-=4;
                                break;

                                // t 12
                                case 0x01: /*LD BC,xxxx*/
                                bc.b.l=readmem(pc); pc++;
                                bc.b.h=readmem(pc); pc++;
                                cycles-=12;
                                break;

                                // t 8
                                case 0x02: /*LD (BC),A*/
                                writemem(bc.w,af.b.h);
                                cycles-=8;
                                break;

                                // t 4
                                case 0x03: /*INC BC*/
                                bc.w++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x04: /*INC B*/
                                bc.b.h++;
                                setinc(bc.b.h);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x05: /*DEC B*/
                                bc.b.h--;
                                setdec(bc.b.h);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x06: /*LD B,xx*/
                                bc.b.h=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x07: /*RLCA*/
                                if (af.b.h&0x80) af.b.l|=C_FLAG;
                                else             af.b.l&=~C_FLAG;
                                af.b.h<<=1;
                                if (af.b.l&C_FLAG) af.b.h|=1;
                                //setznc(af.b.h);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x08: /*EX AF,AF'*/
                                tempw=af.w; af.w=saf.w; saf.w=tempw;
                                cycles-=4;
                                break;

                                // t 12
                                case 0x09: /*ADD HL,BC*/
                                setadd16(hl.w,bc.w);
                                hl.w+=bc.w;
                                cycles-=12;
                                break;

                                // t 8
                                case 0x0A: /*LD A,(BC)*/
                                af.b.h=readmem(bc.w);
                                cycles-=8;
                                break;

                                // t 4
                                case 0x0B: /*DEC BC*/
                                bc.w--;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x0C: /*INC C*/
                                bc.b.l++;
                                setinc(bc.b.l);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x0D: /*DEC C*/
                                bc.b.l--;
                                setdec(bc.b.l);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x0E: /*LD C,xx*/
                                bc.b.l=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x0F: /*RRCA*/
                                if (af.b.h&1) af.b.l|=C_FLAG;
                                else          af.b.l&=~C_FLAG;
                                af.b.h>>=1;
                                if (af.b.l&C_FLAG) af.b.h|=0x80;
                                //setznc(af.b.h);
                                cycles-=4;
                                break;

                                // t 8 8
                                case 0x10: /*DJNZ*/
                                offset=(signed char)readmem(pc); pc++;
                                bc.b.h--;
                                if (bc.b.h)
                                {
                                        pc+=offset;
                                        cycles-=16;
                                }
                                else
                                {
                                cycles-=12;
                                }
                                break;

                                // t 12
                                case 0x11: /*LD DE,xxxx*/
                                de.b.l=readmem(pc); pc++;
                                de.b.h=readmem(pc); pc++;
                                cycles-=12;
                                break;

                                // t 8
                                case 0x12: /*LD (DE),A*/
                                writemem(de.w,af.b.h);
                                cycles-=8;
                                break;

                                // t 4
                                case 0x13: /*INC DE*/
                                de.w++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x14: /*INC D*/
                                de.b.h++;
                                setinc(de.b.h);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x15: /*DEC D*/
                                de.b.h--;
                                setdec(de.b.h);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x16: /*LD D,xx*/
                                de.b.h=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 8
                                case 0x17: /*RLA*/
                                if (af.b.h&0x80) af.b.l|=C_FLAG;
                                else             af.b.l&=~C_FLAG;
                                af.b.h<<=1;
                                if (tempc) af.b.h|=1;
                                //setznc(af.b.h);
                                cycles-=4;
                                break;

                                // t 16
                                case 0x18: /*JR*/
                                offset=(signed char)readmem(pc); pc++;
                                pc+=offset;
                                cycles-=12;
                                break;

                                // t 12
                                case 0x19: /*ADD HL,DE*/
                                setadd16(hl.w,de.w);
                                hl.w+=de.w;
                                cycles-=12;
                                break;

                                // t 8
                                case 0x1A: /*LD A,(DE)*/
                                af.b.h=readmem(de.w);
                                cycles-=8;
                                break;

                                // t 4
                                case 0x1B: /*DEC DE*/
                                de.w--;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x1C: /*INC E*/
                                de.b.l++;
                                setinc(de.b.l);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x1D: /*DEC E*/
                                de.b.l--;
                                setdec(de.b.l);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x1E: /*LD E,xx*/
                                de.b.l=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x1F: /*RRA*/
                                tempc=af.b.l&C_FLAG;
                                if (af.b.h&1) af.b.l|=C_FLAG;
                                else          af.b.l&=~C_FLAG;
                                af.b.h>>=1;
                                if (tempc) af.b.h|=0x80;
                                //setznc(af.b.h);
                                cycles-=4;
                                break;

                                // t 8 8
                                case 0x20: /*JR NZ*/
                                offset=(signed char)readmem(pc); pc++;
                                if (!(af.b.l&Z_FLAG))
                                {
                                        pc+=offset;
                                        cycles-=12;
                                }
                                else
                                {
                                cycles-=8;
                                }
                                break;

                                // t 12
                                case 0x21: /*LD HL,xxxx*/
                                hl.b.l=readmem(pc); pc++;
                                hl.b.h=readmem(pc); pc++;
                                cycles-=12;
                                break;

                                // t 24
                                case 0x22: /*LD (xxxx),HL*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                writemem(addr,hl.b.l);
                                writemem(addr+1,hl.b.h);
                                cycles-=20;
                                break;

                                // t 4
                                case 0x23: /*INC HL*/
                                hl.w++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x24: /*INC H*/
                                hl.b.h++;
                                setinc(hl.b.h);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x25: /*DEC H*/
                                hl.b.h--;
                                setdec(hl.b.h);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x26: /*LD H,xx*/
                                hl.b.h=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x27: /*DAA*/
                                addr=af.b.h;
                                if (af.b.l&C_FLAG) addr|=256;
                                if (af.b.l&H_FLAG) addr|=512;
                                if (af.b.l&S_FLAG) addr|=1024;
                                af.w=DAATable[addr];
                                cycles-=4;
                                break;

                                // t 8 8
                                case 0x28: /*JR Z*/
                                offset=(signed char)readmem(pc); pc++;
                                if (af.b.l&Z_FLAG)
                                {
                                        pc+=offset;
                                        cycles-=12;
                                }
                                else
                                {
                                 cycles-=8;
                                }
                                break;

                                // t 12
                                case 0x29: /*ADD HL,HL*/
                                setadd16(hl.w,hl.w);
                                hl.w+=hl.w;
                                cycles-=12;
                                break;

                                // t 24
                                case 0x2A: /*LD HL,(xxxx)*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                hl.b.l=readmem(addr);
                                hl.b.h=readmem(addr+1);
                                cycles-=20;
                                break;

                                // t 4
                                case 0x2B: /*DEC HL*/
                                hl.w--;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x2C: /*INC L*/
                                hl.b.l++;
                                setinc(hl.b.l);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x2D: /*DEC L*/
                                hl.b.l--;
                                setdec(hl.b.l);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x2E: /*LD L,xx*/
                                hl.b.l=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x2F: /*CPL*/
                                af.b.h^=0xFF;
                                af.b.l|=(H_FLAG|S_FLAG);
                                cycles-=4;
                                break;

                                // t 8 8
                                case 0x30: /*JR NC*/
                                offset=(signed char)readmem(pc); pc++;
                                if (!(af.b.l&C_FLAG))
                                {
                                        pc+=offset;
                                        cycles-=12;
                                }
                                else
                                {
                                 cycles-=8;
                                }
                                break;

                                // t 12
                                case 0x31: /*LD SP,xxxx*/
                                sp=readmem(pc); pc++;
                                sp|=(readmem(pc))<<8; pc++;
                                cycles-=12;
                                break;

                                // t 16
                                case 0x32: /*LD (xxxx),A*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                writemem(addr,af.b.h);
                                cycles-=16;
                                break;

                                // t 4
                                case 0x33: /*INC SP*/
                                sp++;
                                cycles-=8;
                                break;

                                // t 12
                                case 0x34: /*INC (HL)*/
                                temp=readmem(hl.w)+1;
                                setinc(temp);
                                writemem(hl.w,temp);
                                cycles-=12;
                                break;

                                // t 12
                                case 0x35: /*DEC (HL)*/
                                temp=readmem(hl.w)-1;
                                setdec(temp);
                                writemem(hl.w,temp);
                                cycles-=12;
                                break;

                                // t 8
                                case 0x36: /*LD (HL),xx*/
                                temp=readmem(pc); pc++;
                                writemem(hl.w,temp);
                                cycles-=12;
                                break;

                                // t 4
                                case 0x37: /*SCF*/
                                af.b.l|=C_FLAG;
                                cycles-=4;
                                break;

                                // t 8 8
                                case 0x38: /*JR C*/
                                offset=(signed char)readmem(pc); pc++;
                                if (af.b.l&C_FLAG)
                                {
                                        pc+=offset;
                                        cycles-=12;
                                }
                                else
                                {
                                cycles-=8;
                                }
                                break;

                                // t 12
                                case 0x39: /*ADD HL,SP*/
                                setadd16(hl.w,sp);
                                hl.w+=sp;
                                cycles-=12;
                                break;

                                // t 16
                                case 0x3A: /*LD A,(xxxx)*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                af.b.h=readmem(addr);
                                cycles-=16;
                                break;

                                // t 4
                                case 0x3B: /*DEC SP*/
                                sp--;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x3C: /*INC A*/
                                af.b.h++;
                                setinc(af.b.h);
                                cycles-=4;
                                break;

                                // t 4
                                case 0x3D: /*DEC A*/
                                af.b.h--;
                                setdec(af.b.h);
                                cycles-=4;
                                break;

                                // t 8
                                case 0x3E: /*LD A,xx*/
                                af.b.h=readmem(pc); pc++;
                                cycles-=8;
                                break;

                                // t 4
                                case 0x3F: /*CCF*/
                                if (af.b.l&C_FLAG) af.b.l|=H_FLAG;
                                else               af.b.l&=~H_FLAG;
                                af.b.l^=C_FLAG;
                                cycles-=4;
                                break;

                                /*Register to register loads*/
                                case 0x40: bc.b.h=bc.b.h;        cycles-=4; break; /*LD B,B*/
                                case 0x41: bc.b.h=bc.b.l;        cycles-=4; break; /*LD B,C*/
                                case 0x42: bc.b.h=de.b.h;        cycles-=4; break; /*LD B,D*/
                                case 0x43: bc.b.h=de.b.l;        cycles-=4; break; /*LD B,E*/
                                case 0x44: bc.b.h=hl.b.h;        cycles-=4; break; /*LD B,H*/
                                case 0x45: bc.b.h=hl.b.l;        cycles-=4; break; /*LD B,L*/
                                case 0x46: bc.b.h=readmem(hl.w); cycles-=8; break; /*LD B,(HL)*/ // t 8
                                case 0x47: bc.b.h=af.b.h;        cycles-=4; break; /*LD B,A*/
                                case 0x48: bc.b.l=bc.b.h;        cycles-=4; break; /*LD C,B*/
                                case 0x49: bc.b.l=bc.b.l;        cycles-=4; break; /*LD C,C*/
                                case 0x4A: bc.b.l=de.b.h;        cycles-=4; break; /*LD C,D*/
                                case 0x4B: bc.b.l=de.b.l;        cycles-=4; break; /*LD C,E*/
                                case 0x4C: bc.b.l=hl.b.h;        cycles-=4; break; /*LD C,H*/
                                case 0x4D: bc.b.l=hl.b.l;        cycles-=4; break; /*LD C,L*/
                                case 0x4E: bc.b.l=readmem(hl.w); cycles-=8; break; /*LD C,(HL)*/ // t 8
                                case 0x4F: bc.b.l=af.b.h;        cycles-=4; break; /*LD C,A*/
                                case 0x50: de.b.h=bc.b.h;        cycles-=4; break; /*LD D,B*/
                                case 0x51: de.b.h=bc.b.l;        cycles-=4; break; /*LD D,C*/
                                case 0x52: de.b.h=de.b.h;        cycles-=4; break; /*LD D,D*/
                                case 0x53: de.b.h=de.b.l;        cycles-=4; break; /*LD D,E*/
                                case 0x54: de.b.h=hl.b.h;        cycles-=4; break; /*LD D,H*/
                                case 0x55: de.b.h=hl.b.l;        cycles-=4; break; /*LD D,L*/
                                case 0x56: de.b.h=readmem(hl.w); cycles-=8; break; /*LD D,(HL)*/ // t 8
                                case 0x57: de.b.h=af.b.h;        cycles-=4; break; /*LD D,A*/
                                case 0x58: de.b.l=bc.b.h;        cycles-=4; break; /*LD E,B*/
                                case 0x59: de.b.l=bc.b.l;        cycles-=4; break; /*LD E,C*/
                                case 0x5A: de.b.l=de.b.h;        cycles-=4; break; /*LD E,D*/
                                case 0x5B: de.b.l=de.b.l;        cycles-=4; break; /*LD E,E*/
                                case 0x5C: de.b.l=hl.b.h;        cycles-=4; break; /*LD E,H*/
                                case 0x5D: de.b.l=hl.b.l;        cycles-=4; break; /*LD E,L*/
                                case 0x5E: de.b.l=readmem(hl.w); cycles-=8; break; /*LD E,(HL)*/ // t 8
                                case 0x5F: de.b.l=af.b.h;        cycles-=4; break; /*LD E,A*/
                                case 0x60: hl.b.h=bc.b.h;        cycles-=4; break; /*LD H,B*/
                                case 0x61: hl.b.h=bc.b.l;        cycles-=4; break; /*LD H,C*/
                                case 0x62: hl.b.h=de.b.h;        cycles-=4; break; /*LD H,D*/
                                case 0x63: hl.b.h=de.b.l;        cycles-=4; break; /*LD H,E*/
                                case 0x64: hl.b.h=hl.b.h;        cycles-=4; break; /*LD H,H*/
                                case 0x65: hl.b.h=hl.b.l;        cycles-=4; break; /*LD H,L*/
                                case 0x66: hl.b.h=readmem(hl.w); cycles-=8; break; /*LD H,(HL)*/ // t 8
                                case 0x67: hl.b.h=af.b.h;        cycles-=4; break; /*LD H,A*/
                                case 0x68: hl.b.l=bc.b.h;        cycles-=4; break; /*LD L,B*/
                                case 0x69: hl.b.l=bc.b.l;        cycles-=4; break; /*LD L,C*/
                                case 0x6A: hl.b.l=de.b.h;        cycles-=4; break; /*LD L,D*/
                                case 0x6B: hl.b.l=de.b.l;        cycles-=4; break; /*LD L,E*/
                                case 0x6C: hl.b.l=hl.b.h;        cycles-=4; break; /*LD L,H*/
                                case 0x6D: hl.b.l=hl.b.l;        cycles-=4; break; /*LD L,L*/
                                case 0x6E: hl.b.l=readmem(hl.w); cycles-=8; break; /*LD L,(HL)*/ // t 8
                                case 0x6F: hl.b.l=af.b.h;        cycles-=4; break; /*LD L,A*/
                                case 0x70: writemem(hl.w,bc.b.h); cycles-=8; break; /*LD (HL),B*/ // t 8
                                case 0x71: writemem(hl.w,bc.b.l); cycles-=8; break; /*LD (HL),C*/ // t 8
                                case 0x72: writemem(hl.w,de.b.h); cycles-=8; break; /*LD (HL),D*/ // t 8
                                case 0x73: writemem(hl.w,de.b.l); cycles-=8; break; /*LD (HL),E*/ // t 8
                                case 0x74: writemem(hl.w,hl.b.h); cycles-=8; break; /*LD (HL),H*/ // t 8
                                case 0x75: writemem(hl.w,hl.b.l); cycles-=8; break; /*LD (HL),L*/ // t 8
                                case 0x77: writemem(hl.w,af.b.h); cycles-=8; break; /*LD (HL),A*/ // t 8
                                case 0x78: af.b.h=bc.b.h;        cycles-=4; break; /*LD A,B*/
                                case 0x79: af.b.h=bc.b.l;        cycles-=4; break; /*LD A,C*/
                                case 0x7A: af.b.h=de.b.h;        cycles-=4; break; /*LD A,D*/
                                case 0x7B: af.b.h=de.b.l;        cycles-=4; break; /*LD A,E*/
                                case 0x7C: af.b.h=hl.b.h;        cycles-=4; break; /*LD A,H*/
                                case 0x7D: af.b.h=hl.b.l;        cycles-=4; break; /*LD A,L*/
                                case 0x7E: af.b.h=readmem(hl.w); cycles-=8; break; /*LD A,(HL)*/ // t 8
                                case 0x7F: af.b.h=af.b.h;        cycles-=4; break; /*LD A,A*/

                                case 0x76: /*HALT*/
                                if (!intreq) pc--;
                                cycles-=4;
                                break;

                                /*Add group*/
                                case 0x80: setadd(af.b.h,bc.b.h); af.b.h+=bc.b.h; cycles-=4; break; /*ADD B*/
                                case 0x81: setadd(af.b.h,bc.b.l); af.b.h+=bc.b.l; cycles-=4; break; /*ADD C*/
                                case 0x82: setadd(af.b.h,de.b.h); af.b.h+=de.b.h; cycles-=4; break; /*ADD D*/
                                case 0x83: setadd(af.b.h,de.b.l); af.b.h+=de.b.l; cycles-=4; break; /*ADD E*/
                                case 0x84: setadd(af.b.h,hl.b.h); af.b.h+=hl.b.h; cycles-=4; break; /*ADD H*/
                                case 0x85: setadd(af.b.h,hl.b.l); af.b.h+=hl.b.l; cycles-=4; break; /*ADD L*/
                                case 0x86: temp=readmem(hl.w); setadd(af.b.h,temp); af.b.h+=temp; cycles-=8; break; /*ADD (HL)*/ // t 8
                                case 0x87: setadd(af.b.h,af.b.h); af.b.h+=af.b.h; cycles-=4; break; /*ADD A*/

                                /*ADC group*/
                                case 0x88: setadc(af.b.h,bc.b.h); af.b.h+=bc.b.h+tempc; cycles-=4; break; /*ADC B*/
                                case 0x89: setadc(af.b.h,bc.b.l); af.b.h+=bc.b.l+tempc; cycles-=4; break; /*ADC C*/
                                case 0x8A: setadc(af.b.h,de.b.h); af.b.h+=de.b.h+tempc; cycles-=4; break; /*ADC D*/
                                case 0x8B: setadc(af.b.h,de.b.l); af.b.h+=de.b.l+tempc; cycles-=4; break; /*ADC E*/
                                case 0x8C: setadc(af.b.h,hl.b.h); af.b.h+=hl.b.h+tempc; cycles-=4; break; /*ADC H*/
                                case 0x8D: setadc(af.b.h,hl.b.l); af.b.h+=hl.b.l+tempc; cycles-=4; break; /*ADC L*/
                                case 0x8E: temp=readmem(hl.w); setadc(af.b.h,temp); af.b.h+=temp+tempc; cycles-=8; break; /*ADC (HL)*/ // t 8
                                case 0x8F: setadc(af.b.h,af.b.h); af.b.h+=af.b.h+tempc; cycles-=4; break; /*ADC A*/

                                /*Subtract group*/
                                case 0x90: setsub(af.b.h,bc.b.h); af.b.h-=bc.b.h; cycles-=4; break; /*SUB B*/
                                case 0x91: setsub(af.b.h,bc.b.l); af.b.h-=bc.b.l; cycles-=4; break; /*SUB C*/
                                case 0x92: setsub(af.b.h,de.b.h); af.b.h-=de.b.h; cycles-=4; break; /*SUB D*/
                                case 0x93: setsub(af.b.h,de.b.l); af.b.h-=de.b.l; cycles-=4; break; /*SUB E*/
                                case 0x94: setsub(af.b.h,hl.b.h); af.b.h-=hl.b.h; cycles-=4; break; /*SUB H*/
                                case 0x95: setsub(af.b.h,hl.b.l); af.b.h-=hl.b.l; cycles-=4; break; /*SUB L*/
                                case 0x96: temp=readmem(hl.w); setsub(af.b.h,temp); af.b.h-=temp; cycles-=8; break; /*ADD (HL)*/ // t 8
                                case 0x97: setsub(af.b.h,af.b.h); af.b.h-=af.b.h; cycles-=4; break; /*SUB A*/

                                /*SBC group*/
                                case 0x98: setsbc(af.b.h,bc.b.h); af.b.h-=(bc.b.h+tempc); cycles-=4; break; /*SBC B*/
                                case 0x99: setsbc(af.b.h,bc.b.l); af.b.h-=(bc.b.l+tempc); cycles-=4; break; /*SBC C*/
                                case 0x9A: setsbc(af.b.h,de.b.h); af.b.h-=(de.b.h+tempc); cycles-=4; break; /*SBC D*/
                                case 0x9B: setsbc(af.b.h,de.b.l); af.b.h-=(de.b.l+tempc); cycles-=4; break; /*SBC E*/
                                case 0x9C: setsbc(af.b.h,hl.b.h); af.b.h-=(hl.b.h+tempc); cycles-=4; break; /*SBC H*/
                                case 0x9D: setsbc(af.b.h,hl.b.l); af.b.h-=(hl.b.l+tempc); cycles-=4; break; /*SBC L*/
                                case 0x9E: temp=readmem(hl.w); setsbc(af.b.h,temp); af.b.h-=(temp+tempc); cycles-=8; break; /*SBC (HL)*/ // t 8
                                case 0x9F: setsbc(af.b.h,af.b.h); af.b.h-=(af.b.h+tempc); cycles-=4; break; /*SBC A*/

                                /*AND group*/
                                case 0xA0: af.b.h&=bc.b.h; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND B*/
                                case 0xA1: af.b.h&=bc.b.l; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND C*/
                                case 0xA2: af.b.h&=de.b.h; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND D*/
                                case 0xA3: af.b.h&=de.b.l; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND E*/
                                case 0xA4: af.b.h&=hl.b.h; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND H*/
                                case 0xA5: af.b.h&=hl.b.l; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND L*/
                                case 0xA6: temp=readmem(hl.w); af.b.h&=temp; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=8; break; /*AND (HL)*/ // t 8
                                case 0xA7: af.b.h&=af.b.h; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=4; break; /*AND A*/
                                /*XOR group*/
                                case 0xA8: af.b.h^=bc.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*XOR B*/
                                case 0xA9: af.b.h^=bc.b.l; setzn(af.b.h); af.b.l&=~3;cycles-=4; break; /*XOR C*/
                                case 0xAA: af.b.h^=de.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*XOR D*/
                                case 0xAB: af.b.h^=de.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*XOR E*/
                                case 0xAC: af.b.h^=hl.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*XOR H*/
                                case 0xAD: af.b.h^=hl.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*XOR L*/
                                case 0xAE: temp=readmem(hl.w); af.b.h^=temp; setzn(af.b.h); af.b.l&=~3; cycles-=8; break; /*XOR (HL)*/ // t 8
                                case 0xAF: af.b.h^=af.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*XOR A*/
                                /*OR group*/
                                case 0xB0: af.b.h|=bc.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR B*/
                                case 0xB1: af.b.h|=bc.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR C*/
                                case 0xB2: af.b.h|=de.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR D*/
                                case 0xB3: af.b.h|=de.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR E*/
                                case 0xB4: af.b.h|=hl.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR H*/
                                case 0xB5: af.b.h|=hl.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR L*/
                                case 0xB6: temp=readmem(hl.w); af.b.h|=temp; setzn(af.b.h); af.b.l&=~3; cycles-=8; break; /*OR (HL)*/ // t 8
                                case 0xB7: af.b.h|=af.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=4; break; /*OR A*/
                                /*CP group*/
                                case 0xB8: setcp(af.b.h,bc.b.h); cycles-=4; break; /*CP B*/
                                case 0xB9: setcp(af.b.h,bc.b.l); cycles-=4; break; /*CP C*/
                                case 0xBA: setcp(af.b.h,de.b.h); cycles-=4; break; /*CP D*/
                                case 0xBB: setcp(af.b.h,de.b.l); cycles-=4; break; /*CP E*/
                                case 0xBC: setcp(af.b.h,hl.b.h); cycles-=4; break; /*CP H*/
                                case 0xBD: setcp(af.b.h,hl.b.l); cycles-=4; break; /*CP L*/
                                case 0xBE: temp=readmem(hl.w); setcp(af.b.h,temp); /*if (output) { printf("CP %02X %02X\n",af.b.h,temp,ram[hl.w]); dumpregs(); dumpram(); exit(0); } */cycles-=8; break; /*CP (HL)*/ // t 8
                                case 0xBF: setcp(af.b.h,af.b.h); cycles-=4; break; /*CP A*/
//D96A is it

                                // t 12
                                case 0xC0: /*RET NZ*/
                                if (!(af.b.l&Z_FLAG))
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=16;
                                }
                                else
                                {
                                cycles-=8;
                                }
                                break;

                                // t 12
                                case 0xC1: /*POP BC*/
                                bc.b.l=pull();
                                bc.b.h=pull();
                                cycles-=12;
                                break;

                                // t 12
                                case 0xC2: /*JP NZ,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&Z_FLAG))
                                   pc=addr;
                                cycles-=12;
                                break;

                                // t 12
                                case 0xC3: /*JP xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                pc=addr;
                                cycles-=12;
                                break;

                                // t 8 12
                                case 0xC4: /*CALL NZ*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&Z_FLAG))
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20;
                                }
                                else
                                {
                                cycles-=12;
                                }
                                break;

                                // t 12
                                case 0xC5: /*PUSH BC*/
                                push(bc.b.h);
                                push(bc.b.l);
                                cycles-=16;
                                break;

                                // t 8
                                case 0xC6: /*ADD xx*/
                                temp=readmem(pc); pc++;
                                setadd(af.b.h,temp);
                                af.b.h+=temp;
                                cycles-=8;
                                break;

                                // t 16
                                case 0xC7: /*RST 0*/
                                push(pc>>8);
                                push(pc);
                                pc=0x00;
                                cycles-=16;
                                break;

                                // t 12
                                case 0xC8: /*RET Z*/
                                if (af.b.l&Z_FLAG)
                                {
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16;
                                }
                                else
                                {
                                 cycles-=8;
                                }
                                break;

                                // t 12
                                case 0xC9: /*RET*/
//                                if (pc==0xBAA7) output=1;
                                pc=pull();
                                pc|=pull()<<8;
//                                if (output) printf("BAA7 %04X\n",pc);
//                                output=0;
                                cycles-=12;
                                break;

                                // t 12
                                case 0xCA: /*JP Z,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&Z_FLAG)
                                   pc=addr;
                                cycles-=12;
                                break;
                                case 0xCB: /*More opcodes*/
                                opcode=readmem(pc); pc++;
                                ir.b.l++;
                                switch (opcode)
                                {
                                        case 0x00: if (bc.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h<<=1; if (af.b.l&C_FLAG) bc.b.h|=1; setznc(bc.b.h); cycles-=8; break; /*RLC B*/
                                        case 0x01: if (bc.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l<<=1; if (af.b.l&C_FLAG) bc.b.l|=1; setznc(bc.b.l); cycles-=8; break; /*RLC C*/
                                        case 0x02: if (de.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h<<=1; if (af.b.l&C_FLAG) de.b.h|=1; setznc(de.b.h); cycles-=8; break; /*RLC D*/
                                        case 0x03: if (de.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l<<=1; if (af.b.l&C_FLAG) de.b.l|=1; setznc(de.b.l); cycles-=8; break; /*RLC E*/
                                        case 0x04: if (hl.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h<<=1; if (af.b.l&C_FLAG) hl.b.h|=1; setznc(hl.b.h); cycles-=8; break; /*RLC H*/
                                        case 0x05: if (hl.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l<<=1; if (af.b.l&C_FLAG) hl.b.l|=1; setznc(hl.b.l); cycles-=8; break; /*RLC L*/
                                        case 0x06: temp=readmem(hl.w); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; if (af.b.l&C_FLAG) temp|=1; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*RLC L*/ // t 16
                                        case 0x07: if (af.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h<<=1; if (af.b.l&C_FLAG) af.b.h|=1; setznc(af.b.h); cycles-=8; break; /*RLC A*/

                                        case 0x08: if (bc.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h>>=1; if (af.b.l&C_FLAG) bc.b.h|=0x80; setznc(bc.b.h); cycles-=8; break; /*RRC B*/
                                        case 0x09: if (bc.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l>>=1; if (af.b.l&C_FLAG) bc.b.l|=0x80; setznc(bc.b.l); cycles-=8; break; /*RRC C*/
                                        case 0x0A: if (de.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h>>=1; if (af.b.l&C_FLAG) de.b.h|=0x80; setznc(de.b.h); cycles-=8; break; /*RRC D*/
                                        case 0x0B: if (de.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l>>=1; if (af.b.l&C_FLAG) de.b.l|=0x80; setznc(de.b.l); cycles-=8; break; /*RRC E*/
                                        case 0x0C: if (hl.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h>>=1; if (af.b.l&C_FLAG) hl.b.h|=0x80; setznc(hl.b.h); cycles-=8; break; /*RRC H*/
                                        case 0x0D: if (hl.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l>>=1; if (af.b.l&C_FLAG) hl.b.l|=0x80; setznc(hl.b.l); cycles-=8; break; /*RRC L*/
                                        case 0x0E: temp=readmem(hl.w); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (af.b.l&C_FLAG) temp|=0x80; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*RRC (HL)*/// t 16
                                        case 0x0F: if (af.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h>>=1; if (af.b.l&C_FLAG) af.b.h|=0x80; setznc(af.b.h); cycles-=8; break; /*RRC A*/

                                        case 0x10: if (bc.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h<<=1; if (tempc) bc.b.h|=1; setznc(bc.b.h); cycles-=8; break; /*RL B*/
                                        case 0x11: if (bc.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l<<=1; if (tempc) bc.b.l|=1; setznc(bc.b.l); cycles-=8; break; /*RL C*/
                                        case 0x12: if (de.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h<<=1; if (tempc) de.b.h|=1; setznc(de.b.h); cycles-=8; break; /*RL D*/
                                        case 0x13: if (de.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l<<=1; if (tempc) de.b.l|=1; setznc(de.b.l); cycles-=8; break; /*RL E*/
                                        case 0x14: if (hl.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h<<=1; if (tempc) hl.b.h|=1; setznc(hl.b.h); cycles-=8; break; /*RL H*/
                                        case 0x15: if (hl.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l<<=1; if (tempc) hl.b.l|=1; setznc(hl.b.l); cycles-=8; break; /*RL L*/
                                        case 0x16: temp=readmem(hl.w); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; if (tempc) temp|=1; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*RL (HL)*/ // t 16
                                        case 0x17: if (af.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h<<=1; if (tempc) af.b.h|=1; setznc(af.b.h); cycles-=8; break; /*RL A*/

                                        case 0x18: if (bc.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h>>=1; if (tempc) bc.b.h|=0x80; setznc(bc.b.h); cycles-=8; break; /*RR B*/
                                        case 0x19: if (bc.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l>>=1; if (tempc) bc.b.l|=0x80; setznc(bc.b.l); cycles-=8; break; /*RR C*/
                                        case 0x1A: if (de.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h>>=1; if (tempc) de.b.h|=0x80; setznc(de.b.h); cycles-=8; break; /*RR D*/
                                        case 0x1B: if (de.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l>>=1; if (tempc) de.b.l|=0x80; setznc(de.b.l); cycles-=8; break; /*RR E*/
                                        case 0x1C: if (hl.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h>>=1; if (tempc) hl.b.h|=0x80; setznc(hl.b.h); cycles-=8; break; /*RR H*/
                                        case 0x1D: if (hl.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l>>=1; if (tempc) hl.b.l|=0x80; setznc(hl.b.l); cycles-=8; break; /*RR L*/
                                        case 0x1E: temp=readmem(hl.w); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (tempc) temp|=0x80; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*RR (HL)*/ // t 16
                                        case 0x1F: if (af.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h>>=1; if (tempc) af.b.h|=0x80; setznc(af.b.h); cycles-=8; break; /*RR A*/

                                        case 0x20: if (bc.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h<<=1; setznc(bc.b.h); cycles-=8; break; /*SLA B*/
                                        case 0x21: if (bc.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l<<=1; setznc(bc.b.l); cycles-=8; break; /*SLA C*/
                                        case 0x22: if (de.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h<<=1; setznc(de.b.h); cycles-=8; break; /*SLA D*/
                                        case 0x23: if (de.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l<<=1; setznc(de.b.l); cycles-=8; break; /*SLA E*/
                                        case 0x24: if (hl.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h<<=1; setznc(hl.b.h); cycles-=8; break; /*SLA H*/
                                        case 0x25: if (hl.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l<<=1; setznc(hl.b.l); cycles-=8; break; /*SLA L*/
                                        case 0x26: temp=readmem(hl.w); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*SLA (HL)*/ // t 16
                                        case 0x27: if (af.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h<<=1; setznc(af.b.h); cycles-=8; break; /*SLA A*/

                                        case 0x28: if (bc.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h>>=1; if (bc.b.h&0x40) bc.b.h|=0x80; setznc(bc.b.h); cycles-=8; break; /*SRA B*/
                                        case 0x29: if (bc.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l>>=1; if (bc.b.l&0x40) bc.b.l|=0x80; setznc(bc.b.l); cycles-=8; break; /*SRA C*/
                                        case 0x2A: if (de.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h>>=1; if (de.b.h&0x40) de.b.h|=0x80; setznc(de.b.h); cycles-=8; break; /*SRA D*/
                                        case 0x2B: if (de.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l>>=1; if (de.b.l&0x40) de.b.l|=0x80; setznc(de.b.l); cycles-=8; break; /*SRA E*/
                                        case 0x2C: if (hl.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h>>=1; if (hl.b.h&0x40) hl.b.h|=0x80; setznc(hl.b.h); cycles-=8; break; /*SRA H*/
                                        case 0x2D: if (hl.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l>>=1; if (hl.b.l&0x40) hl.b.l|=0x80; setznc(hl.b.l); cycles-=8; break; /*SRA L*/
                                        case 0x2E: temp=readmem(hl.w); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (temp&0x40) temp|=0x80; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*SRA (HL)*/  //t 16
                                        case 0x2F: if (af.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h>>=1; if (af.b.h&0x40) af.b.h|=0x80; setznc(af.b.h); cycles-=8; break; /*SRA A*/

                                        case 0x30: if (bc.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h<<=1; bc.b.h|=1; setznc(bc.b.h); cycles-=8; break; /*SLL B*/
                                        case 0x31: if (bc.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l<<=1; bc.b.l|=1; setznc(bc.b.l); cycles-=8; break; /*SLL C*/
                                        case 0x32: if (de.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h<<=1; de.b.h|=1; setznc(de.b.h); cycles-=8; break; /*SLL D*/
                                        case 0x33: if (de.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l<<=1; de.b.l|=1; setznc(de.b.l); cycles-=8; break; /*SLL E*/
                                        case 0x34: if (hl.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h<<=1; hl.b.h|=1; setznc(hl.b.h); cycles-=8; break; /*SLL H*/
                                        case 0x35: if (hl.b.l&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l<<=1; hl.b.l|=1; setznc(hl.b.l); cycles-=8; break; /*SLL L*/
                                        case 0x36: temp=readmem(hl.w); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; temp|=1; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*SLL (HL)*/ // t 16
                                        case 0x37: if (af.b.h&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h<<=1; af.b.h|=1; setznc(af.b.h); cycles-=8; break; /*SLL A*/

                                        case 0x38: if (bc.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.h>>=1; setznc(bc.b.h); cycles-=8; break; /*SRL B*/
                                        case 0x39: if (bc.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; bc.b.l>>=1; setznc(bc.b.l); cycles-=8; break; /*SRL C*/
                                        case 0x3A: if (de.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.h>>=1; setznc(de.b.h); cycles-=8; break; /*SRL D*/
                                        case 0x3B: if (de.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; de.b.l>>=1; setznc(de.b.l); cycles-=8; break; /*SRL E*/
                                        case 0x3C: if (hl.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.h>>=1; setznc(hl.b.h); cycles-=8; break; /*SRL H*/
                                        case 0x3D: if (hl.b.l&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; hl.b.l>>=1; setznc(hl.b.l); cycles-=8; break; /*SRL L*/
                                        case 0x3E: temp=readmem(hl.w); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; setznc(temp); writemem(hl.w,temp); cycles-=16; break; /*SRL (HL)*/  //t 16
                                        case 0x3F: if (af.b.h&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; af.b.h>>=1; setznc(af.b.h); cycles-=8; break; /*SRL A*/

                                        case 0x40: setznc(bc.b.h&0x01); cycles-=8; break; /*BIT 0,B*/
                                        case 0x41: setznc(bc.b.l&0x01); cycles-=8; break; /*BIT 0,C*/
                                        case 0x42: setznc(de.b.h&0x01); cycles-=8; break; /*BIT 0,D*/
                                        case 0x43: setznc(de.b.l&0x01); cycles-=8; break; /*BIT 0,E*/
                                        case 0x44: setznc(hl.b.h&0x01); cycles-=8; break; /*BIT 0,H*/
                                        case 0x45: setznc(hl.b.l&0x01); cycles-=8; break; /*BIT 0,L*/
                                        case 0x46: temp=readmem(hl.w); setznc(temp&0x01); cycles-=12; break; /*BIT 0,(HL)*/
                                        case 0x47: setznc(af.b.h&0x01); cycles-=8; break; /*BIT 0,A*/
                                        case 0x48: setznc(bc.b.h&0x02); cycles-=8; break; /*BIT 1,B*/
                                        case 0x49: setznc(bc.b.l&0x02); cycles-=8; break; /*BIT 1,C*/
                                        case 0x4A: setznc(de.b.h&0x02); cycles-=8; break; /*BIT 1,D*/
                                        case 0x4B: setznc(de.b.l&0x02); cycles-=8; break; /*BIT 1,E*/
                                        case 0x4C: setznc(hl.b.h&0x02); cycles-=8; break; /*BIT 1,H*/
                                        case 0x4D: setznc(hl.b.l&0x02); cycles-=8; break; /*BIT 1,L*/
                                        case 0x4E: temp=readmem(hl.w); setznc(temp&0x02); cycles-=12; break; /*BIT 1,(HL)*/
                                        case 0x4F: setznc(af.b.h&0x02); cycles-=8; break; /*BIT 1,A*/
                                        case 0x50: setznc(bc.b.h&0x04); cycles-=8; break; /*BIT 2,B*/
                                        case 0x51: setznc(bc.b.l&0x04); cycles-=8; break; /*BIT 2,C*/
                                        case 0x52: setznc(de.b.h&0x04); cycles-=8; break; /*BIT 2,D*/
                                        case 0x53: setznc(de.b.l&0x04); cycles-=8; break; /*BIT 2,E*/
                                        case 0x54: setznc(hl.b.h&0x04); cycles-=8; break; /*BIT 2,H*/
                                        case 0x55: setznc(hl.b.l&0x04); cycles-=8; break; /*BIT 2,L*/
                                        case 0x56: temp=readmem(hl.w); setznc(temp&0x04); cycles-=12; break; /*BIT 2,(HL)*/
                                        case 0x57: setznc(af.b.h&0x04); cycles-=8; break; /*BIT 2,A*/
                                        case 0x58: setznc(bc.b.h&0x08); cycles-=8; break; /*BIT 3,B*/
                                        case 0x59: setznc(bc.b.l&0x08); cycles-=8; break; /*BIT 3,C*/
                                        case 0x5A: setznc(de.b.h&0x08); cycles-=8; break; /*BIT 3,D*/
                                        case 0x5B: setznc(de.b.l&0x08); cycles-=8; break; /*BIT 3,E*/
                                        case 0x5C: setznc(hl.b.h&0x08); cycles-=8; break; /*BIT 3,H*/
                                        case 0x5D: setznc(hl.b.l&0x08); cycles-=8; break; /*BIT 3,L*/
                                        case 0x5E: temp=readmem(hl.w); setznc(temp&0x08); cycles-=12; break; /*BIT 3,(HL)*/
                                        case 0x5F: setznc(af.b.h&0x08); cycles-=8; break; /*BIT 3,A*/

                                        case 0x60: setznc(bc.b.h&0x10); cycles-=8; break; /*BIT 4,B*/
                                        case 0x61: setznc(bc.b.l&0x10); cycles-=8; break; /*BIT 4,C*/
                                        case 0x62: setznc(de.b.h&0x10); cycles-=8; break; /*BIT 4,D*/
                                        case 0x63: setznc(de.b.l&0x10); cycles-=8; break; /*BIT 4,E*/
                                        case 0x64: setznc(hl.b.h&0x10); cycles-=8; break; /*BIT 4,H*/
                                        case 0x65: setznc(hl.b.l&0x10); cycles-=8; break; /*BIT 4,L*/
                                        case 0x66: temp=readmem(hl.w); setznc(temp&0x10); cycles-=12; break; /*BIT 4,(HL)*/
                                        case 0x67: setznc(af.b.h&0x10); cycles-=8; break; /*BIT 4,A*/
                                        case 0x68: setznc(bc.b.h&0x20); cycles-=8; break; /*BIT 5,B*/
                                        case 0x69: setznc(bc.b.l&0x20); cycles-=8; break; /*BIT 5,C*/
                                        case 0x6A: setznc(de.b.h&0x20); cycles-=8; break; /*BIT 5,D*/
                                        case 0x6B: setznc(de.b.l&0x20); cycles-=8; break; /*BIT 5,E*/
                                        case 0x6C: setznc(hl.b.h&0x20); cycles-=8; break; /*BIT 5,H*/
                                        case 0x6D: setznc(hl.b.l&0x20); cycles-=8; break; /*BIT 5,L*/
                                        case 0x6E: temp=readmem(hl.w); setznc(temp&0x20); cycles-=12; break; /*BIT 5,(HL)*/
                                        case 0x6F: setznc(af.b.h&0x20); cycles-=8; break; /*BIT 5,A*/
                                        case 0x70: setznc(bc.b.h&0x40); cycles-=8; break; /*BIT 6,B*/
                                        case 0x71: setznc(bc.b.l&0x40); cycles-=8; break; /*BIT 6,C*/
                                        case 0x72: setznc(de.b.h&0x40); cycles-=8; break; /*BIT 6,D*/
                                        case 0x73: setznc(de.b.l&0x40); cycles-=8; break; /*BIT 6,E*/
                                        case 0x74: setznc(hl.b.h&0x40); cycles-=8; break; /*BIT 6,H*/
                                        case 0x75: setznc(hl.b.l&0x40); cycles-=8; break; /*BIT 6,L*/
                                        case 0x76: temp=readmem(hl.w); setznc(temp&0x40); cycles-=12; break; /*BIT 6,(HL)*/
                                        case 0x77: setznc(af.b.h&0x40); cycles-=8; break; /*BIT 6,A*/
                                        case 0x78: setznc(bc.b.h&0x80); cycles-=8; break; /*BIT 7,B*/
                                        case 0x79: setznc(bc.b.l&0x80); cycles-=8; break; /*BIT 7,C*/
                                        case 0x7A: setznc(de.b.h&0x80); cycles-=8; break; /*BIT 7,D*/
                                        case 0x7B: setznc(de.b.l&0x80); cycles-=8; break; /*BIT 7,E*/
                                        case 0x7C: setznc(hl.b.h&0x80); cycles-=8; break; /*BIT 7,H*/
                                        case 0x7D: setznc(hl.b.l&0x80); cycles-=8; break; /*BIT 7,L*/
                                        case 0x7E: temp=readmem(hl.w); setznc(temp&0x80); cycles-=12; break; /*BIT 7,(HL)*/
                                        case 0x7F: setznc(af.b.h&0x80); cycles-=8; break; /*BIT 7,A*/

                                        case 0x80: bc.b.h&=~1; cycles-=8; break; /*RES 0,B*/
                                        case 0x81: bc.b.l&=~1; cycles-=8; break; /*RES 0,C*/
                                        case 0x82: de.b.h&=~1; cycles-=8; break; /*RES 0,D*/
                                        case 0x83: de.b.l&=~1; cycles-=8; break; /*RES 0,E*/
                                        case 0x84: hl.b.h&=~1; cycles-=8; break; /*RES 0,H*/
                                        case 0x85: hl.b.l&=~1; cycles-=8; break; /*RES 0,L*/
                                        case 0x86: temp=readmem(hl.w)&~0x01; writemem(hl.w,temp); cycles-=16; break; /*RES 0,(HL)*/ // t 16
                                        case 0x87: af.b.h&=~1; cycles-=8; break; /*RES 0,A*/
                                        case 0x88: bc.b.h&=~2; cycles-=8; break; /*RES 1,B*/
                                        case 0x89: bc.b.l&=~2; cycles-=8; break; /*RES 1,C*/
                                        case 0x8A: de.b.h&=~2; cycles-=8; break; /*RES 1,D*/
                                        case 0x8B: de.b.l&=~2; cycles-=8; break; /*RES 1,E*/
                                        case 0x8C: hl.b.h&=~2; cycles-=8; break; /*RES 1,H*/
                                        case 0x8D: hl.b.l&=~2; cycles-=8; break; /*RES 1,L*/
                                        case 0x8E: temp=readmem(hl.w)&~0x02; writemem(hl.w,temp); cycles-=16; break; /*RES 1,(HL)*/ // t 16
                                        case 0x8F: af.b.h&=~2; cycles-=8; break; /*RES 1,A*/
                                        case 0x90: bc.b.h&=~4; cycles-=8; break; /*RES 2,B*/
                                        case 0x91: bc.b.l&=~4; cycles-=8; break; /*RES 2,C*/
                                        case 0x92: de.b.h&=~4; cycles-=8; break; /*RES 2,D*/
                                        case 0x93: de.b.l&=~4; cycles-=8; break; /*RES 2,E*/
                                        case 0x94: hl.b.h&=~4; cycles-=8; break; /*RES 2,H*/
                                        case 0x95: hl.b.l&=~4; cycles-=8; break; /*RES 2,L*/
                                        case 0x96: temp=readmem(hl.w)&~0x04; writemem(hl.w,temp); cycles-=16; break; /*RES 2,(HL)*/ // t 16
                                        case 0x97: af.b.h&=~4; cycles-=8; break; /*RES 2,A*/
                                        case 0x98: bc.b.h&=~8; cycles-=8; break; /*RES 3,B*/
                                        case 0x99: bc.b.l&=~8; cycles-=8; break; /*RES 3,C*/
                                        case 0x9A: de.b.h&=~8; cycles-=8; break; /*RES 3,D*/
                                        case 0x9B: de.b.l&=~8; cycles-=8; break; /*RES 3,E*/
                                        case 0x9C: hl.b.h&=~8; cycles-=8; break; /*RES 3,H*/
                                        case 0x9D: hl.b.l&=~8; cycles-=8; break; /*RES 3,L*/
                                        case 0x9E: temp=readmem(hl.w)&~0x08; writemem(hl.w,temp); cycles-=16; break; /*RES 3,(HL)*/ // t 16
                                        case 0x9F: af.b.h&=~8; cycles-=8; break; /*RES 3,A*/
                                        case 0xA0: bc.b.h&=~0x10; cycles-=8; break; /*RES 4,B*/
                                        case 0xA1: bc.b.l&=~0x10; cycles-=8; break; /*RES 4,C*/
                                        case 0xA2: de.b.h&=~0x10; cycles-=8; break; /*RES 4,D*/
                                        case 0xA3: de.b.l&=~0x10; cycles-=8; break; /*RES 4,E*/
                                        case 0xA4: hl.b.h&=~0x10; cycles-=8; break; /*RES 4,H*/
                                        case 0xA5: hl.b.l&=~0x10; cycles-=8; break; /*RES 4,L*/
                                        case 0xA6: temp=readmem(hl.w)&~0x10; writemem(hl.w,temp); cycles-=16; break; /*RES 4,(HL)*/ // t 16
                                        case 0xA7: af.b.h&=~0x10; cycles-=8; break; /*RES 4,A*/
                                        case 0xA8: bc.b.h&=~0x20; cycles-=8; break; /*RES 5,B*/
                                        case 0xA9: bc.b.l&=~0x20; cycles-=8; break; /*RES 5,C*/
                                        case 0xAA: de.b.h&=~0x20; cycles-=8; break; /*RES 5,D*/
                                        case 0xAB: de.b.l&=~0x20; cycles-=8; break; /*RES 5,E*/
                                        case 0xAC: hl.b.h&=~0x20; cycles-=8; break; /*RES 5,H*/
                                        case 0xAD: hl.b.l&=~0x20; cycles-=8; break; /*RES 5,L*/
                                        case 0xAE: temp=readmem(hl.w)&~0x20; writemem(hl.w,temp); cycles-=16; break; /*RES 5,(HL)*/ // t 16
                                        case 0xAF: af.b.h&=~0x20; cycles-=8; break; /*RES 5,A*/
                                        case 0xB0: bc.b.h&=~0x40; cycles-=8; break; /*RES 6,B*/
                                        case 0xB1: bc.b.l&=~0x40; cycles-=8; break; /*RES 6,C*/
                                        case 0xB2: de.b.h&=~0x40; cycles-=8; break; /*RES 6,D*/
                                        case 0xB3: de.b.l&=~0x40; cycles-=8; break; /*RES 6,E*/
                                        case 0xB4: hl.b.h&=~0x40; cycles-=8; break; /*RES 6,H*/
                                        case 0xB5: hl.b.l&=~0x40; cycles-=8; break; /*RES 6,L*/
                                        case 0xB6: temp=readmem(hl.w)&~0x40; writemem(hl.w,temp); cycles-=16; break; /*RES 6,(HL)*/ // t 16
                                        case 0xB7: af.b.h&=~0x40; cycles-=8; break; /*RES 6,A*/
                                        case 0xB8: bc.b.h&=~0x80; cycles-=8; break; /*RES 7,B*/
                                        case 0xB9: bc.b.l&=~0x80; cycles-=8; break; /*RES 7,C*/
                                        case 0xBA: de.b.h&=~0x80; cycles-=8; break; /*RES 7,D*/
                                        case 0xBB: de.b.l&=~0x80; cycles-=8; break; /*RES 7,E*/
                                        case 0xBC: hl.b.h&=~0x80; cycles-=8; break; /*RES 7,H*/
                                        case 0xBD: hl.b.l&=~0x80; cycles-=8; break; /*RES 7,L*/
                                        case 0xBE: temp=readmem(hl.w)&~0x80; writemem(hl.w,temp); cycles-=16; break; /*RES 7,(HL)*/ // t 16
                                        case 0xBF: af.b.h&=~0x80; cycles-=8; break; /*RES 7,A*/

                                        case 0xC0: bc.b.h|=0x01; cycles-=8; break; /*SET 0,B*/
                                        case 0xC1: bc.b.l|=0x01; cycles-=8; break; /*SET 0,C*/
                                        case 0xC2: de.b.h|=0x01; cycles-=8; break; /*SET 0,D*/
                                        case 0xC3: de.b.l|=0x01; cycles-=8; break; /*SET 0,E*/
                                        case 0xC4: hl.b.h|=0x01; cycles-=8; break; /*SET 0,H*/
                                        case 0xC5: hl.b.l|=0x01; cycles-=8; break; /*SET 0,L*/
                                        case 0xC6: temp=readmem(hl.w)|0x01; writemem(hl.w,temp); cycles-=16; break; /*SET 0,(HL)*/ // t 16
                                        case 0xC7: af.b.h|=0x01; cycles-=8; break; /*SET 0,A*/
                                        case 0xC8: bc.b.h|=0x02; cycles-=8; break; /*SET 1,B*/
                                        case 0xC9: bc.b.l|=0x02; cycles-=8; break; /*SET 1,C*/
                                        case 0xCA: de.b.h|=0x02; cycles-=8; break; /*SET 1,D*/
                                        case 0xCB: de.b.l|=0x02; cycles-=8; break; /*SET 1,E*/
                                        case 0xCC: hl.b.h|=0x02; cycles-=8; break; /*SET 1,H*/
                                        case 0xCD: hl.b.l|=0x02; cycles-=8; break; /*SET 1,L*/
                                        case 0xCE: temp=readmem(hl.w)|0x02; writemem(hl.w,temp); cycles-=16; break; /*SET 1,(HL)*/ // t 16
                                        case 0xCF: af.b.h|=0x02; cycles-=8; break; /*SET 1,A*/
                                        case 0xD0: bc.b.h|=0x04; cycles-=8; break; /*SET 2,B*/
                                        case 0xD1: bc.b.l|=0x04; cycles-=8; break; /*SET 2,C*/
                                        case 0xD2: de.b.h|=0x04; cycles-=8; break; /*SET 2,D*/
                                        case 0xD3: de.b.l|=0x04; cycles-=8; break; /*SET 2,E*/
                                        case 0xD4: hl.b.h|=0x04; cycles-=8; break; /*SET 2,H*/
                                        case 0xD5: hl.b.l|=0x04; cycles-=8; break; /*SET 2,L*/
                                        case 0xD6: temp=readmem(hl.w)|0x04; writemem(hl.w,temp); cycles-=16; break; /*SET 2,(HL)*/ // t 16
                                        case 0xD7: af.b.h|=0x04; cycles-=8; break; /*SET 2,A*/
                                        case 0xD8: bc.b.h|=0x08; cycles-=8; break; /*SET 3,B*/
                                        case 0xD9: bc.b.l|=0x08; cycles-=8; break; /*SET 3,C*/
                                        case 0xDA: de.b.h|=0x08; cycles-=8; break; /*SET 3,D*/
                                        case 0xDB: de.b.l|=0x08; cycles-=8; break; /*SET 3,E*/
                                        case 0xDC: hl.b.h|=0x08; cycles-=8; break; /*SET 3,H*/
                                        case 0xDD: hl.b.l|=0x08; cycles-=8; break; /*SET 3,L*/
                                        case 0xDE: temp=readmem(hl.w)|0x08; writemem(hl.w,temp); cycles-=16; break; /*SET 3,(HL)*/ // t 16
                                        case 0xDF: af.b.h|=0x08; cycles-=8; break; /*SET 3,A*/

                                        case 0xE0: bc.b.h|=0x10; cycles-=8; break; /*SET 4,B*/
                                        case 0xE1: bc.b.l|=0x10; cycles-=8; break; /*SET 4,C*/
                                        case 0xE2: de.b.h|=0x10; cycles-=8; break; /*SET 4,D*/
                                        case 0xE3: de.b.l|=0x10; cycles-=8; break; /*SET 4,E*/
                                        case 0xE4: hl.b.h|=0x10; cycles-=8; break; /*SET 4,H*/
                                        case 0xE5: hl.b.l|=0x10; cycles-=8; break; /*SET 4,L*/
                                        case 0xE6: temp=readmem(hl.w)|0x10; writemem(hl.w,temp); cycles-=16; break; /*SET 4,(HL)*/ // t 16
                                        case 0xE7: af.b.h|=0x10; cycles-=8; break; /*SET 4,A*/
                                        case 0xE8: bc.b.h|=0x20; cycles-=8; break; /*SET 5,B*/
                                        case 0xE9: bc.b.l|=0x20; cycles-=8; break; /*SET 5,C*/
                                        case 0xEA: de.b.h|=0x20; cycles-=8; break; /*SET 5,D*/
                                        case 0xEB: de.b.l|=0x20; cycles-=8; break; /*SET 5,E*/
                                        case 0xEC: hl.b.h|=0x20; cycles-=8; break; /*SET 5,H*/
                                        case 0xED: hl.b.l|=0x20; cycles-=8; break; /*SET 5,L*/
                                        case 0xEE: temp=readmem(hl.w)|0x20; writemem(hl.w,temp); cycles-=16; break; /*SET 5,(HL)*/ // t 16
                                        case 0xEF: af.b.h|=0x20; cycles-=8; break; /*SET 5,A*/
                                        case 0xF0: bc.b.h|=0x40; cycles-=8; break; /*SET 6,B*/
                                        case 0xF1: bc.b.l|=0x40; cycles-=8; break; /*SET 6,C*/
                                        case 0xF2: de.b.h|=0x40; cycles-=8; break; /*SET 6,D*/
                                        case 0xF3: de.b.l|=0x40; cycles-=8; break; /*SET 6,E*/
                                        case 0xF4: hl.b.h|=0x40; cycles-=8; break; /*SET 6,H*/
                                        case 0xF5: hl.b.l|=0x40; cycles-=8; break; /*SET 6,L*/
                                        case 0xF6: temp=readmem(hl.w)|0x40; writemem(hl.w,temp); cycles-=16; break; /*SET 6,(HL)*/ // t 16
                                        case 0xF7: af.b.h|=0x40; cycles-=8; break; /*SET 6,A*/
                                        case 0xF8: bc.b.h|=0x80; cycles-=8; break; /*SET 7,B*/
                                        case 0xF9: bc.b.l|=0x80; cycles-=8; break; /*SET 7,C*/
                                        case 0xFA: de.b.h|=0x80; cycles-=8; break; /*SET 7,D*/
                                        case 0xFB: de.b.l|=0x80; cycles-=8; break; /*SET 7,E*/
                                        case 0xFC: hl.b.h|=0x80; cycles-=8; break; /*SET 7,H*/
                                        case 0xFD: hl.b.l|=0x80; cycles-=8; break; /*SET 7,L*/
                                        case 0xFE: temp=readmem(hl.w)|0x80; writemem(hl.w,temp); cycles-=16; break; /*SET 7,(HL)*/ // t 16
                                        case 0xFF: af.b.h|=0x80; cycles-=8; break; /*SET 7,A*/
                                        default:
                                        //printf("Bad CB opcode %02X at %04X\n",opcode,--pc);
                                        dumpregs();
                                        exit(-1);
                                }
                                break;

                                // t 5 8?
                                case 0xCC: /*CALL Z*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&Z_FLAG)
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20;
                                }
                                else
                                {
                                cycles-=12;
                                }
                                break;

                                // t 20
                                case 0xCD: /*CALL*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                push(pc>>8);
                                push(pc&0xFF);
                                pc=addr;
                                cycles-=20;
                                break;

                                // t 8
                                case 0xCE: /*ADC xx*/
                                temp=readmem(pc); pc++;
                                setadc(af.b.h,temp);
                                af.b.h+=(temp+tempc);
                                cycles-=8;
                                break;

                                // t 16
                                case 0xCF: /*RST 8*/
                                push(pc>>8);
                                push(pc);
                                pc=0x08;
                                cycles-=16;
                                break;

                                // t 12 -- conflict (only 5) - using Aspectrum T
                                case 0xD0: /*RET NC*/
                                if (!(af.b.l&C_FLAG))
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=16;
                                }
                                else
                                {
                                 cycles-=8;
                                }
                                break;

                                // t 12
                                case 0xD1: /*POP DE*/
                                de.b.l=pull();
                                de.b.h=pull();
                                cycles-=12;
                                break;

                                // t 12
                                case 0xD2: /*JP NC,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&C_FLAG))
                                   pc=addr;
                                cycles-=12;
                                break;

                                // t 12
                                case 0xD3: /*OUT (xx),A*/
                                temp=readmem(pc); pc++;
                                z80out(temp|(af.b.h<<8),af.b.h);
                                cycles-=12;
                                break;

                                // t 8 12
                                case 0xD4: /*CALL NC*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&C_FLAG))
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20;
                                }
                                else
                                {
                                cycles-=12;
                                }
                                break;

                                // t 12
                                case 0xD5: /*PUSH DE*/
                                push(de.b.h);
                                push(de.b.l);
                                cycles-=16;
                                break;

                                // t 8
                                case 0xD6: /*SUB xx*/
                                temp=readmem(pc); pc++;
                                setsub(af.b.h,temp);
                                af.b.h-=temp;
                                cycles-=8;
                                break;

                                // t 16
                                case 0xD7: /*RST 10*/
                                push(pc>>8);
                                push(pc);
                                pc=0x10;
                                cycles-=16;
                                break;

                                // t 12 -- CONFLICT (only 5) using ASPECTRUM T
                                case 0xD8: /*RET C*/
                                if (af.b.l&C_FLAG)
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=16;
                                }
                                else
                                {
                                 cycles-=8;
                                }
                                break;

                                // t 4
                                case 0xD9: /*EXX*/
                                tempw=bc.w; bc.w=sbc.w; sbc.w=tempw;
                                tempw=de.w; de.w=sde.w; sde.w=tempw;
                                tempw=hl.w; hl.w=shl.w; shl.w=tempw;
                                cycles-=4;
                                break;

                                // t 12
                                case 0xDA: /*JP C,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&C_FLAG)
                                   pc=addr;
                                cycles-=12;
                                break;

                                // t 12
                                case 0xDB: /*IN A,(xx)*/
                                addr=readmem(pc); pc++;
                                af.b.h=z80in((af.b.h<<8)|addr);
                                cycles-=12;
                                break;

                                // t 8 12
                                case 0xDC: /*CALL C*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&C_FLAG)
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20;
                                }
                                else
                                {
                                cycles-=12;
                                }
                                break;


                                case 0xDD: /*More opcodes - IX*/
                                ir.b.l++;
                                opcode=readmem(pc); pc++;
                                switch (opcode)
                                {
                                        case 0x09: /*ADD IX,BC*/
                                        setadd16(ix.w,bc.w);
                                        ix.w+=bc.w;
                                        cycles-=16; //t 16
                                        break;
                                        case 0x19: /*ADD IX,DE*/
                                        setadd16(ix.w,de.w);
                                        ix.w+=de.w;
                                        cycles-=16; //t 16
                                        break;
                                        case 0x21: /*LD IX,xxxx*/
                                        ix.b.l=readmem(pc); pc++;
                                        ix.b.h=readmem(pc); pc++;
                                        cycles-=16; //t 16
                                        break;
                                        case 0x22: /*LD (xxxx),IX*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        writemem(addr,ix.b.l);
                                        writemem(addr+1,ix.b.h);
                                        cycles-=24; // t 24
                                        break;
                                        case 0x23: /*INC IX*/
                                        ix.w++;
                                        cycles-=12; // t 8
                                        break;
                                        case 0x24: /*INC IXh*/
                                        ix.b.h++;
                                        setinc(ix.b.h);
                                        cycles-=8;
                                        break;
                                        case 0x25: /*DEC IXh*/
                                        ix.b.h--;
                                        setdec(ix.b.h);
                                        cycles-=8;
                                        break;
                                        case 0x26: /*LD IXh,xx*/
                                        ix.b.h=readmem(pc); pc++;
                                        cycles-=12; // t 12
                                        break;
                                        case 0x29: /*ADD IX,IX*/
                                        setadd16(ix.w,ix.w);
                                        ix.w+=ix.w;
                                        cycles-=16; //t 16
                                        break;
                                        case 0x2A: /*LD IX,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        ix.b.l=readmem(addr);
                                        ix.b.h=readmem(addr+1);
                                        cycles-=24; // t 24
                                        break;
                                        case 0x2B: /*DEC IX*/
                                        ix.w--;
                                        cycles-=12; // 8
                                        break;
                                        case 0x2C: /*INC IXl*/
                                        ix.b.l++;
                                        setinc(ix.b.l);
                                        cycles-=8; // 8
                                        break;
                                        case 0x2D: /*DEC IXl*/
                                        ix.b.l--;
                                        setdec(ix.b.l);
                                        cycles-=8; // 8
                                        break;
                                        case 0x2E: /*LD IXl,xx*/
                                        ix.b.l=readmem(pc); pc++;
                                        cycles-=12; // t 12
                                        break;
                                        case 0x34: /*INC (IX+d)*/
                                        offset=(signed char)readmem(pc); pc++;
                                        temp=readmem(ix.w+offset)+1;
                                        setinc(temp);
                                        writemem(ix.w+offset,temp);
                                        cycles-=24; // t 12
                                        break;
                                        case 0x35: /*DEC (IX+d)*/
                                        offset=(signed char)readmem(pc); pc++;
                                        temp=readmem(ix.w+offset)-1;
                                        setdec(temp);
                                        writemem(ix.w+offset,temp);
                                        cycles-=24; // t 12
                                        break;
                                        case 0x36: /*LD (IX+d),xx*/
                                        offset=(signed char)readmem(pc); pc++;
                                        temp=readmem(pc); pc++;
                                        writemem(ix.w+offset,temp);
                                        cycles-=24; // t 24
                                        break;
                                        case 0x39: /*ADD IX,SP*/
                                        setadd16(ix.w,sp);
                                        ix.w+=sp;
                                        cycles-=16; // t 16
                                        break;

                                        case 0x44: bc.b.h=ix.b.h; cycles-=8; break; /*LD B,IXh*/ // NEW
                                        case 0x45: bc.b.h=ix.b.l; cycles-=8; break; /*LD B,IXl*/

                                        case 0x46: offset=(signed char)readmem(pc); pc++; bc.b.h=readmem(ix.w+offset); cycles-=20; break; /*LD B,(IX+d)*/ // t 24
                                        case 0x4E: offset=(signed char)readmem(pc); pc++; bc.b.l=readmem(ix.w+offset); cycles-=20; break; /*LD C,(IX+d)*/ // t 24
                                        case 0x56: offset=(signed char)readmem(pc); pc++; de.b.h=readmem(ix.w+offset); cycles-=20; break; /*LD D,(IX+d)*/ // t 24
                                        case 0x5E: offset=(signed char)readmem(pc); pc++; de.b.l=readmem(ix.w+offset); cycles-=20; break; /*LD E,(IX+d)*/ // t 24
                                        case 0x66: offset=(signed char)readmem(pc); pc++; hl.b.h=readmem(ix.w+offset); cycles-=20; break; /*LD H,(IX+d)*/ // t 24
                                        case 0x6E: offset=(signed char)readmem(pc); pc++; hl.b.l=readmem(ix.w+offset); cycles-=20; break; /*LD L,(IX+d)*/ // t 24
                                        case 0x7E: offset=(signed char)readmem(pc); pc++; af.b.h=readmem(ix.w+offset); cycles-=20; break; /*LD A,(IX+d)*/ // t 24

                                        case 0x4C: bc.b.l=ix.b.h; cycles-=8; break; /*LD C,IXh*/
                                        case 0x4D: bc.b.l=ix.b.l; cycles-=8; break; /*LD C,IXl*/

                                        case 0x54: de.b.h=ix.b.h; cycles-=8; break; /*LD D,IXh*/
                                        case 0x55: de.b.h=ix.b.l; cycles-=8; break; /*LD D,IXl*/

                                        case 0x5C: de.b.l=ix.b.h; cycles-=8; break; /*LD E,IXh*/
                                        case 0x5D: de.b.l=ix.b.l; cycles-=8; break; /*LD E,IXl*/

                                        case 0x60: ix.b.h=bc.b.h; cycles-=8; break; /*LD IXh,B*/
                                        case 0x61: ix.b.h=bc.b.l; cycles-=8; break; /*LD IXh,C*/
                                        case 0x62: ix.b.h=de.b.h; cycles-=8; break; /*LD IXh,B*/
                                        case 0x63: ix.b.h=de.b.l; cycles-=8; break; /*LD IXh,C*/
                                        case 0x64: ix.b.h=hl.b.h; cycles-=8; break; /*LD IXh,B*/
                                        case 0x65: ix.b.h=hl.b.l; cycles-=8; break; /*LD IXh,C*/
                                        case 0x67: ix.b.h=af.b.h; cycles-=8; break; /*LD IXh,A*/

                                        case 0x68: ix.b.l=bc.b.h; cycles-=8; break; /*LD IXl,B*/
                                        case 0x69: ix.b.l=bc.b.l; cycles-=8; break; /*LD IXl,C*/
                                        case 0x6A: ix.b.l=de.b.h; cycles-=8; break; /*LD IXl,B*/
                                        case 0x6B: ix.b.l=de.b.l; cycles-=8; break; /*LD IXl,C*/
                                        case 0x6C: ix.b.l=hl.b.h; cycles-=8; break; /*LD IXl,B*/
                                        case 0x6D: ix.b.l=hl.b.l; cycles-=8; break; /*LD IXl,C*/
                                        case 0x6F: ix.b.l=af.b.h; cycles-=8; break; /*LD IXl,A*/

                                        case 0x70: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,bc.b.h); cycles-=20; break; /*LD (IX+d),B*/ // t 24
                                        case 0x71: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,bc.b.l); cycles-=20; break; /*LD (IX+d),C*/ // t 24
                                        case 0x72: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,de.b.h); cycles-=20; break; /*LD (IX+d),D*/ // t 24
                                        case 0x73: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,de.b.l); cycles-=20; break; /*LD (IX+d),E*/ // t 24
                                        case 0x74: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,hl.b.h); cycles-=20; break; /*LD (IX+d),H*/ // t 24
                                        case 0x75: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,hl.b.l); cycles-=20; break; /*LD (IX+d),L*/ // t 24
                                        case 0x77: offset=(signed char)readmem(pc); pc++; writemem(ix.w+offset,af.b.h); cycles-=20; break; /*LD (IX+d),A*/ // t 24

                                        case 0x7C: af.b.h=ix.b.h; cycles-=8; break; /*LD A,IXh*/
                                        case 0x7D: af.b.h=ix.b.l; cycles-=8; break; /*LD A,IXl*/

                                        case 0x84: setadd(af.b.h,ix.b.h); af.b.h+=ix.b.h; cycles-=8; break; /*ADD IXh*/
                                        case 0x85: setadd(af.b.h,ix.b.l); af.b.h+=ix.b.l; cycles-=8; break; /*ADD IXl*/

                                        case 0x8C: setadc(af.b.h,ix.b.h); af.b.h+=ix.b.h+tempc; cycles-=4; break; /*ADC IXh*/
                                        case 0x8D: setadc(af.b.h,ix.b.l); af.b.h+=ix.b.l+tempc; cycles-=4; break; /*ADC IXl*/

                                        case 0x94: setsub(af.b.h,ix.b.h); af.b.h-=ix.b.h; cycles-=4; break; /*SUB IXh*/
                                        case 0x95: setsub(af.b.h,ix.b.l); af.b.h-=ix.b.l; cycles-=4; break; /*SUB IXl*/

                                        case 0x86: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); setadd(af.b.h,temp); af.b.h+=temp; cycles-=20; break; /*ADD (IX+d)*/ // t 28
                                        case 0x8E: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); setadc(af.b.h,temp); af.b.h+=temp+tempc; cycles-=20; break; /*ADC (IX+d)*/ // t 28
                                        case 0x96: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); setsub(af.b.h,temp); af.b.h-=temp; cycles-=20; break; /*SUB (IX+d)*/ // t 28
                                        case 0xA6: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); af.b.h&=temp; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=20; break;       /*AND (IX+d)*/ // t 28
                                        case 0xAE: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); af.b.h^=temp; setzn(af.b.h); cycles-=20; break;       /*XOR (IX+d)*/ // t 28
                                        case 0xB6: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); af.b.h|=temp; setzn(af.b.h); cycles-=20; break;       /*OR  (IX+d)*/ // t 28

                                        // NEW.
                                        case 0x9C: setsbc(af.b.h,ix.b.h); af.b.h-=(ix.b.h+tempc); cycles-=8; break; /*SBC IXh*/
                                        case 0x9D: setsbc(af.b.h,ix.b.l); af.b.h-=(ix.b.l+tempc); cycles-=8; break; /*SBC IXl*/

                                        case 0x9E: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); setsbc(af.b.h,temp); af.b.h-=temp; cycles-=20; break; /*SBC (IX+d)*/ // NEW


                                        // NEW.
                                        case 0xBC: setcp(af.b.h,ix.b.h); cycles-=8; break; /*CP IXh*/ // NEW
                                        case 0xBD: setcp(af.b.h,ix.b.l); cycles-=8; break; /*CP IXl*/ // NEW

                                        case 0xBE: offset=(signed char)readmem(pc); pc++; temp=readmem(ix.w+offset); setcp(af.b.h,temp); cycles-=20; break;                /*CP  (IX+d)*/ // t 28

                                        // NEW.
                                        case 0xAC: af.b.h^=ix.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=8; break; /*XOR IXh*/ // NEW
                                        case 0xAD: af.b.h^=ix.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=8; break; /*XOR IXL*/ // NEW

                                        case 0xA4: af.b.h&=ix.b.h; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=8; break; /*AND IXh*/
                                        case 0xA5: af.b.h&=ix.b.l; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=8; break; /*AND IXl*/

                                        case 0xB4: af.b.h|=ix.b.h; setzn(af.b.h); cycles-=8; break; /*OR IXh*/
                                        case 0xB5: af.b.h|=ix.b.l; setzn(af.b.h); cycles-=8; break; /*OR IXl*/

                                        case 0xCB: /*More opcodes*/
                                        offset=(signed char)readmem(pc); pc++;
                                        opcode=readmem(pc); pc++;
                                        ir.b.l++;
                                        switch (opcode)
                                        {
                                                case 0x06: temp=readmem(ix.w+offset); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; if (af.b.l&C_FLAG) temp|=1; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*RLC (IX+d)*/ // t 28
                                                case 0x0E: temp=readmem(ix.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (af.b.l&C_FLAG) temp|=0x80; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*RRC (IX+d)*/ // t 28
                                                case 0x16: temp=readmem(ix.w+offset); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; if (tempc) temp|=1; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*RL (IX+d)*/ // t 16
                                                case 0x1E: temp=readmem(ix.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (tempc) temp|=0x80; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*RR (IX+d)*/ // t 28
                                                case 0x26: temp=readmem(ix.w+offset); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*SLA (IX+d)*/ //t 28
                                                case 0x2E: temp=readmem(ix.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (temp&0x40) temp|=0x80; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*SRA (IX+d)*/
                                                case 0x3E: temp=readmem(ix.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; setznc(temp); writemem(ix.w+offset,temp); cycles-=28; break; /*SRA (IX+d)*/ // t 28

                                                case 0x46: temp=readmem(ix.w+offset); setznc(temp&0x01); cycles-=24; break; /*BIT 0,(IX+d)*/ // t 24
                                                case 0x4E: temp=readmem(ix.w+offset); setznc(temp&0x02); cycles-=24; break; /*BIT 1,(IX+d)*/ // t 24
                                                case 0x56: temp=readmem(ix.w+offset); setznc(temp&0x04); cycles-=24; break; /*BIT 2,(IX+d)*/ // t 24
                                                case 0x5E: temp=readmem(ix.w+offset); setznc(temp&0x08); cycles-=24; break; /*BIT 3,(IX+d)*/ // t 24
                                                case 0x66: temp=readmem(ix.w+offset); setznc(temp&0x10); cycles-=24; break; /*BIT 4,(IX+d)*/ // t 24
                                                case 0x6E: temp=readmem(ix.w+offset); setznc(temp&0x20); cycles-=24; break; /*BIT 5,(IX+d)*/ // t 24
                                                case 0x76: temp=readmem(ix.w+offset); setznc(temp&0x40); cycles-=24; break; /*BIT 6,(IX+d)*/ // t 24
                                                case 0x7E: temp=readmem(ix.w+offset); setznc(temp&0x80); cycles-=24; break; /*BIT 7,(IX+d)*/ // t 24

                                                case 0x86: temp=readmem(ix.w+offset)&~0x01; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 0,(IX+d)*/ // t 28
                                                case 0x8E: temp=readmem(ix.w+offset)&~0x02; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 1,(IX+d)*/ // t 28
                                                case 0x96: temp=readmem(ix.w+offset)&~0x04; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 2,(IX+d)*/ // t 28
                                                case 0x9E: temp=readmem(ix.w+offset)&~0x08; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 3,(IX+d)*/ // t 28
                                                case 0xA6: temp=readmem(ix.w+offset)&~0x10; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 4,(IX+d)*/ // t 28
                                                case 0xAE: temp=readmem(ix.w+offset)&~0x20; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 5,(IX+d)*/ // t 28
                                                case 0xB6: temp=readmem(ix.w+offset)&~0x40; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 6,(IX+d)*/ // t 28
                                                case 0xBE: temp=readmem(ix.w+offset)&~0x80; writemem(ix.w+offset,temp); cycles-=28; break; /*RES 7,(IX+d)*/ // t 28

                                                case 0xC6: temp=readmem(ix.w+offset)|0x01; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 0,(IX+d)*/  // t 28
                                                case 0xCE: temp=readmem(ix.w+offset)|0x02; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 1,(IX+d)*/  // t 28
                                                case 0xD6: temp=readmem(ix.w+offset)|0x04; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 2,(IX+d)*/  // t 28
                                                case 0xDE: temp=readmem(ix.w+offset)|0x08; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 3,(IX+d)*/  // t 28
                                                case 0xE6: temp=readmem(ix.w+offset)|0x10; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 4,(IX+d)*/  // t 28
                                                case 0xEE: temp=readmem(ix.w+offset)|0x20; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 5,(IX+d)*/  // t 28
                                                case 0xF6: temp=readmem(ix.w+offset)|0x40; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 6,(IX+d)*/  // t 28
                                                case 0xFE: temp=readmem(ix.w+offset)|0x80; writemem(ix.w+offset,temp); cycles-=28; break; /*SET 7,(IX+d)*/  // t 28

                                                default:
                                                //printf("Bad DD CB opcode %02X at %04X\n, opcode: %d",opcode,--pc, opcode);
                                                dumpregs();
                                                exit(-1);
                                        }
                                        break;

                                        case 0xCD: /*CALL*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20; // t 20
                                        break;

                                        case 0xB1: /*Invalid*/
                                        case 0xC5:
                                        case 0x02:
                                        case 0xDD: /*Another prefix*/
                                        case 0xFD:
                                        ir.b.l--;
                                        cycles-=4;
                                        pc--;
                                        break;

                                        case 0xE1: /*POP IX*/
                                        ix.b.l=pull();
                                        ix.b.h=pull();
                                        cycles-=20; // t 16
                                        break;

                                        case 0xE3: /*EX (SP),IX*/
                                        tempw=ix.w;
                                        ix.b.l=readmem(sp);
                                        ix.b.h=readmem(sp+1);
                                        writemem(sp,tempw&0xFF);
                                        writemem(sp+1,tempw>>8);
                                        cycles-=28; // t 28
                                        break;

                                        case 0xE5: /*PUSH IX*/
                                        push(ix.b.h);
                                        push(ix.b.l);
                                        cycles-=20; // t 20
                                        break;

                                        case 0xE9: /*JP (IX)*/
                                        pc=ix.w;
                                        cycles-=8;
                                        break;

                                        case 0xF9: /*LD SP,IX*/
                                        sp=ix.w;
                                        cycles-=12; // t 8
                                        break;

                                        default:
                                        //printf("Bad DD opcode %02X at %04X\n opcode: %d",opcode,--pc, opcode);
                                        dumpregs();
                                        exit(-1);
                                }
                                break;
                                case 0xDE: /*SBC xx*/
                                temp=readmem(pc); pc++;
                                setsbc(af.b.h,temp);
                                af.b.h-=(temp+tempc);
                                cycles-=8;
                                break;
                                case 0xDF: /*RST 18*/
                                push(pc>>8);
                                push(pc);
                                pc=0x18;
                                cycles-=16; // t 16
                                break;

                                case 0xE0: /*RET PO*/
                                if (!(af.b.l&P_FLAG))
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=12;
                                }
                                else
                                {
                                cycles-=8;
                                }
                                break;
                                case 0xE1: /*POP HL*/
                                hl.b.l=pull();
                                hl.b.h=pull();
                                cycles-=12; // t 12
                                break;
                                case 0xE2: /*JP PO,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&P_FLAG))
                                   pc=addr;
                                cycles-=16; // t 12
                                break;
                                case 0xE3: /*EX (SP),HL*/
                                tempw=hl.w;
                                hl.b.l=readmem(sp);
                                hl.b.h=readmem(sp+1);
                                writemem(sp,tempw&0xFF);
                                writemem(sp+1,tempw>>8);
                                cycles-=24; // t 24
                                break;
                                case 0xE4: /*CALL PO*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&P_FLAG))
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20;
                                }
                                else
                                {
                                cycles-=12;
                                }
                                break;
                                case 0xE5: /*PUSH HL*/
                                push(hl.b.h);
                                push(hl.b.l);
                                cycles-=16; // t 12
                                break;
                                case 0xE6: /*AND xx*/
                                af.b.h&=readmem(pc); pc++;
                                af.b.l&=~3;
                                af.b.l|=H_FLAG; 
                                setzn(af.b.h);
                                cycles-=8; // t 8
                                break;
                                case 0xE7: /*RST 20*/
                                push(pc>>8);
                                push(pc);
                                pc=0x20;
                                cycles-=16; //t 16
                                break;
                                case 0xE8: /*RET PE*/
                                if (af.b.l&P_FLAG)
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=16;
                                }
                                else
                                {
                                cycles-=8;
                                }
                                break;
                                case 0xE9: /*JP (HL)*/
                                pc=hl.w;
                                cycles-=4;
                                break;
                                case 0xEA: /*JP PE,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&P_FLAG)
                                   pc=addr;
                                cycles-=12; // t 12
                                break;
                                case 0xEB: /*EX DE,HL*/
                                tempw=de.w; de.w=hl.w; hl.w=tempw;
                                cycles-=4;
                                break;
                                case 0xEC: /*CALL PE*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&P_FLAG)
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=20; // t 8
                                }
                                else
                                {
                                cycles-=12; // t 12
                                }
                                break;
                                case 0xED: /*More opcodes*/
                                opcode=readmem(pc); pc++;
                                ir.b.l++;

                                switch (opcode)
                                {

                                        case 0x00: /*NOP*/
                                        case 0x01: /*NOP*/
                                        case 0x02: /*NOP*/
                                        case 0x03: /*NOP*/
                                        case 0x04: /*NOP*/
                                        case 0x05: /*NOP*/
                                        case 0x06: /*NOP*/
                                        case 0x07: /*NOP*/
                                        case 0x08: /*NOP*/
                                        case 0x09: /*NOP*/
                                        case 0x0A: /*NOP*/
                                        case 0x0B: /*NOP*/
                                        case 0x0C: /*NOP*/
                                        case 0x0D: /*NOP*/
                                        case 0x0E: /*NOP*/
                                        case 0x0F: /*NOP*/
                                        case 0x10: /*NOP*/
                                        case 0x11: /*NOP*/
                                        case 0x12: /*NOP*/
                                        case 0x13: /*NOP*/
                                        case 0x14: /*NOP*/
                                        case 0x15: /*NOP*/
                                        case 0x16: /*NOP*/
                                        case 0x17: /*NOP*/
                                        case 0x18: /*NOP*/
                                        case 0x19: /*NOP*/
                                        case 0x1A: /*NOP*/
                                        case 0x1B: /*NOP*/
                                        case 0x1C: /*NOP*/
                                        case 0x1D: /*NOP*/
                                        case 0x1E: /*NOP*/
                                        case 0x1F: /*NOP*/
                                        case 0x20: /*NOP*/
                                        case 0x21: /*NOP*/
                                        case 0x22: /*NOP*/
                                        case 0x23: /*NOP*/
                                        case 0x24: /*NOP*/
                                        case 0x25: /*NOP*/
                                        case 0x26: /*NOP*/
                                        case 0x27: /*NOP*/
                                        case 0x28: /*NOP*/
                                        case 0x29: /*NOP*/
                                        case 0x2A: /*NOP*/
                                        case 0x2B: /*NOP*/
                                        case 0x2C: /*NOP*/
                                        case 0x2D: /*NOP*/
                                        case 0x2E: /*NOP*/
                                        case 0x2F: /*NOP*/
                                        case 0x30: /*NOP*/
                                        case 0x31: /*NOP*/
                                        case 0x32: /*NOP*/
                                        case 0x33: /*NOP*/
                                        case 0x34: /*NOP*/
                                        case 0x35: /*NOP*/
                                        case 0x36: /*NOP*/
                                        case 0x37: /*NOP*/
                                        case 0x38: /*NOP*/
                                        case 0x39: /*NOP*/
                                        case 0x3A: /*NOP*/
                                        case 0x3B: /*NOP*/
                                        case 0x3C: /*NOP*/
                                        case 0x3D: /*NOP*/
                                        case 0x3E: /*NOP*/
                                        case 0x3F: /*NOP*/
                                        cycles-=8;
                                        break;

                                        case 0x40: /*IN B,(C)*/
                                        bc.b.h=z80in(bc.w);
                                        setzn(bc.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x41: /*OUT (C),B*/
                                        z80out(bc.w,bc.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x42: /*SBC HL,BC*/
                                        setsbc16(hl.w,bc.w);
                                        hl.w-=(bc.w+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x43: /*LD (xxxx),BC*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        writemem(addr,bc.b.l);
                                        writemem(addr+1,bc.b.h);
                                        cycles-=24;
                                        break;

                                        case 0x44: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x45: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x46: /*IM 0*/
                                        im=0;
                                        cycles-=8;
                                        break;

                                        case 0x47: /*LD I,A*/
                                        ir.b.h=af.b.h;
                                        cycles-=12;
                                        break;

                                        case 0x48: /*IN C,(C)*/
                                        bc.b.l=z80in(bc.w);
                                        setzn(bc.b.l);
                                        cycles-=16;
                                        break;

                                        case 0x49: /*OUT (C),C*/
                                        z80out(bc.w,bc.b.l);
                                        cycles-=16;
                                        break;

                                        case 0x4A: /*ADC HL,BC*/
                                        setadc16(hl.w,bc.w);
                                        hl.w+=(bc.w+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x4B: /*LD BC,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        bc.b.l=readmem(addr);
                                        bc.b.h=readmem(addr+1);
                                        cycles-=24;
                                        break;

                                        case 0x4C: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x4D: /*RETI*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x4E: /*IM 0*/
                                        im=0;
                                        cycles-=8;
                                        break;

                                        case 0x4F: /*LD R,A*/
                                        ir.b.l=af.b.h;
                                        rhigh=af.b.h&0x80;
                                        cycles-=12;
                                        break;

                                        case 0x50: /*IN D,(C)*/
                                        de.b.h=z80in(bc.w);
                                        setzn(de.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x51: /*OUT (C),D*/
                                        z80out(bc.w,de.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x52: /*SBC HL,DE*/
                                        setsbc16(hl.w,de.w);
                                        hl.w-=(de.w+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x53: /*LD (xxxx),DE*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        writemem(addr,de.b.l);
                                        writemem(addr+1,de.b.h);
                                        cycles-=24;
                                        break;

                                        case 0x54: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x55: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x56: /*IM 1*/
                                        im=1;
                                        cycles-=8;
                                        break;

                                        case 0x57: /*LD A,I*/
                                        af.b.h=ir.b.h;
                                        cycles-=12; // t 8
                                        break;

                                        case 0x58: /*IN E,(C)*/
                                        de.b.l=z80in(bc.w);
                                        setzn(de.b.l);
                                        cycles-=16;
                                        break;

                                        case 0x59: /*OUT (C),E*/
                                        z80out(bc.w,de.b.l);
                                        cycles-=16;
                                        break;

                                        case 0x5A: /*ADC HL,DE*/
                                        setadc16(hl.w,de.w);
                                        hl.w+=(de.w+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x5B: /*LD DE,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        de.b.l=readmem(addr);
                                        de.b.h=readmem(addr+1);
                                        cycles-=24;
                                        break;

                                        case 0x5C: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x5D: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x5E: /*IM 2*/
                                        im=2;
                                        cycles-=8;
                                        break;

                                        case 0x5F: /*LD A,R*/
                                        af.b.h=(ir.b.l&0x7F)|rhigh;
                                        cycles-=12;
                                        break;

                                        case 0x60: /*IN H,(C)*/
                                        hl.b.h=z80in(bc.w);
                                        setzn(hl.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x61: /*OUT (C),H*/
                                        z80out(bc.w,hl.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x62: /*SBC HL,HL*/
                                        setsbc16(hl.w,hl.w);
                                        hl.w-=(hl.w+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x63: /*LD (xxxx),HL*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        writemem(addr,hl.b.l);
                                        writemem(addr+1,hl.b.h);
                                        cycles-=24;
                                        break;

                                        case 0x64: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x65: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x66: /*IM 0*/
                                        im=0;
                                        cycles-=8;
                                        break;

                                        case 0x67: /*RRD*/
                                        temp=readmem(hl.w);
                                        temp2=temp&0xF;
                                        temp=(temp>>4)|(af.b.h<<4);
                                        af.b.h=(af.b.h&0xF0)|temp2;
                                        writemem(hl.w,temp);
                                        setznc(af.b.h);
                                        cycles-=20;
                                        break;

                                        // NEW
                                        case 0x68: /*IN L,(C)*/
                                        hl.b.l=z80in(bc.w);
                                        setzn(hl.b.l);
                                        cycles-=16;
                                        break;

                                        // NEW
                                        case 0x69: /*OUT (C),L*/
                                        z80out(bc.w,hl.b.l);
                                        cycles-=16;
                                        break;

                                        case 0x6A: /*ADC HL,HL*/
                                        setadc16(hl.w,hl.w);
                                        hl.w+=(hl.w+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x6B: /*LD HL,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        hl.b.l=readmem(addr);
                                        hl.b.h=readmem(addr+1);
                                        cycles-=24;
                                        break;

                                        case 0x6C: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x6D: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x6E: /*IM 0*/
                                        im=0;
                                        cycles-=8;
                                        break;

                                        case 0x6F: /*RLD*/
                                        temp=readmem(hl.w);
                                        temp2=temp>>4;
                                        temp=(temp<<4)|(af.b.h&0xF);
                                        af.b.h=(af.b.h&0xF0)|temp2;
                                        writemem(hl.w,temp);
                                        setznc(af.b.h);
                                        cycles-=20;
                                        break;

                                        // These seem a bit unknown!
//                                        case 0x70: /*IN L,(C)*/
//                                        hl.b.l=z80in(bc.w);
//                                        setzn(hl.b.l);
//                                        cycles-=12;
//                                        break;

                                        // NEW
                                        case 0x71: /*OUT (C),0*/
                                        z80out(bc.w,0);
                                        cycles-=16;
                                        break;

                                        case 0x72: /*SBC HL,SP*/
                                        setsbc16(hl.w,sp);
                                        hl.w-=(sp+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x73: /*LD (xxxx),SP*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        writemem(addr,sp&0xFF);
                                        writemem(addr+1,sp>>8);
                                        cycles-=24;
                                        break;

                                        case 0x74: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x75: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x76: /*IM 1*/
                                        im=1;
                                        cycles-=8;
                                        break;

                                        case 0x77: /*NOP*/
                                        cycles-=8;
                                        break;

                                        case 0x78: /*IN A,(C)*/
                                        af.b.h=z80in(bc.w);
                                        setzn(af.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x79: /*OUT (C),A*/
                                        z80out(bc.w,af.b.h);
                                        cycles-=16;
                                        break;

                                        case 0x7A: /*ADC HL,SP*/
                                        setadc16(hl.w,sp);
                                        hl.w+=(sp+tempc);
                                        cycles-=16;
                                        break;

                                        case 0x7B: /*LD SP,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        sp=readmem(addr);
                                        sp|=(readmem(addr+1)<<8);
                                        cycles-=24;
                                        break;

                                        case 0x7C: /*NEG*/
                                        setsub(0,af.b.h);
                                        af.b.h=0-af.b.h;
                                        cycles-=8;
                                        break;

                                        case 0x7D: /*RETN*/
                                        iff1=iff2;
                                        pc=pull();
                                        pc|=(pull()<<8);
                                        cycles-=16; // t 16
                                        break;

                                        case 0x7E: /*IM 2*/
                                        im=2;
                                        cycles-=8;
                                        break;

                                        case 0x7F: /*NOP*/
                                        case 0x80: /*NOP*/
                                        case 0x81: /*NOP*/
                                        case 0x82: /*NOP*/
                                        case 0x83: /*NOP*/
                                        case 0x84: /*NOP*/
                                        case 0x85: /*NOP*/
                                        case 0x86: /*NOP*/
                                        case 0x87: /*NOP*/
                                        case 0x88: /*NOP*/
                                        case 0x89: /*NOP*/
                                        case 0x8A: /*NOP*/
                                        case 0x8B: /*NOP*/
                                        case 0x8C: /*NOP*/
                                        case 0x8D: /*NOP*/
                                        case 0x8E: /*NOP*/
                                        case 0x8F: /*NOP*/
                                        case 0x90: /*NOP*/
                                        case 0x91: /*NOP*/
                                        case 0x92: /*NOP*/
                                        case 0x93: /*NOP*/
                                        case 0x94: /*NOP*/
                                        case 0x95: /*NOP*/
                                        case 0x96: /*NOP*/
                                        case 0x97: /*NOP*/
                                        case 0x98: /*NOP*/
                                        case 0x99: /*NOP*/
                                        case 0x9A: /*NOP*/
                                        case 0x9B: /*NOP*/
                                        case 0x9C: /*NOP*/
                                        case 0x9D: /*NOP*/
                                        case 0x9E: /*NOP*/
                                        case 0x9F: /*NOP*/
                                        cycles-=8;
                                        break;

                                        case 0xA0: /*LDI*/
                                        temp=readmem(hl.w); hl.w++;
                                        writemem(de.w,temp); de.w++;
                                        bc.w--;
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xA1: /*CPI*/
//                                        printf("CPI %04X %i\n",pc,curhrom);
                                        temp=readmem(hl.w); hl.w++;
                                        if (af.b.h-temp) af.b.l&=~Z_FLAG;
                                        else             af.b.l|=Z_FLAG;
                                        bc.w--;
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xA2: /*INI*/
                                        temp=z80in(bc.w);
                                        writemem(hl.w,temp);
                                        hl.w++;
                                        bc.b.h--;
                                        if (bc.b.h) af.b.l|=P_FLAG;
                                        else        af.b.l&=~P_FLAG;
                                        af.b.l|=S_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xA3: /*OUTI*/
                                        temp=readmem(hl.w); hl.w++;
                                        bc.b.h--;
                                        z80out(bc.w,temp);
                                        if (bc.b.h) af.b.l|=P_FLAG;
                                        else        af.b.l&=~P_FLAG;
                                        af.b.l|=S_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xA4: /*NOP*/
                                        case 0xA5: /*NOP*/
                                        case 0xA6: /*NOP*/
                                        case 0xA7: /*NOP*/
                                        cycles-=8;
                                        break;

                                        case 0xA8: /*LDD*/
                                        temp=readmem(hl.w); hl.w--;
                                        writemem(de.w,temp); de.w--;
                                        bc.w--;
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        cycles-=20; //T 20
                                        break;

                                        case 0xA9: /*CPD*/
                                        //printf("CPD %04X %i\n",pc,curhrom);
                                        temp=readmem(hl.w); hl.w--;
                                        if (af.b.h-temp) af.b.l&=~Z_FLAG;
                                        else             af.b.l|=Z_FLAG;
                                        bc.w--;
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xAB: /*OUTD*/
                                        temp=readmem(hl.w); hl.w--;
                                        bc.b.h--;
                                        z80out(bc.w,temp);
                                        if (bc.b.h) af.b.l|=P_FLAG;
                                        else        af.b.l&=~P_FLAG;
                                        af.b.l|=S_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xAC: /*NOP*/
                                        case 0xAD: /*NOP*/
                                        case 0xAE: /*NOP*/
                                        case 0xAF: /*NOP*/
                                        cycles-=8;
                                        break;

                                        case 0xB0: /*LDIR*/
                                        temp=readmem(hl.w); hl.w++;
                                        writemem(de.w,temp); de.w++;
                                        bc.w--;
                                        if (bc.w)
                                        {
                                                pc-=2;
                                                cycles-=24;
                                        }
                                        else
                                           cycles-=20;
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        break;

                                        case 0xB1: /*CPIR*/
                                        temp=readmem(hl.w); hl.w++;
                                        if (af.b.h==temp) af.b.l|=Z_FLAG;
                                        else              af.b.l&=~Z_FLAG;
                                        bc.w--;
                                        if (bc.w && af.b.h!=temp) { pc-=2; cycles-=4; }
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xB4: /*NOP*/
                                        case 0xB5: /*NOP*/
                                        case 0xB6: /*NOP*/
                                        case 0xB7: /*NOP*/
                                        cycles-=8;
                                        break;

                                        case 0xB8: /*LDDR*/
                                        temp=readmem(hl.w); hl.w--;
                                        writemem(de.w,temp); de.w--;
                                        bc.w--;
                                        if (bc.w)
                                        {
                                                pc-=2;
                                                cycles-=24;
                                        }
                                        else
                                           cycles-=20;
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        break;

                                        case 0xB9: /*CPDR*/
                                        //printf("CPDR %04X %i\n",pc,curhrom);
                                        temp=readmem(hl.w); hl.w--;
                                        setcp(af.b.h,temp);
                                        bc.w--;
                                        if (bc.w && af.b.h!=temp) { pc-=2; cycles-=8; }
                                        if (bc.w) af.b.l|=P_FLAG;
                                        else      af.b.l&=~P_FLAG;
                                        cycles-=20;
                                        break;

                                        case 0xBC: /*NOP*/
                                        case 0xBD: /*NOP*/
                                        case 0xBE: /*NOP*/
                                        case 0xBF: /*NOP*/
                                        case 0xC0: /*NOP*/
                                        case 0xC1: /*NOP*/
                                        case 0xC2: /*NOP*/
                                        case 0xC3: /*NOP*/
                                        case 0xC4: /*NOP*/
                                        case 0xC5: /*NOP*/
                                        case 0xC6: /*NOP*/
                                        case 0xC7: /*NOP*/
                                        case 0xC8: /*NOP*/
                                        case 0xC9: /*NOP*/
                                        case 0xCA: /*NOP*/
                                        case 0xCB: /*NOP*/
                                        case 0xCC: /*NOP*/
                                        case 0xCD: /*NOP*/
                                        case 0xCE: /*NOP*/
                                        case 0xCF: /*NOP*/
                                        case 0xD0: /*NOP*/
                                        case 0xD1: /*NOP*/
                                        case 0xD2: /*NOP*/
                                        case 0xD3: /*NOP*/
                                        case 0xD4: /*NOP*/
                                        case 0xD5: /*NOP*/
                                        case 0xD6: /*NOP*/
                                        case 0xD7: /*NOP*/
                                        case 0xD8: /*NOP*/
                                        case 0xD9: /*NOP*/
                                        case 0xDA: /*NOP*/
                                        case 0xDB: /*NOP*/
                                        case 0xDC: /*NOP*/
                                        case 0xDD: /*NOP*/
                                        case 0xDE: /*NOP*/
                                        case 0xDF: /*NOP*/
                                        case 0xE0: /*NOP*/
                                        case 0xE1: /*NOP*/
                                        case 0xE2: /*NOP*/
                                        case 0xE3: /*NOP*/
                                        case 0xE4: /*NOP*/
                                        case 0xE5: /*NOP*/
                                        case 0xE6: /*NOP*/
                                        case 0xE7: /*NOP*/
                                        case 0xE8: /*NOP*/
                                        case 0xE9: /*NOP*/
                                        case 0xEA: /*NOP*/
                                        case 0xEB: /*NOP*/
                                        case 0xEC: /*NOP*/
                                        case 0xED: /*NOP*/
                                        case 0xEE: /*NOP*/
                                        case 0xEF: /*NOP*/
                                        case 0xF0: /*NOP*/
                                        case 0xF1: /*NOP*/
                                        case 0xF2: /*NOP*/
                                        case 0xF3: /*NOP*/
                                        case 0xF4: /*NOP*/
                                        case 0xF5: /*NOP*/
                                        case 0xF6: /*NOP*/
                                        case 0xF7: /*NOP*/
                                        case 0xF8: /*NOP*/
                                        case 0xF9: /*NOP*/
                                        case 0xFA: /*NOP*/
                                        case 0xFB: /*NOP*/
                                        case 0xFC: /*NOP*/
                                        case 0xFD: /*NOP*/
                                        case 0xFE: /*NOP*/
                                        case 0xFF: /*NOP*/
                                        cycles-=8;
                                        break;

                                        default:
                                        //printf("Bad ED opcode %02X at %04X\n",opcode,--pc);
                                        dumpregs();
                                        exit(-1);
                                }

                                break;
                                case 0xEE: /*XOR xx*/
                                af.b.h^=readmem(pc); pc++;
                                af.b.l&=~3;
                                setzn(af.b.h);
                                cycles-=8;
                                break;
                                case 0xEF: /*RST 28*/
                                push(pc>>8);
                                push(pc);
                                pc=0x28;
                                cycles-=16;
                                break;

                                case 0xF0: /*RET P*/
                                if (!(af.b.l&N_FLAG))
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=8;
                                }
                                cycles-=8;
                                break;
                                case 0xF1: /*POP AF*/
                                af.b.l=pull();
                                af.b.h=pull();
                                cycles-=12;
                                break;
                                case 0xF2: /*JP P,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&N_FLAG))
                                   pc=addr;
                                cycles-=12;
                                break;
                                case 0xF3: /*DI*/
                                iff1=0;
                                cycles-=4;
                                break;
                                case 0xF4: /*CALL P*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (!(af.b.l&N_FLAG))
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=8;
                                }
                                cycles-=12;
                                break;
                                case 0xF5: /*PUSH AF*/
                                push(af.b.h);
                                push(af.b.l);
                                cycles-=16;
                                break;
                                case 0xF6: /*OR xx*/
                                af.b.h|=readmem(pc); pc++;
                                af.b.l&=~3;
                                setzn(af.b.h);
                                cycles-=8;
                                break;
                                case 0xF7: /*RST 30*/
                                push(pc>>8);
                                push(pc);
                                pc=0x30;
                                cycles-=16;
                                break;
                                case 0xF8: /*RET M*/
                                if (af.b.l&N_FLAG)
                                {
                                        pc=pull();
                                        pc|=pull()<<8;
                                        cycles-=8;
                                }
                                cycles-=8;
                                break;
                                case 0xF9: /*LD SP,HL*/
                                sp=hl.w;
                                cycles-=4;
                                break;
                                case 0xFA: /*JP M,xxxx*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&N_FLAG)
                                   pc=addr;
                                cycles-=12;
                                break;
                                case 0xFB: /*EI*/
                                iff1=2;
                                if (intreq) intreq=-1;
                                cycles-=4;
                                break;
                                case 0xFC: /*CALL M*/
                                addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                if (af.b.l&N_FLAG)
                                {
                                        push(pc>>8);
                                        push(pc&0xFF);
                                        pc=addr;
                                        cycles-=8;
                                }
                                cycles-=12;
                                break;
                                case 0xFD: /*More opcodes - IY*/
                                ir.b.l++;
                                opcode=readmem(pc); pc++;
                                switch (opcode)
                                {
                                        case 0x00: case 0x4D: case 0xE0: ir.b.l--; pc--; cycles-=4; break;
                                        case 0x09: /*ADD IY,BC*/
                                        setadd16(iy.w,bc.w);
                                        iy.w+=bc.w;
                                        cycles-=16;
                                        break;
                                        case 0x19: /*ADD IY,DE*/
                                        setadd16(iy.w,de.w);
                                        iy.w+=de.w;
                                        cycles-=16;
                                        break;
                                        case 0x21: /*LD IY,xxxx*/
                                        iy.b.l=readmem(pc); pc++;
                                        iy.b.h=readmem(pc); pc++;
                                        cycles-=16;
                                        break;
                                        case 0x22: /*LD (xxxx),IY*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        writemem(addr,iy.b.l);
                                        writemem(addr+1,iy.b.h);
                                        cycles-=24;
                                        break;
                                        case 0x23: /*INC IY*/
                                        iy.w++;
                                        cycles-=12;
                                        break;

                                        // NEW
                                        case 0x24: /*INC IYh*/
                                        iy.b.h++;
                                        setinc(iy.b.h);
                                        cycles-=8;
                                        break;
                                        case 0x25: /*DEC IYh*/
                                        iy.b.h--;
                                        setdec(iy.b.h);
                                        cycles-=8;
                                        break;

                                        case 0x26: /*LD IYh,xx*/
                                        iy.b.h=readmem(pc); pc++;
                                        cycles-=12;
                                        break;
                                        case 0x29: /*ADD IY,IY*/
                                        setadd16(iy.w,iy.w);
                                        iy.w+=iy.w;
                                        cycles-=16;
                                        break;
                                        case 0x2A: /*LD IY,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        iy.b.l=readmem(addr);
                                        iy.b.h=readmem(addr+1);
                                        cycles-=24;
                                        break;
                                        case 0x2B: /*DEC IY*/
                                        iy.w--;
                                        cycles-=12;
                                        break;
                                        case 0x2C: /*INC IYl*/
                                        iy.b.l++;
                                        setinc(iy.b.l);
                                        cycles-=8;
                                        break;
                                        case 0x2D: /*DEC IYl*/
                                        iy.b.l--;
                                        setdec(iy.b.l);
                                        cycles-=8;
                                        break;

                                        // NEW
                                        case 0x2E: /*LD IYl,xx*/
                                        iy.b.l=readmem(pc); pc++;
                                        cycles-=12; // t 12
                                        break;
                                        case 0x34: /*INC (IY+d)*/
                                        offset=(signed char)readmem(pc); pc++;
                                        temp=readmem(iy.w+offset)+1;
                                        setinc(temp);
                                        writemem(iy.w+offset,temp);
                                        cycles-=24;
                                        break;
                                        case 0x35: /*DEC (IY+d)*/
                                        offset=(signed char)readmem(pc); pc++;
                                        temp=readmem(iy.w+offset)-1;
                                        setdec(temp);
                                        writemem(iy.w+offset,temp);
                                        cycles-=24;
                                        break;
                                        case 0x36: /*LD (IY+d),xx*/
                                        offset=(signed char)readmem(pc); pc++;
                                        temp=readmem(pc); pc++;
                                        writemem(iy.w+offset,temp);
                                        cycles-=24;
                                        break;
                                        case 0x39: /*ADD IY,SP*/
                                        setadd16(iy.w,sp);
                                        iy.w+=sp;
                                        cycles-=16;
                                        break;
                                        case 0x3A: /*LD A,(xxxx)*/
                                        addr=readmem(pc)|(readmem(pc+1)<<8); pc+=2;
                                        af.b.h=readmem(addr);
                                        cycles-=20;
                                        break;

                                        // t 4
                                        case 0x3B: /*DEC SP*/
                                        sp--;
                                        cycles-=12;
                                        break;

                                        // t 4
                                        case 0x3C: /*INC A*/
                                        af.b.h++;
                                        setinc(af.b.h);
                                        cycles-=4;
                                        break;

                                        // t 4
                                        case 0x3D: /*DEC A*/
                                        af.b.h--;
                                        setdec(af.b.h);
                                        cycles-=4;
                                        break;

                                        // t 8
                                        case 0x3E: /*LD A,xx*/
                                        af.b.h=readmem(pc); pc++;
                                        cycles-=8;
                                        break;

                                        // t 4
                                        case 0x3F: /*CCF*/
                                        if (af.b.l&C_FLAG) af.b.l|=H_FLAG;
                                        else               af.b.l&=~H_FLAG;
                                        af.b.l^=C_FLAG;
                                        cycles-=4;
                                        break;

                                        case 0x40: bc.b.h=bc.b.h;        cycles-=8; break; /*LD B,B*/
                                        case 0x41: bc.b.h=bc.b.l;        cycles-=8; break; /*LD B,C*/
                                        case 0x42: bc.b.h=de.b.h;        cycles-=8; break; /*LD B,D*/
                                        case 0x43: bc.b.h=de.b.l;        cycles-=8; break; /*LD B,E*/
                                        case 0x44: bc.b.h=iy.b.h; cycles-=8; break; /*LD B,IYh*/ // NEW
                                        case 0x45: bc.b.h=iy.b.l; cycles-=8; break; /*LD B,IYl*/
                                        case 0x46: offset=(signed char)readmem(pc); pc++; bc.b.h=readmem(iy.w+offset); cycles-=20; break; /*LD B,(IY+d)*/
                                        case 0x4E: offset=(signed char)readmem(pc); pc++; bc.b.l=readmem(iy.w+offset); cycles-=20; break; /*LD C,(IY+d)*/
                                        case 0x4F: bc.b.l=af.b.h;        cycles-=8; break; /*LD C,A*/
                                        case 0x50: de.b.h=bc.b.h;        cycles-=8; break; /*LD D,B*/
                                        case 0x51: de.b.h=bc.b.l;        cycles-=8; break; /*LD D,C*/
                                        case 0x52: de.b.h=de.b.h;        cycles-=8; break; /*LD D,D*/
                                        case 0x53: de.b.h=de.b.l;        cycles-=8; break; /*LD D,E*/
                                        case 0x54: de.b.h=iy.b.h; cycles-=8; break; /*LD D,IYh*/
                                        case 0x55: de.b.h=iy.b.l; cycles-=8; break; /*LD D,IYl*/
                                        case 0x56: offset=(signed char)readmem(pc); pc++; de.b.h=readmem(iy.w+offset); cycles-=20; break; /*LD D,(IY+d)*/
                                        case 0x57: de.b.h=af.b.h;        cycles-=8; break; /*LD D,A*/
                                        case 0x58: de.b.l=bc.b.h;        cycles-=8; break; /*LD E,B*/
                                        case 0x59: de.b.l=bc.b.l;        cycles-=8; break; /*LD E,C*/
                                        case 0x5A: de.b.l=de.b.h;        cycles-=8; break; /*LD E,D*/
                                        case 0x5B: de.b.l=de.b.l;        cycles-=8; break; /*LD E,E*/
                                        case 0x5C: de.b.l=iy.b.h; cycles-=8; break; /*LD E,IYh*/
                                        case 0x5D: de.b.l=iy.b.l; cycles-=8; break; /*LD E,IYl*/
                                        case 0x5E: offset=(signed char)readmem(pc); pc++; de.b.l=readmem(iy.w+offset); cycles-=20; break; /*LD E,(IY+d)*/
                                        case 0x5F: de.b.l=af.b.h; cycles-=8; break; /*LD E,A*/

                                        case 0x60: iy.b.h=bc.b.h; cycles-=8; break; /*LD IYh,B*/
                                        case 0x61: iy.b.h=bc.b.l; cycles-=8; break; /*LD IYh,C*/
                                        case 0x62: iy.b.h=de.b.h; cycles-=8; break; /*LD IYh,B*/
                                        case 0x63: iy.b.h=de.b.l; cycles-=8; break; /*LD IYh,C*/
                                        case 0x64: iy.b.h=hl.b.h; cycles-=8; break; /*LD IYh,B*/
                                        case 0x65: iy.b.h=hl.b.l; cycles-=8; break; /*LD IYh,C*/
                                        case 0x66: offset=(signed char)readmem(pc); pc++; hl.b.h=readmem(iy.w+offset); cycles-=20; break; /*LD H,(IY+d)*/
                                        case 0x67: iy.b.h=af.b.h; cycles-=8; break; /*LD IYh,A*/
                                        case 0x68: iy.b.l=bc.b.h; cycles-=8; break; /*LD IYl,B*/
                                        case 0x69: iy.b.l=bc.b.l; cycles-=8; break; /*LD IYl,C*/
                                        case 0x6A: iy.b.l=de.b.h; cycles-=8; break; /*LD IYl,B*/
                                        case 0x6B: iy.b.l=de.b.l; cycles-=8; break; /*LD IYl,C*/
                                        case 0x6C: iy.b.l=hl.b.h; cycles-=8; break; /*LD IYl,B*/
                                        case 0x6D: iy.b.l=hl.b.l; cycles-=8; break; /*LD IYl,C*/
                                        case 0x6E: offset=(signed char)readmem(pc); pc++; hl.b.l=readmem(iy.w+offset); cycles-=20; break; /*LD L,(IY+d)*/
                                        case 0x6F: iy.b.l=af.b.h; cycles-=8; break; /*LD IYl,A*/

                                        case 0x70: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,bc.b.h); cycles-=20; break; /*LD (IY+d),B*/
                                        case 0x71: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,bc.b.l); cycles-=20; break; /*LD (IY+d),C*/
                                        case 0x72: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,de.b.h); cycles-=20; break; /*LD (IY+d),D*/
                                        case 0x73: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,de.b.l); cycles-=20; break; /*LD (IY+d),E*/
                                        case 0x74: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,hl.b.h); cycles-=20; break; /*LD (IY+d),H*/
                                        case 0x75: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,hl.b.l); cycles-=20; break; /*LD (IY+d),L*/

                                        case 0x76: /*HALT*/
                                        if (!intreq) pc--;
                                        cycles-=8;
                                        break;

                                        case 0x77: offset=(signed char)readmem(pc); pc++; writemem(iy.w+offset,af.b.h); cycles-=20; break; /*LD (IY+d),A*/
                                        case 0x78: af.b.h=bc.b.h;        cycles-=8; break; /*LD A,B*/
                                        case 0x79: af.b.h=bc.b.l;        cycles-=8; break; /*LD A,C*/
                                        case 0x7A: af.b.h=de.b.h;        cycles-=8; break; /*LD A,D*/
                                        case 0x7B: af.b.h=de.b.l;        cycles-=8; break; /*LD A,E*/
                                        case 0x7C: af.b.h=iy.b.h; cycles-=8; break; /*LD A,IYh*/
                                        case 0x7D: af.b.h=iy.b.l; cycles-=8; break; /*LD A,IYl*/
                                        case 0x7E: offset=(signed char)readmem(pc); pc++; af.b.h=readmem(iy.w+offset); cycles-=20; break; /*LD A,(IY+d)*/
                                        case 0x7F: af.b.h=af.b.h;        cycles-=8; break; /*LD A,A*/
                                        case 0x80: setadd(af.b.h,bc.b.h); af.b.h+=bc.b.h; cycles-=8; break; /*ADD B*/
                                        case 0x81: setadd(af.b.h,bc.b.l); af.b.h+=bc.b.l; cycles-=8; break; /*ADD C*/
                                        case 0x82: setadd(af.b.h,de.b.h); af.b.h+=de.b.h; cycles-=8; break; /*ADD D*/
                                        case 0x83: setadd(af.b.h,de.b.l); af.b.h+=de.b.l; cycles-=8; break; /*ADD E*/
                                        case 0x84: setadd(af.b.h,iy.b.h); af.b.h+=iy.b.h; cycles-=8; break; /*ADD IYh*/
                                        case 0x85: setadd(af.b.h,iy.b.l); af.b.h+=iy.b.l; cycles-=8; break; /*ADD IYl*/
                                        case 0x86: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); setadd(af.b.h,temp); af.b.h+=temp; cycles-=20; break; /*ADD (IY+d)*/
                                        case 0x87: setadd(af.b.h,af.b.h); af.b.h+=af.b.h; cycles-=8; break; /*ADD A*/
                                        case 0x88: setadc(af.b.h,bc.b.h); af.b.h+=bc.b.h+tempc; cycles-=8; break; /*ADC B*/
                                        case 0x89: setadc(af.b.h,bc.b.l); af.b.h+=bc.b.l+tempc; cycles-=8; break; /*ADC C*/
                                        case 0x8A: setadc(af.b.h,de.b.h); af.b.h+=de.b.h+tempc; cycles-=8; break; /*ADC D*/
                                        case 0x8B: setadc(af.b.h,de.b.l); af.b.h+=de.b.l+tempc; cycles-=8; break; /*ADC E*/
                                        case 0x8E: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); setadc(af.b.h,temp); af.b.h+=temp+tempc; cycles-=20; break; /*ADC (IY+d)*/ // t 28

                                        case 0x8C: setadc(af.b.h,iy.b.h); af.b.h+=iy.b.h+tempc; cycles-=8; break; /*ADC IYh*/
                                        case 0x8D: setadc(af.b.h,iy.b.l); af.b.h+=iy.b.l+tempc; cycles-=8; break; /*ADC IYl*/

                                        case 0x94: setsub(af.b.h,iy.b.h); af.b.h-=iy.b.h; cycles-=8; break; /*SUB IYh*/
                                        case 0x95: setsub(af.b.h,iy.b.l); af.b.h-=iy.b.l; cycles-=8; break; /*SUB IYl*/

                                        case 0x96: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); setsub(af.b.h,temp); af.b.h-=temp; cycles-=20; break; /*SUB (IY+d)*/
                                        case 0x9C: setsbc(af.b.h,iy.b.h); af.b.h-=(iy.b.h+tempc); cycles-=8; break; /*SBC IYh*/
                                        case 0x9D: setsbc(af.b.h,iy.b.l); af.b.h-=(iy.b.l+tempc); cycles-=8; break; /*SBC IYl*/
                                        case 0x9E: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); setsbc(af.b.h,temp); af.b.h-=(temp+tempc); cycles-=20; break; /*SBC (IY+d)*/
                                        case 0xA6: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); af.b.h&=temp; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=20; break;       /*AND (IY+d)*/
                                        case 0xAE: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); af.b.h^=temp; setzn(af.b.h); cycles-=20; break;       /*XOR (IY+d)*/
                                        case 0xB6: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); af.b.h|=temp; setzn(af.b.h); cycles-=20; break;       /*OR  (IY+d)*/
                                        case 0xBE: offset=(signed char)readmem(pc); pc++; temp=readmem(iy.w+offset); setcp(af.b.h,temp); cycles-=20; break;                /*CP  (IY+d)*/


                                        case 0xA4: af.b.h&=iy.b.h; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=8; break; /*AND IYh*/
                                        case 0xA5: af.b.h&=iy.b.l; setzn(af.b.h); af.b.l&=~3; af.b.l|=H_FLAG; cycles-=8; break; /*AND IYl*/

                                        case 0xB4: af.b.h|=iy.b.h; setzn(af.b.h); cycles-=8; break; /*OR IYh*/
                                        case 0xB5: af.b.h|=iy.b.l; setzn(af.b.h); cycles-=8; break; /*OR IYl*/
                                        // NEW.
                                        case 0xAC: af.b.h^=iy.b.h; setzn(af.b.h); af.b.l&=~3; cycles-=8; break; /*XOR IYh*/ // NEW
                                        case 0xAD: af.b.h^=iy.b.l; setzn(af.b.h); af.b.l&=~3; cycles-=8; break; /*XOR IYL*/ // NEW

                                        // NEW.
                                        case 0xBC: setcp(af.b.h,iy.b.h); cycles-=8; break; /*CP IYh*/ // NEW
                                        case 0xBD: setcp(af.b.h,iy.b.l); cycles-=8; break; /*CP IYl*/ // NEW


                                        case 0xCB: /*More opcodes*/
                                        offset=(signed char)readmem(pc); pc++;
                                        opcode=readmem(pc); pc++;
                                        ir.b.l++;
                                        switch (opcode)
                                        {
                                                case 0x06: temp=readmem(iy.w+offset); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; if (af.b.l&C_FLAG) temp|=1; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*RLC (IY+d)*/
                                                case 0x0E: temp=readmem(iy.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (af.b.l&C_FLAG) temp|=0x80; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*RRC (IY+d)*/
                                                case 0x16: temp=readmem(iy.w+offset); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; if (tempc) temp|=1; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*RL (IY+d)*/
                                                case 0x1E: temp=readmem(iy.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (tempc) temp|=0x80; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*RR (IY+d)*/
                                                case 0x26: temp=readmem(iy.w+offset); if (temp&0x80) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp<<=1; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*SLA (IY+d)*/
                                                case 0x2E: temp=readmem(iy.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; if (temp&0x40) temp|=0x80; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*SRA (IY+d)*/
                                                case 0x3E: temp=readmem(iy.w+offset); if (temp&1) af.b.l|=C_FLAG; else af.b.l&=~C_FLAG; temp>>=1; setznc(temp); writemem(iy.w+offset,temp); cycles-=28; break; /*SRL (IY+d)*/

                                                case 0x46: temp=readmem(iy.w+offset); setznc(temp&0x01); cycles-=24; break; /*BIT 0,(IY+d)*/
                                                case 0x4E: temp=readmem(iy.w+offset); setznc(temp&0x02); cycles-=24; break; /*BIT 1,(IY+d)*/
                                                case 0x56: temp=readmem(iy.w+offset); setznc(temp&0x04); cycles-=24; break; /*BIT 2,(IY+d)*/
                                                case 0x5E: temp=readmem(iy.w+offset); setznc(temp&0x08); cycles-=24; break; /*BIT 3,(IY+d)*/
                                                case 0x66: temp=readmem(iy.w+offset); setznc(temp&0x10); cycles-=24; break; /*BIT 4,(IY+d)*/
                                                case 0x6E: temp=readmem(iy.w+offset); setznc(temp&0x20); cycles-=24; break; /*BIT 5,(IY+d)*/
                                                case 0x76: temp=readmem(iy.w+offset); setznc(temp&0x40); cycles-=24; break; /*BIT 6,(IY+d)*/
                                                case 0x7E: temp=readmem(iy.w+offset); setznc(temp&0x80); cycles-=24; break; /*BIT 7,(IY+d)*/

                                                case 0x86: temp=readmem(iy.w+offset)&~0x01; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 0,(IY+d)*/
                                                case 0x8E: temp=readmem(iy.w+offset)&~0x02; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 1,(IY+d)*/
                                                case 0x96: temp=readmem(iy.w+offset)&~0x04; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 2,(IY+d)*/
                                                case 0x9E: temp=readmem(iy.w+offset)&~0x08; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 3,(IY+d)*/
                                                case 0xA6: temp=readmem(iy.w+offset)&~0x10; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 4,(IY+d)*/
                                                case 0xAE: temp=readmem(iy.w+offset)&~0x20; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 5,(IY+d)*/
                                                case 0xB6: temp=readmem(iy.w+offset)&~0x40; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 6,(IY+d)*/
                                                case 0xBE: temp=readmem(iy.w+offset)&~0x80; writemem(iy.w+offset,temp); cycles-=28; break; /*RES 7,(IY+d)*/

                                                case 0xC6: temp=readmem(iy.w+offset)|0x01; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 0,(IY+d)*/
                                                case 0xCE: temp=readmem(iy.w+offset)|0x02; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 1,(IY+d)*/
                                                case 0xD6: temp=readmem(iy.w+offset)|0x04; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 2,(IY+d)*/
                                                case 0xDE: temp=readmem(iy.w+offset)|0x08; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 3,(IY+d)*/
                                                case 0xE6: temp=readmem(iy.w+offset)|0x10; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 4,(IY+d)*/
                                                case 0xEE: temp=readmem(iy.w+offset)|0x20; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 5,(IY+d)*/
                                                case 0xF6: temp=readmem(iy.w+offset)|0x40; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 6,(IY+d)*/
                                                case 0xFE: temp=readmem(iy.w+offset)|0x80; writemem(iy.w+offset,temp); cycles-=28; break; /*SET 7,(IY+d)*/

                                                default:
                                                //printf("Bad FD CB opcode %02X at %04X\n",opcode,--pc);
                                                dumpregs();
                                                exit(-1);
                                        }
                                        break;
                                        case 0xE1: /*POP IY*/
                                        iy.b.l=pull();
                                        iy.b.h=pull();
                                        cycles-=20;
                                        break;

                                        case 0xE3: /*EX (SP),Iy*/
                                        tempw=ix.w;
                                        iy.b.l=readmem(sp);
                                        iy.b.h=readmem(sp+1);
                                        writemem(sp,tempw&0xFF);
                                        writemem(sp+1,tempw>>8);
                                        cycles-=28; // t 28
                                        break;

                                        case 0xE5: /*PUSH IY*/
                                        push(iy.b.h);
                                        push(iy.b.l);
                                        cycles-=20;
                                        break;
                                        case 0xE9: /*JP (IY)*/
                                        pc=iy.w;
                                        cycles-=8;
                                        break;

                                        case 0xF9: /*LD SP,IY*/
                                        sp=iy.w;
                                        cycles-=12;
                                        break;

                                        case 0xDD: /*Another prefix*/
                                        case 0xFD:
                                        ir.b.l--;
                                        cycles-=4;
                                        pc--;
                                        break;

                                        default:
                                        //printf("Bad FD opcode %02X at %04X\n",opcode,--pc);
                                        dumpregs();
                                        exit(-1);
                                }
                                break;

                                case 0xFE: /*CP xx*/
                                temp=readmem(pc); pc++;
                                setcp(af.b.h,temp);
                                cycles-=8;
                                break;
                                case 0xFF: /*RST 38*/
                                push(pc>>8);
                                push(pc);
                                pc=0x38;
                                cycles-=16;
                                break;

                                default:
                                //printf("Bad opcode %02X at %04X\n",opcode,--pc);
                                dumpregs();
                                exit(-1);
                        }
                        ir.b.l++;
                        if (intreq==1 && iff1==1)
                        {
                                cleargacount();
                                intreq=0;
                                iff2=iff1;
                                iff1=0;
                                push(pc>>8);
                                push(pc&0xFF);
                                switch (im)
                                {
                                        case 0:
                                        pc=0x38;
                                        cycles-=20;
                                        break;

                                        case 1:
                                        pc=0x38;
                                        cycles-=20;
                                        break;

                                        case 2:
                                        addr=(ir.b.h<<8)|0xFF; pc=readmem(addr)|(readmem(addr+1)<<8);
                                        cycles-=76;
                                        break;

                                        default:
                                        //printf("Bad IM %i\n",im);
                                        dumpregs();
                                        exit(-1);
                                }
                        }
                        if (intreq==-1) intreq=1;
                        if (iff1==3) iff1=iff2=1;
                        if (iff1==2) iff1=3;
                }
        }
}

