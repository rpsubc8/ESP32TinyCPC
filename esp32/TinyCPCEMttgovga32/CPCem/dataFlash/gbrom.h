#ifndef _GBROM_H
 #define _GBROM_H
 
 #include "../gbConfig.h"
 #include "rom/romAmsdos.h"
 #include "rom/rom464.h"
 //#include "rom/rom664.h"
 #if defined  use_lib_128k || defined use_lib_cheat_128k
  #include "rom/rom6128.h"
 #endif
   
 #define max_list_rom 1
 
 static const char * gb_list_rom_title[max_list_rom]={
  "AMSDOS"
 };
  
 static const unsigned char * gb_list_rom_data[max_list_rom]={
  gb_rom_amsdos
 };
 
#endif
 
