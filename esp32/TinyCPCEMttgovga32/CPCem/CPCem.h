//#pragma once
#ifndef _CPCEM_H
 #define _CPCEM_H

 #include "def/hardware.h"
 #include "MartianVGA.h"

 // Declared vars
 #ifdef COLOR_3B
  #ifdef use_lib_vga_low_memory
   extern VGA3BitI vga;
  #else
   extern VGA3Bit vga;
  #endif
 #else   
  #ifdef use_lib_vga_low_memory
   extern VGA6BitI vga;
  #else
   extern VGA6Bit vga;
  #endif 
 #endif


 // Declared methods
 void do_keyboard();
#endif
