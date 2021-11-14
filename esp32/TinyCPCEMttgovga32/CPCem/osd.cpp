#include "gbConfig.h"
#include "CPCem.h"
#include "osd.h"
#include "CRTC.h"
#include "FDC.h"
#include "Z80.h"
#include "dataFlash/gbdsk.h"
#include "gbGlobals.h"
#include "PS2Kbd.h"
#include "gb_sdl_font8x8.h"


//#define BLACK   0
//#define BLUE    4
//#define RED     1
//#define MAGENTA 5
//#define GREEN   2
//#define CYAN    6
//#define YELLOW  3
//#define WHITE   15

#ifdef COLOR_3B           //       BGR 
 #define BLACK   0x08      // 0000 1000
 #define BLUE    0x0C      // 0000 1100
 #define RED     0x09      // 0000 1001
 #define MAGENTA 0x0D      // 0000 1101
 #define GREEN   0x0A      // 0000 1010
 #define CYAN    0x0E      // 0000 1110
 #define YELLOW  0x0B      // 0000 1011
 #define WHITE   0x0F      // 0000 1111
#endif


#ifdef use_lib_vga8colors
 #define ID_COLOR_BLACK 0 //Negro
 #define ID_COLOR_WHITE 12 //Blanco
 #define ID_COLOR_MAGENTA 6 //Magenta
#else
 #define ID_COLOR_BLACK 0 //Negro
 #define ID_COLOR_WHITE 12 //Blanco
 #define ID_COLOR_MAGENTA 6 //Magenta
#endif 

//extern int gb_screen_xIni;
//extern int gb_screen_yIni;
//extern unsigned char gb_cache_zxcolor[8];


unsigned char gb_show_osd_main_menu=0;

//extern SDL_Surface * gb_screen;
//extern SDL_Event gb_event;





#define max_gb_osd_screen 1
const char * gb_osd_screen[max_gb_osd_screen]={
 "Pixels Left"//,
 //"Pixels Top",
 //"Color 8",
 //"Mono Blue 8",
 //"Mono Green 8",
 //"Mono Red 8",
 //"Mono Grey 8"
};

#define max_gb_osd_screen_values 5
const char * gb_osd_screen_values[max_gb_osd_screen_values]={
 "0",
 "2",
 "4", 
 "8", 
 "16"
};


#define max_gb_main_menu 8
const char * gb_main_menu[max_gb_main_menu]={
 "Machine",
 "Load DSK",
 "Speed",
 "Screen Adjust",
 "Mouse",
 "Sound",
 "Reset",
 "Return"
};

#define max_gb_machine_menu 3
const char * gb_machine_menu[max_gb_machine_menu]={
 "464",
 "664",
 "6128"
};

#define max_gb_osd_sound_menu 2
const char * gb_osd_sound_menu[max_gb_osd_sound_menu]={
 "Sound Enabled",
 "Sound Volume"
};

#define max_gb_speed_sound_menu 52
const char * gb_speed_sound_menu[max_gb_speed_sound_menu]={
 "0 (FAST)","1","2","3","4","5","6","7",
 "8","9","10","11","12","13","14","15",
 "16","17","18","19","20","21","22","23",
 "24","25","26","27","28","29","30","31",
 "32","33","34","35","36","37","38","39",
 "40","41","42","43","44","45","46","47",
 "48","49","50","AUTO"
};

#define max_osd_sound_vol_values 5
const char * gb_osd_sound_vol_values[max_osd_sound_vol_values]={
 "100%",
 "75%",
 "50%",
 "25%",
 "5%"
};


#define max_gb_value_binary_menu 2
const char * gb_value_binary_menu[max_gb_value_binary_menu]={
 "0 (OFF)",
 "1 (ON)"
};


#define max_gb_speed_videoaudio_options_menu 5
const char * gb_speed_videoaudio_options_menu[max_gb_speed_videoaudio_options_menu]={
 "Audio poll",
 "CPU delay",
 "Skip Frame",
 "Keyboard poll",
 "Mouse poll"
};


//#define max_gb_speed_menu 5
//const char * gb_speed_menu[max_gb_speed_menu]={
// "Normal",
// "2x",
// "4x",
// "8x",
// "16x"
//};


#define max_gb_osd_mouse_menu 3
const char * gb_osd_mouse_menu[max_gb_osd_mouse_menu]={
 "Mouse Detect",
 "Mouse Buttons",
 "Mouse Enabled"
};

#define max_gb_osd_mouse_buttons_menu 2
const char * gb_osd_mouse_buttons_menu[max_gb_osd_mouse_buttons_menu]={
 "right handed",
 "left handed"
};

#define max_gb_reset_menu 2
const char * gb_reset_menu[max_gb_reset_menu]={
 "Soft",
 "Hard"
};


#ifdef use_lib_400x300
 #define gb_pos_x_menu 120
 #define gb_pos_y_menu 50 
#else
 #define gb_pos_x_menu 90
 #define gb_pos_y_menu 50
#endif

#define gb_osd_max_rows 10


//*************************************************************************************
void SDLprintCharOSD(char car,int x,int y,unsigned char color,unsigned char backcolor)
{ 
// unsigned char aux = gb_sdl_font_6x8[(car-64)];
 int auxId = car << 3; //*8
 unsigned char aux;
 unsigned char auxBit,auxColor;
 for (unsigned char j=0;j<8;j++)
 {
  aux = gb_sdl_font_8x8[auxId + j];
  for (int i=0;i<8;i++)
  {
   auxColor= ((aux>>i) & 0x01);
   //SDLputpixel(surface,x+(6-i),y+j,(auxColor==1)?color:backcolor);
   jj_fast_putpixel(x+(6-i),y+j,(auxColor==1)?color:backcolor);
  }
 }
}

void SDLprintText(const char *cad,int x, int y, unsigned char color,unsigned char backcolor)
{
//SDL_Surface *surface,
// gb_sdl_font_6x8
 int auxLen= strlen(cad);
 if (auxLen>50)
  auxLen=50;
 for (int i=0;i<auxLen;i++)
 {
  SDLprintCharOSD(cad[i],x,y,color,backcolor);
  x+=7;
 }
}


void OSDMenuRowsDisplayScroll(const char **ptrValue,unsigned char currentId,unsigned char aMax)
{//Dibuja varias lineas
 for (int i=0;i<gb_osd_max_rows;i++)
  SDLprintText("                    ",gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),0,0);
 
 for (int i=0;i<gb_osd_max_rows;i++)
 {
  if (currentId >= aMax)
   break;  
  SDLprintText(ptrValue[currentId],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_MAGENTA:ID_COLOR_BLACK));
  currentId++;
 }     

 
 //vga.setTextColor(WHITE,BLACK);
 //for (int i=0;i<gb_osd_max_rows;i++)
 //{
 // vga.setCursor(gb_pos_x_menu, gb_pos_y_menu+8+(i<<3));
 // vga.print("                    ");
 //}
 //
 //for (int i=0;i<gb_osd_max_rows;i++)
 //{
 // if (currentId >= aMax)
 //  break;
 // if (i == 0)
 //  vga.setTextColor(CYAN,BLUE);
 // else
 //  vga.setTextColor(WHITE,BLACK);
 // vga.setCursor(gb_pos_x_menu, gb_pos_y_menu+8+(i<<3));
 // vga.print(ptrValue[currentId]);
 // currentId++;
 //}     
}

//Maximo 256 elementos
unsigned char ShowTinyMenu(const char *cadTitle,const char **ptrValue,unsigned char aMax)
{
 unsigned char aReturn=0;
 unsigned char salir=0;
 SDLClear();
 SDLprintText("Port Cpcem(Tom Walker) by Ackerman",(gb_pos_x_menu-(32)),(gb_pos_y_menu-16),ID_COLOR_WHITE,ID_COLOR_BLACK);
 //for (int i=0;i<20;i++) 
 for (int i=0;i<14;i++) 
  SDLprintCharOSD(' ',gb_pos_x_menu+(i<<3),gb_pos_y_menu,ID_COLOR_BLACK,ID_COLOR_WHITE);
 SDLprintText(cadTitle,gb_pos_x_menu,gb_pos_y_menu,ID_COLOR_BLACK,ID_COLOR_WHITE);

 OSDMenuRowsDisplayScroll(ptrValue,0,aMax);
 
 while (salir == 0)
 {             
  //case SDLK_UP:
  if (checkAndCleanKey(KEY_CURSOR_LEFT))
  {
   if (aReturn>10) aReturn-=10;
   OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);       
  }
  if (checkAndCleanKey(KEY_CURSOR_RIGHT))
  {
   if (aReturn<(aMax-10)) aReturn+=10;
   OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);       
  }  
  if (checkAndCleanKey(KEY_CURSOR_UP))
  {
   if (aReturn>0) aReturn--;
   OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
  }
  if (checkAndCleanKey(KEY_CURSOR_DOWN))
  {
   if (aReturn < (aMax-1)) aReturn++;
   OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
  }
  if (checkAndCleanKey(KEY_ENTER))
  {
   salir= 1;
  }
  //case SDLK_KP_ENTER: case SDLK_RETURN: salir= 1;break;
  if (checkAndCleanKey(KEY_ESC))
  {
   salir=1; aReturn= 255;    
  }
  //case SDLK_ESCAPE: salir=1; aReturn= 255; break;
  //default: break;    
 }
 gb_show_osd_main_menu= 0;
 return aReturn;



/*
 unsigned char aReturn=0;
 unsigned char salir=0;
 #ifdef use_lib_400x300
  vga.fillRect(0,0,400,300,BLACK);
  vga.fillRect(0,0,400,300,BLACK);//Repeat Fix visual defect
 #else
  vga.fillRect(0,0,320,200,BLACK);
  vga.fillRect(0,0,320,200,BLACK);//Repeat Fix visual defect    
 #endif
 vga.setTextColor(WHITE,BLACK);
 vga.setCursor((gb_pos_x_menu-(32)), gb_pos_y_menu-16);
 vga.print("Port Cpcem(Tom Walker) by Ackerman");

 vga.setTextColor(BLACK,WHITE); 
 for (int i=0;i<12;i++)
 {  
  vga.setCursor((gb_pos_x_menu+(i*6)), gb_pos_y_menu);
  vga.print(" ");
 }
  
 vga.setCursor(gb_pos_x_menu,gb_pos_y_menu);
 vga.print(cadTitle);  
 
 //for (int i=0;i<aMax;i++)
 //{     
 // //SDLprintText(gb_osd_sdl_surface,ptrValue[i],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?CYAN:WHITE),((i==0)?BLUE:BLACK),1);  
 // vga.setTextColor(((i==0)?CYAN:WHITE),((i==0)?BLUE:BLACK));
 // vga.setCursor(gb_pos_x_menu,(gb_pos_y_menu+8+(i<<3)));
 // vga.print(ptrValue[i]);
 //}
 
 OSDMenuRowsDisplayScroll(ptrValue,0,aMax);
 
 while (salir == 0)
 {
  //SDL_PollEvent(gb_osd_sdl_event);
  //if(SDL_WaitEvent(gb_osd_sdl_event))
  {
   //if(gb_osd_sdl_event->type == SDL_KEYDOWN)
   {
    //switch(gb_osd_sdl_event->key.keysym.sym)
    {
     //case SDLK_UP:
     if (checkAndCleanKey(KEY_CURSOR_UP))
     {
      //vga.setTextColor(WHITE,BLACK);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      ////SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),WHITE,BLACK,1);
      if (aReturn>0) aReturn--;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
      //vga.setTextColor(CYAN,BLUE);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      ////SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),CYAN,BLUE,1);
      ////break;
     }
     if (checkAndCleanKey(KEY_CURSOR_DOWN))
     {
     //case SDLK_DOWN: 
      //SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),WHITE,BLACK,1);
      //vga.setTextColor(WHITE,BLACK);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      if (aReturn < (aMax-1)) aReturn++;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
      //vga.setTextColor(CYAN,BLUE);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      ////SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),CYAN,BLUE,1);
      ////break;
     }
     if (checkAndCleanKey(KEY_ENTER))
     {
      salir= 1;
     }
     //case SDLK_KP_ENTER: case SDLK_RETURN: salir= 1;break;
     if (checkAndCleanKey(KEY_ESC))
     {
      salir=1; aReturn= 255;    
     }
     //case SDLK_ESCAPE: salir=1; aReturn= 255; break;
     //default: break;
    }
    //SDL_Flip(gb_osd_sdl_surface);
    //SDL_PollEvent(gb_osd_sdl_event);
   }  
  }  
 } 
 gb_show_osd_main_menu= 0;
 return aReturn;
 */
}

//Menu SNA
void ShowTinyDSKMenu()
{
 unsigned char aSelNum;     
 aSelNum = ShowTinyMenu("Machine DSK",gb_machine_menu,max_gb_machine_menu);
 if (aSelNum != 255)
 {
  if (aSelNum == 0)
  {
   aSelNum = ShowTinyMenu("464 DSK",gb_list_dsk_title,max_list_dsk);  
   loaddsk2Flash(aSelNum);
  }
  else
  {
   aSelNum = ShowTinyMenu("6128 DSK",gb_list_dsk_title,max_list_dsk);                
   loaddsk2Flash(aSelNum);  
  }
 }         
}

//Menu ROM
void ShowTinyMachineMenu()
{
 //unsigned char aSelNum;
 model = ShowTinyMenu("Machine",gb_machine_menu,max_gb_machine_menu);
 if (model != 255)
 {
  loadroms2FlashModel();
  //switch (aSelNum)
  //{
  // case 0: model= 0; loadroms2FlashModel(0); break;
  // case 1: model= 1; loadroms2FlashModel(1); break;
  // case 2: model= 2; loadroms2FlashModel(2); break;
  // default: model= 0; loadroms2FlashModel(0); break;
  //}
  //vTaskDelay(2);
  resetz80();
  resetcrtc();
  //SDL_Delay(2);
 }
}


//Menu resetear
void ShowTinyResetMenu()
{
 unsigned char aSelNum;
 aSelNum= ShowTinyMenu("Reset",gb_reset_menu,max_gb_reset_menu);
 if (aSelNum != 255)
 {
  loadroms2FlashModel();
  resetz80();
  resetcrtc(); 
  if (aSelNum == 1)
   ESP.restart();
 }
}

//Menu TAPE
/*void ShowTinyTAPEMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K TAPE",gb_list_tapes_48k_title,max_list_tape_48);
 load_tape2Flash(aSelNum);
}

//Play sound tape
void ShowTinySelectTAPEMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K TAPE",gb_list_tapes_48k_title,max_list_tape_48);
 Z80EmuSelectTape(aSelNum);
// gb_play_tape_current = aSelNum;
// gb_play_tape_on_now = 1;
}

//Menu SCREEN
void ShowTinySCRMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K SCREEN",gb_list_scr_48k_title,max_list_scr_48);
 load_scr2Flash(aSelNum);     
}
*/

//Menu velocidad emulador
void ShowTinySpeedMenu()
{
 unsigned char aSelNum,aSelNumSpeed;
 aSelNum = ShowTinyMenu("SPEED CPU AUDIO",gb_speed_videoaudio_options_menu,max_gb_speed_videoaudio_options_menu);
 if (aSelNum == 255)
  return;
 switch (aSelNum)
 {
  case 0: aSelNumSpeed= ShowTinyMenu("AUDIO Poll ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
   gb_current_ms_poll_sound= aSelNumSpeed;
   break;
  case 1: aSelNumSpeed= ShowTinyMenu("CPU DELAY ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
   if (aSelNumSpeed == 51){
    gb_auto_delay_cpu= 1;  
    gb_current_delay_emulate_ms = 1; //Revisar condicion execute main cpu
   }
   else{
    gb_auto_delay_cpu= 0;
    gb_current_delay_emulate_ms = aSelNumSpeed;
   }
   break;
  case 2: 
   aSelNumSpeed = ShowTinyMenu("Skip Frame",gb_value_binary_menu,max_gb_value_binary_menu);
   switch (aSelNumSpeed)
   {
    case 0: gb_current_frame_crt_skip = 0; break;
    case 1: gb_current_frame_crt_skip = 1; break;
    default: break;
   }
   break;
  case 3: aSelNumSpeed= ShowTinyMenu("Keyboard Poll ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
   gb_current_ms_poll_keyboard= aSelNumSpeed;
   break;
  case 4: aSelNumSpeed= ShowTinyMenu("Mouse Poll ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;   
   gb_current_ms_poll_mouse= aSelNumSpeed;
   break;      
  default: break;
 }
}

//Activar desactivar sonido
void ShowTinySoundMenu()
{
 unsigned char aSelNum,aSelSoundON,aSelVol;
 aSelNum = ShowTinyMenu("SOUND MENU",gb_osd_sound_menu,max_gb_osd_sound_menu); 
 switch (aSelNum)
 {
  case 0:
   aSelSoundON = ShowTinyMenu("SOUND ENABLED",gb_value_binary_menu,max_gb_value_binary_menu); 
   #ifdef use_lib_sound_ay8912
    switch (aSelSoundON)
    {
     case 0: gb_mute_sound=1; break;
     case 1: gb_mute_sound=0; break;
     default: break;
    }
   #endif
   break;
  case 1:
   aSelVol = ShowTinyMenu("SOUND VOLUME",gb_osd_sound_vol_values,max_osd_sound_vol_values);
   #ifdef use_lib_sound_ay8912
    gbShiftLeftVolumen= aSelVol;
   #endif 
   break;
  default: break;
 }

}




//Ajustar pantalla
void ShowTinyScreenAdjustMenu()
{
 unsigned char aSelNum, auxCol; 
 aSelNum= ShowTinyMenu("Screen Adjust",gb_osd_screen,max_gb_osd_screen);
 if (auxCol == 255)
  return;
 auxCol= ShowTinyMenu("Pixels",gb_osd_screen_values,max_gb_osd_screen_values);
 if (auxCol != 255)
 {
  auxCol = auxCol<<1; //x2
  gb_screen_xOffset = auxCol; 
 }
 /*switch (aSelNum)
 {
  case 2:
   //OSD_PreparaPaleta8Colores();
   OSD_PreparaPaleta64para8Colores();
   memcpy(gb_cache_zxcolor,gb_const_colornormal8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_colornormal8_bright,8);
   #endif
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;
  case 3:
   OSD_PreparaPaleta64para8Colores();
   memcpy(gb_cache_zxcolor,gb_const_monoBlue8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_monoBlue8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   //for (int k=0;k<256;k++)
   // printf("%d %d %d\n",gb_osd_sdl_palette[k].r,gb_osd_sdl_palette[k].g,gb_osd_sdl_palette[k].b);
   //for (int k=0;k<8;k++)
   // printf("%d ",gb_cache_zxcolor[k]);
   //printf("\n");    
   return;
   break;
  case 4:
   OSD_PreparaPaleta64para8Colores();   
   memcpy(gb_cache_zxcolor,gb_const_monoGreen8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_monoGreen8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;
  case 5:
   OSD_PreparaPaleta64para8Colores();   
   memcpy(gb_cache_zxcolor,gb_const_monoRed8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_monoRed8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
  case 6:
   OSD_PreparaPaleta64para8Colores();   
   memcpy(gb_cache_zxcolor,gb_const_grey8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_grey8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;          
 }
 auxCol= ShowTinyMenu("Cols or pixels",gb_osd_screen_values,max_gb_osd_screen_values);
 switch (aSelNum)
 {
  case 0: gb_screen_xIni = -(auxCol<<3); break;
  case 1: gb_screen_yIni = (auxCol<=3)? (-auxCol):(-3); break;  
  default: break;
 }
 */
}

void ShowTinyMouseMenu()
{
 #ifdef use_lib_amx_mouse
  unsigned char aSelNum,aSelBtn,aSelMouseEnabled;
  aSelNum = ShowTinyMenu("Mouse",gb_osd_mouse_menu,max_gb_osd_mouse_menu);   
  switch (aSelNum)
  {
   case 0:
    #ifdef use_lib_amx_mouse
     gb_mouse_init_error= 0;
    #endif 
    break; //Init Detectar raton
   case 1:
    aSelBtn = ShowTinyMenu("Mouse buttons",gb_osd_mouse_buttons_menu,max_gb_osd_mouse_buttons_menu);
    switch (aSelBtn)
    {
     case 0: gb_force_left_handed= 0; break; //diestro
     case 1: gb_force_left_handed= 1; break; //zurdo
     default: break;
    }    
    //gb_force_left_handed= 1; 
    break; //zurdo
   case 2:
    aSelMouseEnabled = ShowTinyMenu("Mouse Enabled",gb_value_binary_menu,max_gb_value_binary_menu);
    #ifdef use_lib_amx_mouse
     switch (aSelMouseEnabled)
     {
      case 0: gb_mouse_init_error= 1; break; //Error 0 usa mouse
      case 1: gb_mouse_init_error= 0; break; //Error 1 apaga mouse
      default: break;
     }
    #endif
    break;
   default: break;
  }
 #endif
 //vTaskDelay(2);
}


//*******************************************
void SDLActivarOSDMainMenu()
{     
 gb_show_osd_main_menu= 1;   
}

//Very small tiny osd
void do_tinyOSD() 
{
 unsigned char aSelNum;
 if (checkAndCleanKey(KEY_F1))
 {
  gb_show_osd_main_menu= 1;
  #ifdef use_lib_sound_ay8912  
   gb_silence_all_channels = 1;
  #endif    
  return;
 }

 if (gb_show_osd_main_menu == 1)
 {
  aSelNum = ShowTinyMenu("MAIN MENU",gb_main_menu,max_gb_main_menu);
  switch (aSelNum)
  {
   case 0: ShowTinyMachineMenu(); break;
   case 1: ShowTinyDSKMenu(); break;
//   case 2: ShowTinyTAPEMenu(); break;
//   case 3: ShowTinySelectTAPEMenu(); break;
//   case 4: ShowTinySCRMenu(); break;
   case 2: ShowTinySpeedMenu(); break;
   case 3: ShowTinyScreenAdjustMenu(); break;
   case 4: ShowTinyMouseMenu(); break;
   case 5: ShowTinySoundMenu(); break;
   case 6: ShowTinyResetMenu(); break; 
    //resetz80();
    //resetcrtc();
    //SDL_Delay(2);
    //break;
//   case 5: ShowTinySpeedMenu(); break;   
//   case 9: OSD_Test64Colores(); break;
//   case 10: OSD_Test8Colores(); break;*/
   default: break;
  }
 }
 #ifdef use_lib_sound_ay8912  
  //gb_silence_all_channels = 0; 
  gb_silence_all_channels = gb_mute_sound;
 #endif 
}

