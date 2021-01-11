#ifndef _Z80_H
 #define _Z80_H
 
 #include "gbConfig.h"
 
 #define N_FLAG 0x80
 #define Z_FLAG 0x40
 #define H_FLAG 0x10
 #define P_FLAG 0x04
 #define V_FLAG 0x04
 #define S_FLAG 0x02
 #define C_FLAG 0x01
 
 void initz80();
 void resetz80();
 void loadroms();
 void execz80();
 void rebuildmem(void);
 void loadroms2FlashModel(void);
 #ifndef use_lib_lookup_znptable
  inline unsigned char Get_no_lookup_znptable(unsigned char c);
 #endif
 #ifndef use_lib_lookup_znptable16
  inline unsigned char Get_no_lookup_znptable16(unsigned short int c);
 #endif
  
#endif
 
