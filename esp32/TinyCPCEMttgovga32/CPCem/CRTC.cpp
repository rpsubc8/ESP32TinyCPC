#include "gbConfig.h"
#include "string.h"
#include "CRTC.h"
#include "CPCem.h"
#include "gb_globals.h"

int crtcvsync;
int linessincevsync=0;
int galc=0;
//JJ int motoron=0; //no necesito estado motor disco
int vc;
int sc;
int crtcline;
int vsyncpulse;
//JJ int vols[3][512]; //no ncesito sonido
//JJ int pglatchs[5][512]; //no ncesito sonido
//unsigned char resolution=0;
int cpuline;
unsigned char crtctype=0;
unsigned short ma;
unsigned char crtcregs[32];
int crtcreg;
//JJ unsigned short crtctable[8][0x8000]; //precalculada
//static int frames=0,fps=0;
static unsigned char frames=0;



/*JJunsigned char cpcpalpc[][3]=
{
        {0,0,0},
        {31,31,31},
        {31,31,31},
        {0,63,31},
        {63,63,31},
        {0,0,31},
        {63,0,31},
        {0,31,31},
        {63,31,31},
        {63,0,31},
        {63,63,31},
        {63,63,0},
        {63,63,63},
        {63,0,0},
        {0,63,63},
        {63,31,0},
        {63,31,63},
        {0,0,31},
        {0,63,31},
        {0,63,0},
        {0,63,63},
        {0,0,0},
        {0,0,63},
        {0,31,0},
        {0,31,63},
        {31,0,31},
        {31,63,31},
        {31,63,0},
        {31,63,63},
        {31,0,0},
        {31,0,63},
        {31,31,0},
        {31,31,63},
        {47,47,47},
        {15,15,15}
};*/


//static void framecount()
//{
//        fps=frames;
//        frames=0;
//}


//void initvid()
//{
//}

//void switchres()
//{
//}

void writecrtc(unsigned short a, unsigned char v)
{
 switch (a&0xFF00)
 {
  case 0xBC00: crtcreg=v&31; return;
  case 0xBD00: crtcregs[crtcreg]=v; /*printf("R%i=%02X %i at vc %i %i\n",crtcreg,v,v,vc,linessincevsync); */return;
 }
}

unsigned char readcrtc(unsigned short a)
{
 switch (a&0xFF00)
 {
  case 0xBF00: return crtcregs[crtcreg];
 }
}

void resetcrtc()
{
 vc=sc=crtcline=vsyncpulse=0;
}

static unsigned long look2bpp[256];
//,look2bpph[256][2];

/*void ImprimeTablaPrecalculadaCRT()
{
 int cont=0;
 for (int j=0;j<8;j++)
 {
  cont=0;
  printf("\n\nTable crtctable(%d)\n",j); 
  for (int i=0;i<0x7FFF;i++)
  {
   printf("0x%04x,",crtctable[j][i]);
   cont++;
   if (cont>=16)
   {
    cont=0;
    printf("\n");
   }
  }
  printf("\n");
 } 
}
*/

/*JJ sin precalculada
void makelookup()
{
 int c,d;
 //Address table
 for (c=0;c<0x7FFF;c++)
 {
  for (d=0;d<8;d++)
  {
   crtctable[d][c]=(c&0x7FF)|(d<<11)|((c&0x6000)<<1);
  }
 }
// ImprimeTablaPrecalculadaCRT();
}
*/


void remakelookup()
{
        int c,d,e;
        //switch (scrmode|((resolution)?4:0))
        switch (scrmode)
        {
                case 0:
                for (c=0;c<256;c++)
                {
                        look2bpp[c]=0;
                        d=((c&1)<<3) | ((c&4)>>1) | ((c&0x10)>>2) | ((c&0x40)>>6);
                        look2bpp[c]|=gapal[d]<<16;
                        e=c>>1;
                        d=((e&1)<<3) | ((e&4)>>1) | ((e&0x10)>>2) | ((e&0x40)>>6);
                        look2bpp[c]|=gapal[d];
                        look2bpp[c]|=(look2bpp[c]<<8);
                }
                break;
                case 1:
                for (c=0;c<256;c++)
                {
                        look2bpp[c]=0;
                        d=((c&1)<<1)|((c&0x10)>>4);
                        look2bpp[c]|=gapal[d]<<24;
                        d=(((c&2)<<1)|((c&0x20)>>4))>>1;
                        look2bpp[c]|=gapal[d]<<16;
                        d=(((c&4)<<1)|((c&0x40)>>4))>>2;
                        look2bpp[c]|=gapal[d]<<8;
                        d=(((c&8)<<1)|((c&0x80)>>4))>>3;
                        look2bpp[c]|=gapal[d];
                }
                break;
                case 2:
                for (c=0;c<256;c++)
                {
                        look2bpp[c]=gapal[c&1]<<24;
                        look2bpp[c]|=gapal[(c>>2)&1]<<16;
                        look2bpp[c]|=gapal[(c>>4)&1]<<8;
                        look2bpp[c]|=gapal[(c>>6)&1];
                }
                break;
                case 4:
//JJ                for (c=0;c<256;c++)
//JJ                {
//JJ                        d=((c&1)<<3) | ((c&4)>>1) | ((c&0x10)>>2) | ((c&0x40)>>6);
//JJ                        look2bpph[c][1]=gapal[d]|(gapal[d]<<8);
//JJ                        look2bpph[c][1]|=(look2bpph[c][1]<<16);
//JJ                        e=c>>1;
//JJ                        d=((e&1)<<3) | ((e&4)>>1) | ((e&0x10)>>2) | ((e&0x40)>>6);
//JJ                        look2bpph[c][0]=gapal[d]|(gapal[d]<<8);
//JJ                        look2bpph[c][0]|=(look2bpph[c][0]<<16);
//JJ                }
                break;
                case 5:
//JJ                for (c=0;c<256;c++)
//JJ                {
//JJ                        look2bpph[c][0]=look2bpph[c][1]=0;
//JJ                        d=((c&1)<<1)|((c&0x10)>>4);
//JJ                        look2bpph[c][1]|=gapal[d]<<24;
//JJ                        look2bpph[c][1]|=gapal[d]<<16;
//JJ                        d=(((c&2)<<1)|((c&0x20)>>4))>>1;
//JJ                        look2bpph[c][1]|=gapal[d]<<8;
//JJ                        look2bpph[c][1]|=gapal[d];
//JJ                        d=(((c&4)<<1)|((c&0x40)>>4))>>2;
//JJ                        look2bpph[c][0]|=gapal[d]<<24;
//JJ                        look2bpph[c][0]|=gapal[d]<<16;
//JJ                        d=(((c&8)<<1)|((c&0x80)>>4))>>3;
//JJ                        look2bpph[c][0]|=gapal[d]<<8;
//JJ                        look2bpph[c][0]|=gapal[d];
                //JJ}
                break;
                case 6:
//JJ                for (c=0;c<256;c++)
//JJ                {
//JJ                        look2bpph[c][0]=gapal[(c>>7)&1];
//JJ                        look2bpph[c][0]|=gapal[(c>>6)&1]<<8;
//JJ                        look2bpph[c][0]|=gapal[(c>>5)&1]<<16;
//JJ                        look2bpph[c][0]|=gapal[(c>>4)&1]<<24;
//JJ                        look2bpph[c][1]=gapal[(c>>3)&1];
//JJ                        look2bpph[c][1]|=gapal[(c>>2)&1]<<8;
//JJ                        look2bpph[c][1]|=gapal[(c>>1)&1]<<16;
//JJ                        look2bpph[c][1]|=gapal[c&1]<<24;
//JJ                }
                break;
        }
}


#ifdef use_lib_vga8colors
//8 colores
static unsigned char gb_const_colorNormal35[35]={
 BLACK, BLUE, BLUE, GREEN, YELLOW, BLUE, MAGENTA, BLUE,
 RED, MAGENTA, YELLOW, YELLOW, WHITE, RED, CYAN, RED,
 MAGENTA, BLUE, GREEN, GREEN, CYAN, BLACK, BLUE, GREEN,
 BLUE, MAGENTA, GREEN, GREEN, CYAN, RED, MAGENTA, GREEN,
 MAGENTA, BLUE, BLACK
};
#else
//64 colores
static unsigned char gb_const_colorNormal35[35]={
  0,21,21,29,31,16,35,20,
  23,35,47,15,63,3,60,7,
  59,16,28,12,61,0,48,4,
  36,17,29,14,62,1,49,5,
  58,42,21
};
#endif

//inline void SDL_rectfill(int x1, int y1, int x2, int y2,int pixel)
//{
// for (int y=y1;y<y2;y++)
//  for (int x=x1;x<x2;x++)   
//   vga.dot(x,y,gb_const_colorNormal35[pixel]);
//}

//#define gbvgaMask8Colores 0x3F
//#define gbvgaBits8Colores 0x40

#ifdef use_lib_ultrafast_vga 
 //**************************************
 void PrepareColorsUltraFastVGA()
 {  
  //(color & RGBAXMask) | SBits;
  #ifdef use_lib_vga8colors
   #ifdef use_lib_vga_low_memory
   #else
    for (unsigned char i=0;i<35;i++)
    {
     gb_const_colorNormal35[i]= ((gb_const_colorNormal35[i] & vga.RGBAXMask)|vga.SBits);
    }    
   #endif
  #else  
   for (unsigned char i=0;i<35;i++)
   {
    gb_const_colorNormal35[i]= ((gb_const_colorNormal35[i] & vga.RGBAXMask)|vga.SBits);
   }    
  #endif 
 } 
#endif 

//**************************************
inline void SDLputpixel32(unsigned char x, unsigned short int y, unsigned int pixel)
{        
 #if (defined use_lib_400x300) || (defined use_lib_320x200_video_noborder)        
  int auxX = x<<2;//X*4
 #else
  int auxX = (x<<1)+x;//X*3
 #endif 
 #ifdef use_lib_ultrafast_vga
  #ifdef use_lib_vga_low_memory
   #if (defined use_lib_400x300) || (defined use_lib_320x200_video_noborder)
    #ifdef use_lib_vga8colors
     if(auxX & 1){//Impar primero
      ptrVGA[y][auxX >> 1] = (ptrVGA[y][auxX >> 1] & 0xf) | (gb_const_colorNormal35[(pixel & 0x000000FF)] << 4);      
      ptrVGA[y][(auxX+1) >> 1] = (ptrVGA[y][(auxX+1) >> 1] & 0xf0) | (gb_const_colorNormal35[((pixel>>8) & 0x000000FF)] & 0xf);
      ptrVGA[y][(auxX+2) >> 1] = (ptrVGA[y][(auxX+2) >> 1] & 0xf) | (gb_const_colorNormal35[((pixel>>16) & 0x000000FF)] << 4);
      ptrVGA[y][(auxX+3) >> 1] = (ptrVGA[y][(auxX+3) >> 1] & 0xf0) | (gb_const_colorNormal35[((pixel>>24) & 0x000000FF)] & 0xf);
     }
     else{//Par primero
      ptrVGA[y][auxX >> 1] = (ptrVGA[y][auxX >> 1] & 0xf0) | (gb_const_colorNormal35[(pixel & 0x000000FF)] & 0xf);      
      ptrVGA[y][(auxX+1) >> 1] = (ptrVGA[y][(auxX+1) >> 1] & 0xf) | (gb_const_colorNormal35[((pixel>>8) & 0x000000FF)] << 4);
      ptrVGA[y][(auxX+2) >> 1] = (ptrVGA[y][(auxX+2) >> 1] & 0xf0) | (gb_const_colorNormal35[((pixel>>16) & 0x000000FF)] & 0xf);
      ptrVGA[y][(auxX+3) >> 1] = (ptrVGA[y][(auxX+3) >> 1] & 0xf) | (gb_const_colorNormal35[((pixel>>24) & 0x000000FF)] << 4);
     }
     //ptrVGA[y][auxX] = gb_const_colorNormal35[(pixel & 0x000000FF)];
     //ptrVGA[y][(auxX+1)] = gb_const_colorNormal35[((pixel>>8) & 0x000000FF)];
     //ptrVGA[y][(auxX+2)] = gb_const_colorNormal35[((pixel>>16) & 0x000000FF)];
     //ptrVGA[y][(auxX+3)] = gb_const_colorNormal35[((pixel>>24) & 0x000000FF)];
    #else     
     ptrVGA[y][auxX] = gb_const_colorNormal35[(pixel & 0x000000FF)];
     ptrVGA[y][(auxX+1)] = gb_const_colorNormal35[((pixel>>8) & 0x000000FF)];
     ptrVGA[y][(auxX+2)] = gb_const_colorNormal35[((pixel>>16) & 0x000000FF)];
     ptrVGA[y][(auxX+3)] = gb_const_colorNormal35[((pixel>>24) & 0x000000FF)];
    #endif
   #else
    //Modo 320x200 con borde    
    #ifdef use_lib_vga8colors
     //320x200 borde 8 colores low fastvga
     if(auxX & 1){//Impar primero
      ptrVGA[y][auxX >> 1] = (ptrVGA[y][auxX >> 1] & 0xf) | (gb_const_colorNormal35[(pixel & 0x000000FF)] << 4);      
      ptrVGA[y][(auxX+1) >> 1] = (ptrVGA[y][(auxX+1) >> 1] & 0xf0) | (gb_const_colorNormal35[((pixel>>8) & 0x000000FF)] & 0xf);
      ptrVGA[y][(auxX+2) >> 1] = (ptrVGA[y][(auxX+2) >> 1] & 0xf) | (gb_const_colorNormal35[((pixel>>16) & 0x000000FF)] << 4);
     }
     else{//Par primero
      ptrVGA[y][auxX >> 1] = (ptrVGA[y][auxX >> 1] & 0xf0) | (gb_const_colorNormal35[(pixel & 0x000000FF)] & 0xf);      
      ptrVGA[y][(auxX+1) >> 1] = (ptrVGA[y][(auxX+1) >> 1] & 0xf) | (gb_const_colorNormal35[((pixel>>8) & 0x000000FF)] << 4);
      ptrVGA[y][(auxX+2) >> 1] = (ptrVGA[y][(auxX+2) >> 1] & 0xf0) | (gb_const_colorNormal35[((pixel>>16) & 0x000000FF)] & 0xf);
     }     
    #else
     //320x200 borde 64 colores low fastvga
     ptrVGA[y][auxX] = gb_const_colorNormal35[(pixel & 0x000000FF)];
     ptrVGA[y][(auxX+1)] = gb_const_colorNormal35[((pixel>>8) & 0x000000FF)];
     ptrVGA[y][(auxX+2)] = gb_const_colorNormal35[((pixel>>16) & 0x000000FF)];
    #endif
   #endif
  #else
   #if (defined use_lib_400x300) || (defined use_lib_320x200_video_noborder)
    ptrVGA[y][auxX^2] = gb_const_colorNormal35[(pixel & 0x000000FF)];
    ptrVGA[y][(auxX+1)^2] = gb_const_colorNormal35[((pixel>>8) & 0x000000FF)];
    ptrVGA[y][(auxX+2)^2] = gb_const_colorNormal35[((pixel>>16) & 0x000000FF)];
    ptrVGA[y][(auxX+3)^2] = gb_const_colorNormal35[((pixel>>24) & 0x000000FF)];
   #else
    //Video 320x200 borde relacion aspecto fastvga doble buffer
    ptrVGA[y][auxX^2] = gb_const_colorNormal35[(pixel & 0x000000FF)];
    ptrVGA[y][(auxX+1)^2] = gb_const_colorNormal35[((pixel>>8) & 0x000000FF)];
    ptrVGA[y][(auxX+2)^2] = gb_const_colorNormal35[((pixel>>16) & 0x000000FF)];
   #endif 
  #endif
 #else
  #if (defined use_lib_400x300) || (defined use_lib_320x200_video_noborder)
   vga.dotFast(auxX,y,gb_const_colorNormal35[(pixel & 0x000000FF)]);
   vga.dotFast(auxX+1,y,gb_const_colorNormal35[((pixel>>8) & 0x000000FF)]);
   vga.dotFast(auxX+2,y,gb_const_colorNormal35[((pixel>>16) & 0x000000FF)]);
   vga.dotFast(auxX+3,y,gb_const_colorNormal35[((pixel>>24) & 0x000000FF)]);
  #else
   //Video 320x200 con borde relacion aspecto normal doble buffer
   vga.dotFast(auxX,y,gb_const_colorNormal35[(pixel & 0x000000FF)]);
   vga.dotFast(auxX+1,y,gb_const_colorNormal35[((pixel>>8) & 0x000000FF)]);
   vga.dotFast(auxX+2,y,gb_const_colorNormal35[((pixel>>16) & 0x000000FF)]);
  #endif 
 #endif 
}

//*****************************************************************
inline void SDL_hline(int x1,int y1,int x2,unsigned char pixel)
{              
 #ifdef use_lib_ultrafast_vga        
  #ifdef use_lib_vga_low_memory
   #ifdef use_lib_vga8colors
    for (int x=x1;x<x2;x++)
    {
     if(x & 1)
      ptrVGA[y1][x >> 1] =  (ptrVGA[y1][x >> 1] & 0xf) | (gb_const_colorNormal35[pixel] << 4);
     else
      ptrVGA[y1][x >> 1] = (ptrVGA[y1][x >> 1] & 0xf0) | (gb_const_colorNormal35[pixel] & 0xf);
     //ptrVGA[y1][x] = gb_const_colorNormal35[pixel];
    }
   #else
    for (int x=x1;x<x2;x++)
    {
     ptrVGA[y1][x] = gb_const_colorNormal35[pixel];
    }
   #endif
  #else
   for (int x=x1;x<x2;x++)
    ptrVGA[y1][x^2] = gb_const_colorNormal35[pixel];
  #endif  
 #else
  for (int x=x1;x<x2;x++)
   vga.dotFast(x, y1, gb_const_colorNormal35[pixel]); 
 #endif
}

//******************************
void pollline()
{
 int aux_d;
 int aux_c;
 int aux_crtctable;

 int x,xoff;
 unsigned char v;
 unsigned char force_draw=1; //Dibuja o no dibuja
 //JJ vols[0][cpuline&511]=psgregs[8];//no ncesito sonido
 //JJ vols[1][cpuline&511]=psgregs[9];//no ncesito sonido
 //JJ vols[2][cpuline&511]=psgregs[10];//no ncesito sonido
 //JJ pglatchs[0][cpuline&511]=((psgregs[0]|((psgregs[1]<<8)&0xF00))>>1)+1;//no ncesito sonido
 //JJ pglatchs[1][cpuline&511]=((psgregs[2]|((psgregs[3]<<8)&0xF00))>>1)+1;//no ncesito sonido
 //JJ pglatchs[2][cpuline&511]=((psgregs[4]|((psgregs[5]<<8)&0xF00))>>1)+1;//no ncesito sonido
 //JJ pglatchs[3][cpuline&511]=(psgregs[6]>>3)+1;//no ncesito sonido
 //JJ pglatchs[4][cpuline&511]=((psgregs[11]|((psgregs[12]<<8)&0xFF00))<<18)+1;//no ncesito sonido
 if (gb_frame_crt_skip >= 1)
 {
  force_draw = ((frames & 1) == 1);
 }

 linessincevsync++;
 if (crtcline<(crtcregs[5]&31))
 {
  crtcline++;
  galines++;
  if (vsyncpulse)
  {
   vsyncpulse--;
   if (!vsyncpulse) crtcvsync=0;
  }
  if (galc)
  {
   galc--;
   if (!galc)
   {
    if (galines&32) intreq=1;
    galines=0;
   }
  }
  return;
 }                                 
 if (vc<crtcregs[6])
 {
  if (palchange)
  {
   remakelookup();
   palchange=0;
  }
    
  #if (defined use_lib_400x300) || (defined use_lib_320x200_video_border)
   xoff=50-crtcregs[1] - gb_screen_xOffset;
  #else
   xoff=0;
  #endif

  if (force_draw == 1)
  {
   //JJ hline(b,0,(crtcline-16)&511,xoff<<2,gaborder);   
   #ifdef use_lib_400x300
    if ((crtcline-16)>=0)
    {
     SDL_hline(0,(crtcline-16)&511,xoff<<2,gaborder); //cuadrado izquierdo
    }
   #else 
    #ifdef use_lib_320x200_video_border
     if (crtcline >=72 && crtcline <270){
      SDL_hline(0,(crtcline-72)&511,((xoff<<1)+xoff),gaborder); //cuadrado izquierdo     
     }       
    #endif    
   #endif
   aux_d= ((sc&7)<<11);
   for (x=0;x<crtcregs[1]<<1;x++)
   {
    //precalculada crtctable[d][c]=(c&0x7FF)|(d<<11)|((c&0x6000)<<1);
    //JJ v=ram[crtctable[sc&7][(ma+x)&0x7fff]]; //usa tabla precalculada   
    //aux_d= (sc&7);
    //aux_c= (ma+x)&0x7fff;
    //aux_crtctable= (aux_c&0x7FF)|(aux_d<<11)|((aux_c&0x6000)<<1);
    //v=ram[aux_crtctable]; //sin precalculada   
       
    aux_c= (ma+x)&0x7fff;
    aux_crtctable= (aux_c&0x7FF)|aux_d|((aux_c&0x6000)<<1);
    v=ram[aux_crtctable]; //sin precalculada

    #ifdef use_lib_400x300
     //JJ((unsigned long *)b->line[(crtcline-16)&511])[(x+xoff)&127]=look2bpp[v];
     if ((crtcline-16)>=0)
     {
      SDLputpixel32((x+xoff)&127, (crtcline-16)&511, look2bpp[v]);   
     }
    #else
     #ifdef use_lib_320x200_video_noborder
      if (crtcline >=72 && crtcline <270)
      {
       if (x>=0 && x<80)
        SDLputpixel32(x, (crtcline-72), look2bpp[v]);                
      }
     #else
      if (crtcline >=72 && crtcline <270)
      {//320x200 no border
       if (x>=0 && x<96)            
        SDLputpixel32(x+xoff, (crtcline-72)&511, look2bpp[v]);
      }     
     #endif 
    #endif
   }//fin del for
   //JJ hline(b,(x+xoff)<<2,(crtcline-16)&511,400,gaborder);
   #ifdef use_lib_400x300
    if (crtcline>=16 and crtcline<300)
    {
     SDL_hline((x+xoff)<<2,(crtcline-16)&511,400,gaborder); //cuadrado derecho
    }
   #else 
    #ifdef use_lib_320x200_video_border
     if (crtcline >=72 && crtcline <270){
      SDL_hline((((x+xoff)<<1)+(x+xoff)),(crtcline-72)&511,319,gaborder); //cuadrado derecho 320x200
     }
    #endif    
   #endif 
  }//fin del force_draw
 
  if (sc==crtcregs[9])
  {
   ma+=crtcregs[1]<<1;
  }
 }
 else
 {
  //JJ hline(b,0,(crtcline-16)&511,399,gaborder);
  if (force_draw == 1)
  {
   #ifdef use_lib_400x300
    if ((crtcline-16)>=0) //comprobacion limite
    {
     SDL_hline(0,(crtcline-16)&511,399,gaborder); //Arriba y abajo
    }
   #endif 
  }
 }
 if (sc==crtcregs[9])
 {
  sc=0;
  vc++;
  vc&=127;
 }
 else
 {
  if (!(crtcline<(crtcregs[5]&31)))
  {
   sc++;
   sc&=31;
  }
 }
 crtcline++;
 galines++;
 if (galines==52)
 {
  galines=0;
  intreq=1;
 }
 if (vsyncpulse)
 {
  vsyncpulse--;
  if (!vsyncpulse) crtcvsync=0;
 }
 if (galc)
 {
  galc--;
  if (!galc)
  {
   if (galines&32) intreq=1;
   galines=0;
  }
 }
 if (vc==crtcregs[4]+1)
 {
  sc=vc=0;
  ma=(crtcregs[13]|((crtcregs[12]&0x3F)<<8))<<1;
 }
 if (vc==crtcregs[7] && !sc)
 {
  frames++;
  //JJblit(b,screen,0,0,0,0,400,300-4);
  gb_sdl_blit=1; //solo necesito en pc
  
  crtcline=0;
  vsyncpulse=crtcregs[3]>>4;
  if (crtctype==0)
  {
   vsyncpulse=crtcregs[3]>>4;
  }
  else
  {
   vsyncpulse=16;
  }
  vsyncpulse++;
  galc=2;
  crtcvsync=1;
  linessincevsync=0;
 }
 if (crtcline==313) //Force vsync
 {
  crtcline=0;  
  //JJ blit(b,screen,0,0,0,0,400,300-4);
  gb_sdl_blit= 1;  //solo necesito en pc
 }

}





//JJ void dumpscr()
//JJ{
//JJ}

//JJ void transfersound()//No necesito sonido
//{
// memcpy(psgvols,vols,sizeof(vols));//No necesito sonido
// memcpy(psglatchs,pglatchs,sizeof(pglatchs));//No necesito sonido
//}
