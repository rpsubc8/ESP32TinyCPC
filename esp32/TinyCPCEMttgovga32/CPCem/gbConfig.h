#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //Corrige teclados que no se inicializan(solucion dcrespo3D)
 //#define FIX_PERIBOARD_NOT_INITING

 //usar raton amx
 //#define use_lib_amx_mouse
 //Espera para inicializar raton
 #define gb_delay_init_ms 1000
 //zurdo
 //#define use_lib_amx_mouse_lefthanded

 //Video mode
 #define use_lib_400x300
 //#define use_lib_320x200_video_border
 //#define use_lib_320x200_video_noborder

 //Video colors (8 colores es DAC 3 bits)
 //#define use_lib_vga8colors

 //Video ram consume (Ya no se usa)
 //#define use_lib_vga_low_memory

 //Video CPU optimice speed (Ya no se usa)
 //#define use_lib_ultrafast_vga

 //Usa la libreria bitluni modificada en C tiny fast
 #define use_lib_tinybitluni_fast
 
 //Ya no se usa, siempre es tiny bitluni
 //#ifdef use_lib_tinybitluni_fast
 // #ifndef use_lib_ultrafast_vga
 //  #define use_lib_ultrafast_vga
 // #endif 
 //#endif 
 
 //RAM mode
 //#define use_lib_128k
 //#define use_lib_cheat_128k 
 //#define use_lib_fix_psram_128k

 //Sound
 //#define use_lib_sound_ay8912

 //Measure times (si no se usa va mas rapido)
 //#define use_lib_measure_time

 //Bank switch optimice speed
 #define usb_lib_optimice_checkchange_bankswitch

 //Usar tabla precalculada 256 bytes flash znptable
 #define use_lib_lookup_znptable
 //Usar tabla precalculada 64 KB flash znptable16
 #define use_lib_lookup_znptable16

 //Logs (si no se usa, va mas rapido)
 #define use_lib_log_serial
 


 #ifdef use_lib_measure_time
  #ifndef use_lib_log_serial
   #define use_lib_log_serial 
  #endif 
 #endif
 

 //Read mouse poll millis
 #define gb_ms_mouse 10
 //Read keyboard poll x millis
 #define gb_ms_keyboard 20
 //Read sound poll x millis
 #define gb_ms_sound 1
 //Skip x frames
 #define gb_frame_crt_skip 0 
 //milisegundos espera en cada frame 
 #define use_lib_delay_tick_cpu_auto 1
 //Delay millis iteration emulate
 #define gb_delay_emulate_ms 0





 
 #ifdef use_lib_128k
  #define use_lib_mem_blocks
 #endif 


 
 #ifdef use_lib_vga8colors
  #define COLOR_3B
 #else 
  #define COLOR_6B
 #endif

 #ifdef use_lib_400x300
  #define gb_topeX 400
  #define gb_topeY 300
  #define gb_topeX_div4 100
 #else
  #define gb_topeX 320
  #define gb_topeY 200
  #define gb_topeX_div4 80
 #endif

 //fabgl dependence
 // - use_lib_sound_ay8912

 //use_lib_vga8colors - 3 pin vga
 //use_lib_use_bright - 6 pin vga
#endif
