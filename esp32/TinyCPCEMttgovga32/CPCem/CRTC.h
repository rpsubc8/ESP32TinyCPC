#ifndef _CRTC_H
 #define _CRTC_H

 #include "gbConfig.h"
 //CRTC
 //void initvid(void);
 //JJ void makelookup(void); //sin precalculada
 void resetcrtc(void);
 void remakelookup(void);
 //void switchres(void);
 void pollline(void); 
 void writecrtc(unsigned short a, unsigned char v);
 unsigned char readcrtc(unsigned short a);
 #ifdef use_lib_ultrafast_vga  
  void PrepareColorsUltraFastVGA(void);  
 #endif 
 
#endif
