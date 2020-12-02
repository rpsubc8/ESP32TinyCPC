#ifndef _GB_GLOBALS_H
 #define _GB_GLOBALS_H
 #include "gbConfig.h"
 #include <stdio.h>

 extern unsigned char keymap[256];
 extern unsigned char oldKeymap[256];

#ifdef use_lib_sound_ay8912
 extern unsigned char gb_silence_all_channels;
#endif 

#ifdef use_lib_ultrafast_vga
 extern unsigned char ** ptrVGA;
#endif
//extern unsigned char gb_sdl_current_frame;

extern unsigned char gb_current_ms_poll_sound; //milisegundos muestreo
extern unsigned char gb_current_ms_poll_keyboard; //milisegundos muestreo teclado
extern unsigned char gb_current_frame_crt_skip; //el actual salto de frame
extern unsigned char gb_current_delay_emulate_ms; //la espera en cada iteracion
extern unsigned char gb_sdl_blit;
extern unsigned char gb_screen_xOffset;

extern unsigned char gb_show_osd_main_menu;
//JJ extern int cpckeys[16][8];
extern unsigned char cpckeys[16][8];
//JJ extern unsigned char gb_sdl_blit; //solo lo necesito en pc

extern unsigned char psgdat;
extern unsigned char psgmode;
extern unsigned char loromena,hiromena;
//,curhrom;
extern unsigned char curhrom;
extern unsigned char curhromBefore;

extern unsigned char psgreg,psgregs[16];
//JJ extern int psgvols[4][512]; //no necesito sonido
//JJ extern int psglatchs[5][512];//no necesito sonido

//extern unsigned char *ramArray[8]; //8 bancos de 16384 bytes
extern unsigned char *ram;
#ifdef use_lib_mem_blocks
 extern unsigned char *ramArray[2]; //2 bloques de 64 KB
#endif
extern unsigned char *lorom;
extern unsigned char *hirom[16];
extern unsigned char *readarray[4],*writearray[4];

//Z80
typedef union
{
        unsigned short w;
        struct
        {
                unsigned char l,h;
        } b;
} z80reg;

//JJ z80reg af,bc,de,hl,ix,iy,ir,saf,sbc,sde,shl;
//JJ unsigned short pc,sp;
//JJ int iff1,iff2;
//JJ int intreq;
//JJ int im;

extern z80reg af,bc,de,hl,ix,iy,ir,saf,sbc,sde,shl;
extern unsigned short pc,sp;
extern int iff1,iff2;
extern int intreq;
extern int im;

//GA
extern int galines;
extern int gapal[16];
extern unsigned char gaborder;
extern unsigned char palchange;
extern unsigned char scrmode;


//CRT
extern int crtcvsync;
extern int linessincevsync;
extern int galc;
//JJ extern int motoron;//no necesito estado motor disco
extern int vc;
extern int sc;
extern int crtcline;
extern int vsyncpulse;
//JJ extern int vols[3][512];//no ncesito sonido
//JJ extern int pglatchs[5][512];//no ncesito sonido
//extern unsigned char resolution;
extern int cpuline;
extern unsigned char crtctype;
extern unsigned short ma;
extern unsigned char crtcregs[32];
extern int crtcreg;
//JJ extern unsigned short crtctable[8][0x8000]; //sin precalculada


//FDC
//extern unsigned char gb_select_dsk_disk; //el disco actual
//extern int gb_ptrBeginDataDisc[42][11];//42 tracks 11 sectors
extern int startreal;
extern unsigned char discon;
extern int endread;
extern int fdcint;
extern unsigned char fdcstatus;
extern int params;
extern int readparams;
extern unsigned char paramdat[16];
extern unsigned char command;
extern unsigned char st0,st1,st2,st3;
extern int fdctrack;
extern int starttrack,startsector,endsector;
extern int posinsector;
extern unsigned char reading;
//JJ extern int disctracks; //no lo necesito
//extern int discsects[40]; //esta en rom
//JJ extern unsigned char discdat[42][11][512];
//extern unsigned char discid[42][11][4];
 typedef struct Tdiscsects
 {
  unsigned char datos[40];
 }Tdiscsect;
 typedef struct Tdiscids
 {
  unsigned char datos[42][11][4];
 }Tdiscid;
 typedef struct Tdiscdats
 {
  unsigned char datos[42][11][512];
 }Tdiscdat; 
 extern Tdiscsect *discsects;
 extern Tdiscid *discid;
 extern Tdiscdat *discdat;



//GA
extern int palentry;
//extern int ramconfig;
extern unsigned char ramconfig;
extern unsigned char ramconfigBefore;



//GUI
//JJ extern char discname[260]; //no lo necesito
//extern int soundavail; //no necesito
//JJ extern int quit; //no lo necesito
//extern int resolution;
extern unsigned char model;
extern int soundon; 
//JJ #define JJ_MAX_MENU 128 //no lo necesito
//JJ MENU discmenu[];
//JJ MENU modelmenu[];
//JJ MENU soundmenu[];
//JJ MENU videomenu[];
//JJ MENU cpcemmenu[];

//MENU discmenu[JJ_MAX_MENU];
//MENU modelmenu[JJ_MAX_MENU];
//MENU soundmenu[JJ_MAX_MENU];
//MENU videomenu[JJ_MAX_MENU];
//MENU cpcemmenu[JJ_MAX_MENU];



//PSG
//JJ int cpuline;
//JJ int soundon,soundavail;
//JJ extern int psgcount[5]; //no lo necesito
//JJ extern int psglatch[5];//no lo necesito
//extern int psgvol[4];//no lo necesito
#ifdef use_lib_sound_ay8912
 //extern int psglatch[5];
 extern unsigned char psgvol[4];
 extern unsigned char psgvolBefore[4];
 #define gb_frecuencia_ini 220
#endif
//JJ extern int psgstat[4];//no lo necesito
//JJ extern int psgenv;//no lo necesito
//JJ extern int psgenvpos;//no lo necesito
//JJ extern unsigned long psgnoise;//no lo necesito
//extern FILE *sndf;

#endif

 
