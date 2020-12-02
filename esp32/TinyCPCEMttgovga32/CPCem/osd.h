#ifndef OSDFILE_H
 #define OSDFILE_H
 
 void changeSna2Flash(unsigned char id);
 void do_tinyOSD(void);
 //void SDLAssignSDLSurfaceOSD(SDL_Surface *surface,SDL_Event * ptrEvent);
 void SDLActivarOSDMainMenu();
 void SDLOSDClear(void);
 //SDL_Surface * SDLOSDGetSurface();
 void SDLEsperaTeclado(void);
#endif
