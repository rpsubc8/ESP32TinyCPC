#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //usar raton amx
 //#define use_lib_amx_mouse
 //zurdo
 //#define use_lib_amx_mouse_lefthanded

 //Video mode
 #define use_lib_400x300
 //#define use_lib_320x200_video_border
 //#define use_lib_320x200_video_noborder

 //Video colors
 //#define use_lib_vga8colors

 //Video ram consume
 //#define use_lib_vga_low_memory

 //Video CPU optimice speed
 //#define use_lib_ultrafast_vga
 
 //RAM mode
 //#define use_lib_128k
 //#define use_lib_cheat_128k 
 //#define use_lib_fix_psram_128k

 //Sound
 //#define use_lib_sound_ay8912

 //Bank switch optimice speed
 #define usb_lib_optimice_checkchange_bankswitch

 //Usar tabla precalculada 256 bytes flash znptable
 #define use_lib_lookup_znptable
 //Usar tabla precalculada 64 KB flash znptable16
 #define use_lib_lookup_znptable16

 //Logs
 //#define use_lib_log_serial
 
 

 //Read mouse poll millis
 #define gb_ms_mouse 10
 //Read keyboard poll x millis
 #define gb_ms_keyboard 20
 //Read sound poll x millis
 #define gb_ms_sound 1
 //Skip x frames
 #define gb_frame_crt_skip 0
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



 //fabgl dependence
 // - use_lib_sound_ay8912

 //use_lib_vga8colors - 3 pin vga
 //use_lib_use_bright - 6 pin vga
#endif
