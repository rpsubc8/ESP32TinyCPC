//C72B - try command
//A9B0
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FDC.h"
#include "gbGlobals.h"
#include "dataFlash/gbdsk.h"
#include <iostream>

//unsigned char gb_select_dsk_disk=0;
//int gb_ptrBeginDataDisc[42][11]; //Donde empieza el disco
int startreal;
unsigned char discon;
int endread;
int fdcint=0;
unsigned char fdcstatus=0x80;
int params=0;
int readparams=0;
unsigned char paramdat[16]; //Escrive disco revisar
unsigned char command;
unsigned char st0,st1,st2,st3;
int fdctrack;
int starttrack,startsector,endsector;
int posinsector;
unsigned char reading=0;
Tdiscsect *discsects;
//JJ int disctracks; //no lo necesito
//JJ int discsects[40]; //esta en rom
//JJ unsigned char discdat[42][11][512];
//unsigned char discid[42][11][4];
Tdiscid *discid;
Tdiscdat *discdat;

using namespace std;

void dumpdisc()
{
//JJ        FILE *f=fopen("disk.dmp","wb");
//JJ        fwrite(discdat[0][0],512,1,f);
//JJ        fclose(f);
}

void loaddsk2Flash(unsigned char id)
{
 discid = (Tdiscid *)gb_list_dsk_discid[id];
 discdat = (Tdiscdat *)gb_list_dsk_discdat[id];;

/* int contBuffer=0;
 int numsect;
 int c,d;
 char head[40];
 unsigned char dskhead[256],trkhead[256];
 
 gb_select_dsk_disk = id;
 
 memcpy(dskhead,&gb_list_dsk_data[id][contBuffer],256);
 cout<<"Fichero "<<dskhead<<"\n";
 contBuffer+= 256;
 for (c=0;c<40;c++) head[c]=0;
 for (c=0;c<0x21;c++) head[c]=dskhead[c];
 disctracks=dskhead[0x30];
 //printf ("Head\nDisc tracks %d ",disctracks);
 for (d=0;d<disctracks;d++)
 {
  memcpy(trkhead,&gb_list_dsk_data[id][contBuffer],256);
  contBuffer+= 256;
  //cout<< "cont " <<contBuffer<<"\n";
  while (strncmp((const char *)trkhead,"Track-Info",10) && contBuffer< gb_list_dsk_size[id])
  {//194816 bytes normalmente
   memcpy(trkhead,&gb_list_dsk_data[id][contBuffer],256);
   contBuffer+= 256;
   //printf ("cont %d\n", contBuffer);
   if (contBuffer > gb_list_dsk_size[id])
    return;      
  }
  discsects[d]=numsect=trkhead[0x15];   
  //printf ("Num sects %d\n", numsect);
  for (c=0;c<numsect;c++)
  {
   //if (c == 0)
   //{
   // gb_ptrBeginDataDisc[d] = contBuffer; //Donde empieza disco      
   // printf("Disco begin %d Track ",gb_ptrBeginDataDisc[d],d);    
   //}
   discid[d][c][0]=trkhead[0x18+(c<<3)];
   discid[d][c][1]=trkhead[0x19+(c<<3)];
   discid[d][c][2]=trkhead[0x1A+(c<<3)];
   discid[d][c][3]=trkhead[0x1B+(c<<3)];
   //printf("%i %i %i %i  ",discid[d][c][0],discid[d][c][1],discid[d][c][2],discid[d][c][3]);
//JJ   memcpy(&(discdat[d][(discid[d][c][2]-1)&15]),&(gb_list_dsk_data[id][contBuffer]),512);
   gb_ptrBeginDataDisc[d][c]=contBuffer;
   //printf("Begin Track:%d Sector:%d Address:%d\n",d,c,gb_ptrBeginDataDisc[d][c]);
   contBuffer+= 512;
  }  
 }
 //printf ("\n");
 */
}

/*
void loaddsk()
{
        int numsect;
        int c,d;
        char head[40];
        unsigned char dskhead[256],trkhead[256];
        FILE *f=fopen(discname,"rb");
        if (!f) return;
        fread(dskhead,256,1,f);
        for (c=0;c<40;c++) head[c]=0;
        for (c=0;c<0x21;c++) head[c]=dskhead[c];
        disctracks=dskhead[0x30];
//        printf("%i tracks\n",dskhead[0x30]);
//        printf("%s\n",head);
        for (d=0;d<disctracks;d++)
        {
                fread(trkhead,256,1,f);
                //JJ while (strncmp(trkhead,"Track-Info",10) && !feof(f))
                while (strncmp((const char *)trkhead,"Track-Info",10) && !feof(f))
                      fread(trkhead,256,1,f);
//                printf("Track %i ftell %05X : ",d,ftell(f)-256);
                if (feof(f))
                {
                        fclose(f);
                        return;
                }
                discsects[d]=numsect=trkhead[0x15];
//                printf("%i sectors\n",discsects[d]);
                for (c=0;c<numsect;c++)
                {
                        discid[d][c][0]=trkhead[0x18+(c<<3)];
                        discid[d][c][1]=trkhead[0x19+(c<<3)];
                        discid[d][c][2]=trkhead[0x1A+(c<<3)];
                        discid[d][c][3]=trkhead[0x1B+(c<<3)];
//                        printf("%i %i %i %i  ",discid[d][c][0],discid[d][c][1],discid[d][c][2],discid[d][c][3]);
//JJ                        fread(discdat[d][(discid[d][c][2]-1)&15],512,1,f);
                }
//                printf("\n");
        }
//        printf("DSK pos %i\n",ftell(f));
        fclose(f);
}
*/

unsigned char readfdc(unsigned short addr)
{
        unsigned char aux_discdat/*[11]*/[1024];
        int c;
        unsigned char temp;
//        printf("Read %04X  %04X\n",addr,pc);
        if (addr&1)
        {
                if (!readparams)
                {
                        //printf("Reading but no params - last command %02X\n",command);
                        exit(-1);
                }
                switch (command)
                {
                        case 0x04: //Sense drive status
                        readparams=0;
                        fdcstatus=0x80;
                        return st3;

                        case 0x06: //Read sectors
                        if (reading)
                        {

                                temp=discdat->datos[fdctrack][startsector-1][posinsector];                                
//                                printf("Read track %i sector %i pos %i\n",fdctrack,startreal,posinsector);
//                                printf("%c",temp);
                                posinsector++;
                                if (posinsector==512)
                                {
                                        if ((startsector&15)==(endsector&15))
                                        {
                                                reading=0;
                                                readparams=7;
//                                                output=1;
                                                fdcstatus=0xD0;
//                                                if (startsector==4) output=1;
//                                                printf("Done it %04X\n",pc);
                                                endread=1;
                                                fdcint=1;
                                                discon=0;
//                                                output=1;
//                                                dumpregs();
//                                                dumpram();
//                                                exit(-1);
                                        }
                                        else
                                        {
                                                posinsector=0;
                                                startsector++;
                                                //JJ if ((startsector&15)==(discsects[fdctrack]+1))
                                                if ((startsector&15)==(discsects->datos[fdctrack]+1))
                                                {
                                                        if (command&0x80)
                                                           fdctrack++;
                                                        startsector=0xC1;
                                                }
                                                startreal=0;
                                                for (c=0;c<11;c++)
                                                {
                                                        if ((discid->datos[starttrack][c][2]&15)==(startsector&15))
                                                        {
                                                                startreal=c;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                return temp;
                        }
                        readparams--;
                        switch (readparams)
                        {
                                case 6: st0=0x40; st1=0x80; st2=0; return st0;
                                case 5: return st1;
                                case 4: return st2;
                                case 3: return fdctrack;
                                case 2: return 0;
                                case 1: return startsector;
                                case 0: fdcstatus=0x80; return 2;
                        }
                        break;
                        case 0x86: /*Read sector fail*/
                        readparams--;
                        switch (readparams)
                        {
                                case 6: st0=0x40; st1=0x84; st2=0; return st0;
                                case 5: return st1;
                                case 4: return st2;
                                case 3: return fdctrack;
                                case 2: return 0;
                                case 1: return startsector;
                                case 0: fdcstatus=0x80; return 2;
                        }
                        break;

                        case 0x08: /*Sense interrupt state*/
                        readparams--;
                        if (readparams==1)
                           return st0;
                        fdcstatus=0x80;
                        return fdctrack;

                        case 0x0A: /*Read sector ID*/
                        readparams--;
                        switch (readparams)
                        {
                                case 6: return st0;
                                case 5: return st1;
                                case 4: return st2;
                                case 3: return discid->datos[fdctrack][startsector][0];
                                case 2: return discid->datos[fdctrack][startsector][1];
                                case 1: return discid->datos[fdctrack][startsector][2];
                                case 0: fdcstatus=0x80; return discid->datos[fdctrack][startsector][3];
                        }
                        break;

                        default:
                        //printf("Reading command %02X\n",command);
                        exit(-1);
                }
        }
        else
        {
//                if (reading)
//                   fdcstatus^=0x80;
                return fdcstatus;
        }
}

void writefdc(unsigned short addr, unsigned char val)
{
        int c;
//        printf("Write %04X %02X  %04X\n",addr,val,pc);
        if (addr==0xFA7E)
        {
                //JJ motoron=val&1;//No necesito estado motor disco
                return;
        }
        if (addr&1)
        {
                if (params)
                {
                        paramdat[params-1]=val;
                        params--;
                        if (!params)
                        {
                                switch (command)
                                {
                                        case 0x03: /*Specify*/
//                                        printf("Specified %02X %02X\n",paramdat[1],paramdat[0]);
                                        fdcstatus=0x80;
                                        break;

                                        case 0x04: /*Sense drive status*/
                                        st3=0x60;
                                        if (!fdctrack) st3|=0x10;
                                        fdcstatus=0xD0;
                                        readparams=1;
                                        break;

                                        case 0x06: /*Read sectors*/
//                                        printf("Read sectors %02X %02X %02X %02X %02X %02X %02X %02X\n",paramdat[7],paramdat[6],paramdat[5],paramdat[4],paramdat[3],paramdat[2],paramdat[1],paramdat[0]);
                                        starttrack=paramdat[6];
                                        startsector=paramdat[4]&15;
                                        endsector=paramdat[2]&15;
                                        startreal=0;
/*                                        for (c=0;c<11;c++)
                                        {
                                                printf("Sector %i ID %02X\n",c,discid[starttrack][c][2]);
                                                if (discid[starttrack][c][2]==paramdat[4])
                                                {
                                                        startreal=c;
                                                        break;
                                                }
                                        }
                                        if (c==11)
                                        {
                                                printf("Sector %02X not found on track %02X\n",startsector,starttrack);
                                                command=0x86;
                                                reading=0;
                                                readparams=7;
                                                fdcstatus=0xD0;
//                                                exit(-1);
                                        }
                                        else
                                        {*/
//                                        printf("FDC read %02X %02X %i %i %i\n",paramdat[4],endsector,startreal,c,starttrack);
//                                        printf("FDC Read sector track %i start %i end %i\n",starttrack,startsector,endsector);
                                        posinsector=0;
                                        readparams=1;
                                        reading=1;
                                        fdcstatus=0xF0;
//                                        }
//                                        printf("Start - track %i sector %i\n",starttrack,startsector);
                                        break;

                                        case 0x07: /*Recalibrate*/
//                                        printf("Recalibrate %02X\n",paramdat[0]);
                                        fdcstatus=0x80;
                                        fdctrack=0;
                                        fdcint=1;
                                        break;

                                        case 0x0A: /*Read sector ID*/
                                        fdcstatus|=0x60;
                                        readparams=7;
                                        break;

                                        case 0x0F: /*Seek*/
//                                        printf("Seek %02X %02X\n",paramdat[1],paramdat[0]);
                                        fdcstatus=0x80;
                                        fdctrack=paramdat[0];
                                        fdcint=1;
//                                        output=1;
                                        break;

                                        default:
                                        //printf("Executing bad command %02X\n",command);
                                        exit(-1);
                                }
                        }
                }
                else
                {
                        command=val&0x1F;
                        switch (command)
                        {
                                case 0: case 0x1F: return; /*Invalid*/
                                case 0x03: /*Specify*/
                                params=2;
                                fdcstatus|=0x10;
                                break;

                                case 0x04: /*Sense drive status*/
                                params=1;
                                fdcstatus|=0x10;
                                break;

                                case 0x06: /*Read sectors*/
//                                if (output) exit(0);
                                params=8;
                                fdcstatus|=0x10;
                                discon=1;
                                break;

                                case 0x07: /*Recalibrate*/
                                params=1;
                                fdcstatus|=0x10;
                                break;

                                case 0x08: /*Sense interrupt state*/
                                st0=0x21;
                                if (!fdcint) st0|=0x80;
                                else         fdcint=0;
                                fdcstatus|=0xD0;
                                readparams=2;
                                break;

                                case 0x0A: /*Read sector ID*/
                                params=1;
                                fdcstatus|=0x10;
                                break;

                                case 0x0F: /*Seek*/
                                params=2;
                                fdcstatus|=0x10;
                                break;

                                default:
                                //printf("Starting bad command %02X\n",command);
                                exit(-1);
                        }
                }
        }
}
