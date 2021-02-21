//Port Cpc-em (Tom Walker) emulator to TTGO VGA32 by ackerman:
// DSK 44 tracks 11 sectors
// VGA 400x300, 320x200
// Mode 464, 664, 6128
// Modo 128K (inestable)
// Low video ram mode
// Mode 8 colors, 64 colors
// Audio AY8912 fabgl library 0.9.0 (not include)
// VGA library bitluni 0.3.3 (include)
// gbConfig options configuration compile

#include "Emulator/Keyboard/PS2Kbd.h"
#include <Arduino.h>
#include "PS2Boot/PS2KeyCode.h"
//Para ahorrar memoria
//JJ #include <esp_bt.h>

#include "gbConfig.h"

#ifdef use_lib_amx_mouse
 #include "fabgl.h" //Para fabgl
 #include "WiFiGeneric.h"; //Fix WiFiGeneric.h No such file or directory
#endif

#ifdef use_lib_sound_ay8912
 #include "fabgl.h" //Para fabgl
 #include "fabutils.h" //Para fabgl
 #include "WiFiGeneric.h"
#endif 

#include "CPCEM.h"
#include "MartianVGA.h"
#include "def/Font.h"
#include "def/hardware.h"
#include "driver/timer.h"
#include "soc/timer_group_struct.h"

#include "CRTC.h"
#include "FDC.h"
#include "PSG.h"
#include "Z80.h"
#include "gb_globals.h"
#include "osd.h"

#ifdef COLOR_3B
 #ifdef use_lib_vga_low_memory
  VGA3BitI vga; 
 #else
  VGA3Bit vga;
 #endif 
#else
 #ifdef use_lib_vga_low_memory
  VGA6BitI vga;
 #else
  VGA6Bit vga;
 #endif
#endif

#ifdef use_lib_amx_mouse
 //short int gb_sdl_mouse_x;
 //short int gb_sdl_mouse_y;
 //short int gb_sdl_mouse_x_before;
 //short int gb_sdl_mouse_y_before; 
 //unsigned char gb_auxMouseBtn;
 //unsigned char gb_auxMouseBtn_before;
 unsigned char gb_mouse_key_btn_left=1;
 unsigned char gb_mouse_key_btn_right=1;
 unsigned char gb_mouse_key_left=1;
 unsigned char gb_mouse_key_right=1;
 unsigned char gb_mouse_key_up=1;
 unsigned char gb_mouse_key_down=1; 
 #ifdef use_lib_amx_mouse_lefthanded
  unsigned char gb_force_left_handed=1;
 #else
  unsigned char gb_force_left_handed=0; 
 #endif
 fabgl::PS2Controller PS2Controller;
#endif 

unsigned char gb_run_emulacion = 1; //Ejecuta la emulacion
unsigned char gb_current_ms_poll_sound = gb_ms_sound;
unsigned char gb_current_ms_poll_keyboard = gb_ms_keyboard;
unsigned char gb_current_ms_poll_mouse = gb_ms_mouse;
unsigned char gb_current_frame_crt_skip= gb_frame_crt_skip; //No salta frames
unsigned char gb_current_delay_emulate_ms= gb_delay_emulate_ms;
unsigned char gb_sdl_blit=0;
unsigned char gb_screen_xOffset=0;
static unsigned long gb_time_ini_espera;

//volatile unsigned char keymap[256];
//volatile unsigned char oldKeymap[256];
//unsigned char *ramArray[8];
static unsigned long gb_currentTime;
static unsigned long gb_keyboardTime;
static unsigned long gb_mouseTime;
unsigned char *ram;
#ifdef use_lib_mem_blocks
 unsigned char *ramArray[2]; //2 bloques de 64 KB
#endif
//unsigned char gb_sdl_current_frame=0;
#ifdef use_lib_ultrafast_vga
 unsigned char ** ptrVGA;
#endif

//JJ unsigned char gb_sdl_blit=0;//solo necesito en pc
unsigned char model;
//JJ int soundavail=1; //no necesito
//JJ char discname[260]="";//no lo necesito
//JJ int quit; //no lo necesito
int soundon;
int spdc=0;

#ifdef use_lib_sound_ay8912
 SoundGenerator soundGenerator;
 SineWaveformGenerator gb_sineArray[3];
 unsigned char gbVolMixer_before[3]={0,0,0};
 unsigned short int gbFrecMixer_before[3]={0,0,0};
 unsigned char gbVolMixer_now[3]={0,0,0};
 unsigned short int gbFrecMixer_now[3]={0,0,0}; 
 unsigned char gb_silence_all_channels=1; 
 static unsigned long gb_sdl_time_sound_before;
  
 void sound_cycleFabgl(void);
 void jj_mixpsg(void);
#endif

#ifdef use_lib_sound_ay8912
 void SilenceAllChannels()
 {  
  for (unsigned char i=0;i<3;i++)  
  {
   gb_sineArray[i].setFrequency(0);
   gb_sineArray[i].setVolume(0);
   gbVolMixer_before[i] = gbVolMixer_now[i] = 0;
   gbFrecMixer_before[i] = gbFrecMixer_now[i] = 0;
  }  
 }

 inline void sound_cycleFabgl()
 {  
  //AY8912
  for (unsigned char i=0;i<3;i++)
  {
   if (gbVolMixer_now[i] != gbVolMixer_before[i])
   {
    gb_sineArray[i].setVolume((gbVolMixer_now[i]<<2));
    gbVolMixer_before[i] = gbVolMixer_now[i];
   }
   if (gbFrecMixer_now[i] != gbFrecMixer_before[i])
   {
    gb_sineArray[i].setFrequency(gbFrecMixer_now[i]);
    gbFrecMixer_before[i] = gbFrecMixer_now[i];
   }
  }
 }

 inline void jj_mixpsg()
 {
  int auxFrec;
  gbVolMixer_now[0] = psgregs[8]&15;//((psgregs[8]&15)>4)?16:0;
  gbVolMixer_now[1] = psgregs[9]&15;//((psgregs[9]&15)>4)?16:0;
  gbVolMixer_now[2] = psgregs[10]&15;//((psgregs[10]&15)>4)?16:0;
  if (gbVolMixer_now[0] == 0) gbFrecMixer_now[0] = 0;
  else{
   gbVolMixer_now[0]=15;
   auxFrec =((psgregs[0]|((psgregs[1]<<8)&0xF00)))+1;
   auxFrec = (auxFrec>0)?(62500/auxFrec):0;
   if (auxFrec>15000) auxFrec=15000;
   gbFrecMixer_now[0] = auxFrec;
  }
  if (gbVolMixer_now[1] == 0) gbFrecMixer_now[1] = 0;
  else{
   gbVolMixer_now[1]=15; 
   auxFrec = ((psgregs[2]|((psgregs[3]<<8)&0xF00)))+1;  
   auxFrec = (auxFrec>0)?(62500/auxFrec):0;
   if (auxFrec>15000) auxFrec=15000;   
   gbFrecMixer_now[1] = auxFrec;
  }
  if (gbVolMixer_now[2] == 0) gbFrecMixer_now[2] = 0;
  else{
   gbVolMixer_now[2]=15;
   auxFrec = ((psgregs[4]|((psgregs[5]<<8)&0xF00)))+1;
   auxFrec = (auxFrec>0)?(62500/auxFrec):0;
   if (auxFrec>15000) auxFrec=15000;   
   gbFrecMixer_now[2] = auxFrec;
  }
 } 
#endif 

#ifdef use_lib_amx_mouse
 void PollMouse(void);
#endif


void SDL_keys_poll()
{

   //case SDLK_HOME: cpckeys[0][0]= 0; break; //Home
   //case SDLK_PAGEDOWN: cpckeys[0][1]= 0; break; //Pg Down
   //case SDLK_END: cpckeys[0][2]= 0; break; //End
   cpckeys[0][3]= keymap[PS2_KC_KP9]; //Num keypad 9 0x7D
   cpckeys[0][4]= keymap[PS2_KC_KP6]; //Num keypad 6 0x74
   cpckeys[0][5]= keymap[PS2_KC_KP3]; //Num keypad 3 0x7A
   //case enterPad
   //case SDLK_KP_CLEAR: cpckeys[0][7]= 0; break; //Delete supr numpad   

   //case SDLK_PAGEUP: cpckeys[1][0]= 0; break; //Pg Up
   ////case 0xFF
   cpckeys[1][2]= keymap[PS2_KC_KP7]; //Num keypad 7 0x6C
   cpckeys[1][3]= keymap[PS2_KC_KP8]; //Num keypad 8 0x75
   cpckeys[1][4]= keymap[PS2_KC_KP5]; //Num keypad 5 0x73
   cpckeys[1][5]= keymap[PS2_KC_KP1]; //Num keypad 1 0x69
   cpckeys[1][6]= keymap[PS2_KC_KP2]; //Num keypad 2 0x72
   cpckeys[1][7]= keymap[PS2_KC_KP0]; //Num keypad 0 0x70

   cpckeys[9][7] = cpckeys[2][0]= (keymap[PS2_KC_L_SHIFT]==0 && keymap[PS2_KC_0]==0)?0:1;//delete shift 0x12 0x45
   cpckeys[2][1] = keymap[PS2_KC_OPEN_SQ]; //case SDLK_OPENBRACE: 0x54
   cpckeys[2][2]= keymap[PS2_KC_ENTER]; //return 0x5A
   cpckeys[2][3] = keymap[PS2_KC_CLOSE_SQ]; //case SDLK_CLOSEBRACE:    0x5B
   //case SDLK_LEFT: cpckeys[2][4]= 0; break;
   cpckeys[2][5]= keymap[PS2_KC_R_SHIFT];//rshift 0x59
   cpckeys[2][6]= keymap[PS2_KC_BACK];//BACKSLASH barra izquierda 0x5D
   cpckeys[2][7]= keymap[PS2_KC_CTRL];//rcontrol   0x14
                          
   cpckeys[3][0]= keymap[PS2_KC_F2]; //F2 0x06
   cpckeys[3][1]= keymap[PS2_KC_MINUS]; //MINUS - 0x4E
   cpckeys[3][2]= keymap[PS2_KC_F10]; //| para cpm con el shift+F10 SDLK_TILDE 0x09
   cpckeys[3][3]= keymap[PS2_KC_P]; //p 0x4D   
   if(
      ((keymap[PS2_KC_L_SHIFT]==0) && (keymap[PS2_KC_COMMA]==0))
      ||
      ((keymap[PS2_KC_R_SHIFT]==0) && (keymap[PS2_KC_COMMA]==0))
     )
   {
    cpckeys[3][4]= 0; //; L shift ,  R shift ,
   }
   else
   {
    cpckeys[3][4]= keymap[PS2_KC_SEMI]; //; ñ 0x4C
   }

   cpckeys[3][5] = keymap[PS2_KC_APOS]; //case SDLK_QUOTE: cpckeys[3][5]= 0; break; //' 0x52
   //case SDLK_SLASH: cpckeys[3][6]= 0; break;   
   cpckeys[3][7]= keymap[PS2_KC_DOT]; //teclado stop . 0x49


   cpckeys[4][0]= keymap[PS2_KC_0]; //0 0x45
   cpckeys[4][1]= keymap[PS2_KC_9]; //9 0x46
   cpckeys[4][2]= keymap[PS2_KC_O]; //o 0x44
   cpckeys[4][3]= keymap[PS2_KC_I]; //i 0x43
   cpckeys[4][4]= keymap[PS2_KC_L]; //l 0x4B
   cpckeys[4][5]= keymap[PS2_KC_K]; //k 0x42
   cpckeys[4][6]= keymap[PS2_KC_M]; //m 0x3A
   if ((keymap[PS2_KC_L_SHIFT]==1)&&(keymap[PS2_KC_R_SHIFT]==1))
   {
    cpckeys[4][7]= keymap[PS2_KC_COMMA]; //, 0x41
   }

   cpckeys[5][0]= keymap[PS2_KC_8];//8 0x3E
   cpckeys[5][1]= keymap[PS2_KC_7];//7 0x3D
   cpckeys[5][2]= keymap[PS2_KC_U];//u 0x3C
   cpckeys[5][3]= keymap[PS2_KC_Y];//y 0x35
   cpckeys[5][4]= keymap[PS2_KC_H];//h 0x33
   cpckeys[5][5]= keymap[PS2_KC_J];//j 0x3B
   cpckeys[5][6]= keymap[PS2_KC_N];//n 0x31
   cpckeys[5][7]= keymap[PS2_KC_SPACE];//space 0x29

   cpckeys[6][0]= keymap[PS2_KC_6];//6 0x36
   cpckeys[6][1]= keymap[PS2_KC_5];//5 0x2E
   cpckeys[6][2]= keymap[PS2_KC_R];//r 0x2D
   cpckeys[6][3]= keymap[PS2_KC_T];//t 0x2C
   cpckeys[6][4]= keymap[PS2_KC_G];//g 0x34
   cpckeys[6][5]= keymap[PS2_KC_F];//f 0x2B
   cpckeys[6][6]= keymap[PS2_KC_B];//b 0x32
   cpckeys[6][7]= keymap[PS2_KC_V];//v 0x2A

   cpckeys[7][0]= keymap[PS2_KC_4];//4 0x25
   cpckeys[7][1]= keymap[PS2_KC_3];//3 0x26
   cpckeys[7][2]= keymap[PS2_KC_E];//e 0x24
   cpckeys[7][3]= keymap[PS2_KC_W];//w 0x1D
   cpckeys[7][4]= keymap[PS2_KC_S];//s 0x1B
   cpckeys[7][5]= keymap[PS2_KC_D];//d 0x23
   cpckeys[7][6]= keymap[PS2_KC_C];//c 0x21
   cpckeys[7][7]= keymap[PS2_KC_X];//x 0x22

   cpckeys[8][0]= keymap[PS2_KC_1];//1 0x16
   cpckeys[8][1]= keymap[PS2_KC_2];//2 0x1E
   cpckeys[8][2]= keymap[PS2_KC_ESC];//ESCAPE 0x76
   cpckeys[8][3]= keymap[PS2_KC_Q];//q 0x15
   cpckeys[8][4]= keymap[PS2_KC_TAB];//tab 0x0D
   cpckeys[8][5]= keymap[PS2_KC_A];//a 0x1C
   cpckeys[8][6]= keymap[PS2_KC_CAPS];//capslock 0x58
   cpckeys[8][7]= keymap[PS2_KC_Z];//z 0x1A

   #ifdef use_lib_amx_mouse
    cpckeys[9][0]= keymap[KEY_CURSOR_UP] && gb_mouse_key_up;  //UP
    cpckeys[9][1]= keymap[KEY_CURSOR_DOWN] && gb_mouse_key_down; //down
    cpckeys[9][2]= keymap[KEY_CURSOR_LEFT] && gb_mouse_key_left; //left
    cpckeys[9][3]= keymap[KEY_CURSOR_RIGHT] && gb_mouse_key_right; //right
    #ifdef use_lib_amx_mouse_lefthanded
     //zurdo
     cpckeys[9][5] = keymap[PS2_KC_KP0] && gb_mouse_key_btn_left; //0x70
     cpckeys[9][4] = keymap[PS2_KC_KP_DOT] && gb_mouse_key_btn_right;  //0x71
    #else
     //Diestro
     if (gb_force_left_handed == 1)
     {//zurdo
      cpckeys[9][5] = keymap[PS2_KC_KP0] && gb_mouse_key_btn_left; //0x70
      cpckeys[9][4] = keymap[PS2_KC_KP_DOT] && gb_mouse_key_btn_right; //0x71
     }
     else
     {//diestro
      cpckeys[9][4] = keymap[PS2_KC_KP0] && gb_mouse_key_btn_left; //insert 0 keypad
      cpckeys[9][5] = keymap[PS2_KC_KP_DOT] && gb_mouse_key_btn_right; //dot . keypad
     }
    #endif
   #else
    cpckeys[9][0]= keymap[KEY_CURSOR_UP];//UP
    cpckeys[9][1]= keymap[KEY_CURSOR_DOWN]; //down
    cpckeys[9][2]= keymap[KEY_CURSOR_LEFT]; //left
    cpckeys[9][3]= keymap[KEY_CURSOR_RIGHT]; //right   
    cpckeys[9][4] = keymap[PS2_KC_KP0];
    cpckeys[9][5] = keymap[PS2_KC_KP_DOT];
   #endif

   //case SDLK_INSERT: cpckeys[9][4]= 0; break;
   //
   //
   cpckeys[9][7]= keymap[KEY_BACKSPACE]; //Backspace Borrar KEY_BACKSPACE
}

#ifdef use_lib_amx_mouse
 void SDL_keys_mouse_set_btnLeft(unsigned char aux)
 {  
  #ifdef use_lib_amx_mouse_lefthanded
   //Zurdo
   if (gb_force_left_handed == 1)
    cpckeys[9][4]= aux;
   else 
    cpckeys[9][5]= aux;
  #else
   //Diestro
   if (gb_force_left_handed == 1)
    cpckeys[9][5]= aux;
   else 
    cpckeys[9][4]= aux;
  #endif 
  //keymap[KEY_ALT_GR] = 0;   
 }

 void SDL_keys_mouse_set_btnRight(unsigned char aux)
 {
  #ifdef use_lib_amx_mouse_lefthanded
   //zurdo
   if (gb_force_left_handed == 1)
    cpckeys[9][5]= aux;
   else 
    cpckeys[9][4]= aux;
  #else
   //diestro
   if (gb_force_left_handed == 1)
    cpckeys[9][4]= aux;
   else 
    cpckeys[9][5]= aux;
  #endif 
 } 

void PollMouse(void)
{
 auto mouse = PS2Controller.mouse();
 //cpckeys[9][4]= 1;
 //cpckeys[9][5]= 1;
 //keymap[KEY_CURSOR_RIGHT] = 1;
 //keymap[KEY_CURSOR_LEFT] = 1;
 //keymap[KEY_CURSOR_DOWN] = 1;
 //keymap[KEY_CURSOR_UP] = 1;
 gb_mouse_key_right = gb_mouse_key_left= gb_mouse_key_down = gb_mouse_key_up = 1;
 //gb_mouse_key_btn_left = gb_mouse_key_btn_right = 1;
 
 //MouseStatus auxStatus = mouse->getNextStatus();
 //gb_mouse_key_btn_left = (auxStatus.buttons.left)?0:1;
 //gb_mouse_key_btn_right= (auxStatus.buttons.right)?0:1;
 if (mouse->deltaAvailable())
 {  
  MouseDelta mouseDelta;
  mouse->getNextDelta(&mouseDelta);  

  gb_mouse_key_btn_left = (mouseDelta.buttons.left)?0:1;
  gb_mouse_key_btn_right = (mouseDelta.buttons.right)?0:1;

  /*if (mouseDelta.buttons.left)     
   SDL_keys_mouse_set_btnLeft(0);  
  else
   SDL_keys_mouse_set_btnLeft(1);

  if (mouseDelta.buttons.right)
   SDL_keys_mouse_set_btnRight(0);
  else
   SDL_keys_mouse_set_btnRight(1);
   
  cpckeys[9][3]= (mouseDelta.deltaX > 0)?0:1;       
  cpckeys[9][2] = (mouseDelta.deltaX < 0)?0:1;
  cpckeys[9][1] = (mouseDelta.deltaY < 0)?0:1;
  cpckeys[9][0] = (mouseDelta.deltaY > 0 )?0:1; */

  gb_mouse_key_right= (mouseDelta.deltaX > 0)?0:1;       
  gb_mouse_key_left = (mouseDelta.deltaX < 0)?0:1;
  gb_mouse_key_down = (mouseDelta.deltaY < 0)?0:1;
  gb_mouse_key_up = (mouseDelta.deltaY > 0 )?0:1;     
  
  //Serial.printf("deltaX = %d  deltaY = %d  deltaZ = %d  leftBtn = %d  midBtn = %d  rightBtn = %d\n",
  //              mouseDelta.deltaX, mouseDelta.deltaY, mouseDelta.deltaZ,
  //              mouseDelta.buttons.left, mouseDelta.buttons.middle, mouseDelta.buttons.right);
 }
 
 //cpckeys[9][3] &= gb_mouse_key_right;
 //cpckeys[9][2] &= gb_mouse_key_left;
 //cpckeys[9][1] &= gb_mouse_key_down;
 //cpckeys[9][0] &= gb_mouse_key_up;

}
#endif



void setup()
{
 //DO NOT turn off peripherals to recover some memory
 //esp_bt_controller_deinit(); //Reduzco consumo RAM
 //esp_bt_controller_mem_release(ESP_BT_MODE_BTDM); //Reduzco consumo RAM
 #ifdef use_lib_mem_blocks
  //for (int i=0;i<2;i++)
  // ramArray[i] = (unsigned char *)calloc(0x10000,1);//64KB+16KB fix error
  ramArray[0] = (unsigned char *)calloc(0x10000,1);//64KB+16KB fix error
  ramArray[1] = (unsigned char *)calloc(0x10000,1);//64KB
  ram = ramArray[0]; 
 #else
  #ifdef use_lib_cheat_128k
   ram=(unsigned char *)malloc(0x1B000);  //truco 122880 bytes
   memset(ram,1,0x10000);
  #else
   #ifdef use_lib_fix_psram_128k
    ram=(unsigned char *)ps_malloc(0x20000); //Modo 128K psram
    memset(ram,1,0x20000);
   #else
    ram=(unsigned char *)malloc(0x10000); //Modo 64K
    memset(ram,1,0x10000);
   #endif 
  #endif 
 #endif 

 #ifdef use_lib_log_serial
  Serial.begin(115200);         
  Serial.printf("HEAP BEGIN %d\n", ESP.getFreeHeap()); 
 #endif

 //ram=(unsigned char *)heap_caps_malloc(0x20000,MALLOC_CAP_8BIT);
 //for (int i=0;i<8;i++)
 // ramArray[i] = (unsigned char *)calloc(0x20000,1);
  
 //Serial.printf("Address RAM: %d\n",ram);


 #ifdef use_lib_log_serial
  Serial.printf("RAM %d\n", ESP.getFreeHeap()); 
 #endif

 #ifdef COLOR_3B
  #ifdef use_lib_400x300
   vga.init(vga.MODE400x300, RED_PIN_3B, GRE_PIN_3B, BLU_PIN_3B, HSYNC_PIN, VSYNC_PIN);       
  #else
   vga.init(vga.MODE320x200, RED_PIN_3B, GRE_PIN_3B, BLU_PIN_3B, HSYNC_PIN, VSYNC_PIN);    
  #endif
 #else
  const int redPins[] = {RED_PINS_6B};
  const int grePins[] = {GRE_PINS_6B};
  const int bluPins[] = {BLU_PINS_6B}; 
  #ifdef use_lib_400x300
   vga.init(vga.MODE400x300, redPins, grePins, bluPins, HSYNC_PIN, VSYNC_PIN);   
  #else
   vga.init(vga.MODE320x200, redPins, grePins, bluPins, HSYNC_PIN, VSYNC_PIN);
  #endif 
 #endif
 #ifdef use_lib_ultrafast_vga  
  PrepareColorsUltraFastVGA();  
 #endif 
 vga.setFont(Font6x8);
 vga.clear(BLACK); 
 #ifdef use_lib_ultrafast_vga
  #ifdef use_lib_400x300
   vga.fillRect(0,0,400,300,BLACK);
   vga.fillRect(0,0,400,300,BLACK);//fix mode fast video
  #else
   vga.fillRect(0,0,320,200,BLACK);
   vga.fillRect(0,0,320,200,BLACK);//fix mode fast video
  #endif 
  ptrVGA = vga.backBuffer; //Asignamos  puntero buffer
 #endif  

 #ifdef use_lib_log_serial
  Serial.printf("VGA %d\n", ESP.getFreeHeap()); 
 #endif

 #ifdef use_lib_amx_mouse   
  PS2Controller.begin();
 #endif 

 kb_begin();

 //resolution=0;
 model=0;
 soundon=0;
 initz80();
 loadroms2FlashModel();
 resetz80();
 resetcrtc();
 loaddsk2Flash(0);

 gb_keyboardTime = gb_currentTime = millis();
 #ifdef use_lib_amx_mouse
  gb_mouseTime = gb_keyboardTime;
 #endif

 #ifdef use_lib_sound_ay8912
  gb_sdl_time_sound_before = gb_currentTime;
  for (unsigned char i=0;i<3;i++)
  {
   soundGenerator.attach(&gb_sineArray[i]);
   gb_sineArray[i].enable(true);
   gb_sineArray[i].setFrequency(0);
  }
  soundGenerator.play(true);
 #endif

 #ifdef use_lib_log_serial  
  Serial.printf("END SETUP %d\n", ESP.getFreeHeap()); 
 #endif
}




// VIDEO core 0 *************************************

//Video sin hilos
//void videoTaskNoThread()
//{
//}



// +-------------+
// | LOOP core 1 |
// +-------------+
//
void loop()
{
 gb_currentTime = millis();
 #ifdef use_lib_amx_mouse
  if ((gb_currentTime-gb_mouseTime) >= gb_current_ms_poll_mouse)
  {
   gb_mouseTime = gb_currentTime;  
   PollMouse();
  }
 #endif

 if ((gb_currentTime-gb_keyboardTime) >= gb_current_ms_poll_keyboard)
 { 
  gb_keyboardTime = gb_currentTime;  
  SDL_keys_poll();
 } 
 do_tinyOSD();

 if ((gb_current_delay_emulate_ms == 0) || (gb_run_emulacion == 1))
 {     
  cpuline=0;
  execz80();
 }

 if (gb_current_delay_emulate_ms != 0)
 {
  if (gb_sdl_blit == 1)
  {
   gb_sdl_blit= 0;
   gb_run_emulacion= 0;
   gb_time_ini_espera = millis();
  }
 }

 #ifdef use_lib_sound_ay8912
  if (gb_silence_all_channels == 1)
   SilenceAllChannels();
  else
  {
   gb_currentTime = millis();
   if ((gb_currentTime-gb_sdl_time_sound_before) >= gb_current_ms_poll_sound)
   {
    gb_sdl_time_sound_before= gb_currentTime;
    jj_mixpsg();
    sound_cycleFabgl();
   }
  }
 #endif   

 if (gb_current_delay_emulate_ms != 0)
 {
  gb_currentTime = millis();     
  if (gb_run_emulacion == 0)  
   if ((gb_currentTime - gb_time_ini_espera) >= gb_current_delay_emulate_ms)   
    gb_run_emulacion = 1;     
 }
 
 //videoTaskNoThread(); 
}
