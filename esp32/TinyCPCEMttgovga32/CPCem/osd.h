#ifndef OSDFILE_H
 #define OSDFILE_H
 
 void changeSna2Flash(unsigned char id);
 void do_tinyOSD(void);
 //void SDLAssignSDLSurfaceOSD(SDL_Surface *surface,SDL_Event * ptrEvent);
 void SDLActivarOSDMainMenu();
 void SDLOSDClear(void);
 //SDL_Surface * SDLOSDGetSurface();
 void SDLEsperaTeclado(void);
 void SDLprintCharOSD(char car,int x,int y,unsigned char color,unsigned char backcolor);
 void SDLprintText(const char *cad,int x, int y, unsigned char color,unsigned char backcolor);
 void ShowTinyRunROMMenu(unsigned char idRom);
#endif
