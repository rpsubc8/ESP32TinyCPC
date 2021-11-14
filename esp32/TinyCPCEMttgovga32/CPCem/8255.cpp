#include "gbConfig.h"
#include "CPCem.h"
#include "PSG.h"
#include "8255.h"
#include "gbGlobals.h"

//JJ int crtcline,sc,vc;
struct
{
        unsigned char portc;
        unsigned char ctrl;
} PIA;

void write8255(unsigned short a, unsigned char v)
{
        switch (a&0xFF00)
        {
                case 0xF400: psgdat=v; updatepsg(); return;
                case 0xF500: return;
                case 0xF600: PIA.portc=v; psgmode=v>>6; updatekeys(v&15); updatepsg(); return;
                case 0xF700: if (v&0x80) PIA.ctrl=v; return;
        }
}

unsigned char read8255(unsigned short a)
{
        switch (a&0xFF00)
        {
                case 0xF400: return psgdat;
                case 0xF500: /*printf("Read %i at %i %i %04X\n",crtcvsync,crtcline,(vc<<8)|sc,pc); */return 0x3E|crtcvsync;
                case 0xF600: return PIA.portc;
                case 0xF700: return PIA.ctrl;
        }
}
