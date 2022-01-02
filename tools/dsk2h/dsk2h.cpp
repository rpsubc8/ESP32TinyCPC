//Author: ackerman
//Convert dsks .dsk to .h and .rom to .h Tiny ESP32 CPC emulator
//Longitud de titulo de ficheros maxima 12 caracteres
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define maxDSK 255

#define fichDSK 0
#define fichROM 1

#define maxLengthFileName 12

unsigned char gbTipoFichero= fichDSK;

unsigned char discsects[40];
unsigned char discid[42][11][4];
unsigned char discdat[42][11][512];
unsigned char gb_datROMEXTRA[65535]; //El maximo es 16384 16 KB
int disctracks;


//unsigned char gb_bufferFile[524288]; //512 KB archivo maximo

void listFilesRecursively(char *path);
void ShowHelp(void);
void WriteHeadH_DSK(char *titleArray, unsigned char totalTitle);
void WriteHeadH_ROMEXTRA(char *titleArray, unsigned char totalTitle);
void WriteFileDSK(unsigned char num,char *cadPath, char *cadFile);
void WriteFileROMEXTRA(unsigned char num,char *cadPath, char *cadFile, int aSize);
int loadROMEXTRA(char *cadPath,char *discname);
void WriteHexROMEXTRA(FILE *fileOut,char *cadFile,int aSize);
//int GetSizeFile(char *cadFile);
void InitTitles(void);
void loaddsk(char *cadPath,char *discname);
void ClearDSK(void);
void WriteHexSects(FILE *fileOut,char *cadFile);
void WriteHexDiscid(FILE *fileOut,char *cadFile);
void WriteHexDiscdat(FILE *fileOut,char *cadFile);
void RemoveExt(char *cad);

FILE *gb_fileWrite = NULL;
char gb_titles[maxDSK][32];
unsigned char gb_contRom=0;

void RemoveExt(char *cad)
{
 int total= strlen(cad);
 if (total>maxLengthFileName)
 {
  total= maxLengthFileName;
  cad[maxLengthFileName]='\0';
 }
 for (int i=0;i<total;i++)
 {
  if (cad[i]==' ')   
   cad[i] = '_';
  else
  {
   if (cad[i]=='.')
   {
    cad[i]='\0';
    return;
   }
  }
 }
}

//*****************************
void ClearDSK()
{
 int i,j,k;
 memset(discsects,0,40);
 for (i=0;i<42;i++)
  for (j=0;j<11;j++)
   for (k=0;k<42;k++)
    discid[i][j][k]=0;
 for (i=0;i<42;i++)
  for (j=0;j<11;j++)
   for (k=0;k<512;k++)
    discid[i][j][k]=0;    
}

//*****************************
void loaddsk(char *cadPath,char *discname)
{
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,discname); 
      
 int numsect;
 int c,d;
 char head[40];
 unsigned char dskhead[256],trkhead[256];
 FILE *f=fopen(cadOri,"rb");
 if (!f) return;
 fread(dskhead,256,1,f);
 for (c=0;c<40;c++) head[c]=0;
 for (c=0;c<0x21;c++) head[c]=dskhead[c];
 disctracks=dskhead[0x30];
 for (d=0;d<disctracks;d++)
 {
  fread(trkhead,256,1,f);
  while (strncmp((const char *)trkhead,"Track-Info",10) && !feof(f))
   fread(trkhead,256,1,f);
  if (feof(f))
  {
   fclose(f);
   return;
  }
  discsects[d]=numsect=trkhead[0x15];
  for (c=0;c<numsect;c++)
  {
   discid[d][c][0]=trkhead[0x18+(c<<3)];
   discid[d][c][1]=trkhead[0x19+(c<<3)];
   discid[d][c][2]=trkhead[0x1A+(c<<3)];
   discid[d][c][3]=trkhead[0x1B+(c<<3)];
   fread(discdat[d][(discid[d][c][2]-1)&15],512,1,f);
  }
 }
 fclose(f);
}


//**********************************************
void InitTitles()
{
 for (unsigned char i=0;i<maxDSK;i++)
  gb_titles[i][0]='\0';
}

//**********************************************
void ShowHelp()
{
 printf("Author: ackerman\n\n");
}

//**********************************************
//int GetSizeFile(char *cadFile)
//{
// long aReturn=0;
// FILE* fp = fopen(cadFile,"rb");
// if(fp) 
// {
//  fseek(fp, 0 , SEEK_END);
//  aReturn = ftell(fp);
//  fseek(fp, 0 , SEEK_SET);// needed for next read from beginning of file
//  fclose(fp);
// }
// return aReturn;
//}


//**********************************************
void WriteHexSects(FILE *fileOut,char *cadFile)
{//Escribe sectores
 int i;
 int cont=0;
 fprintf(fileOut,"const unsigned char gb_dsk_discsects_%s[]={\n",cadFile);
 for (int i=0;i<40;i++)
 {
  fprintf(fileOut,"0x%02x",discsects[i]);
  if (i<(40-1))
   fprintf(fileOut,",");  
  cont++;
  if (cont>15) {cont=0; fprintf(fileOut,"\n");}
 }
 fprintf(fileOut,"\n};\n\n");    
}

//**********************************************
void WriteHexDiscid(FILE *fileOut,char *cadFile)
{//Escribe sectores
 int i;
 int cont=0;
 int tope=(42*11*4)-1;
 int contInterno=0;
 fprintf(fileOut,"const unsigned char gb_dsk_discid_%s[]={\n",cadFile);
 for (int i=0;i<42;i++)
  for (int j=0;j<11;j++)
   for (int k=0;k<4;k++)
   {
    fprintf(fileOut,"0x%02x",discid[i][j][k]);
    if (contInterno<tope)
     fprintf(fileOut,",");
    contInterno++; 
    cont++;
    if (cont>15) {cont=0; fprintf(fileOut,"\n");}
   } 
 fprintf(fileOut,"\n};\n\n");    
}

//**********************************************
void WriteHexDiscdat(FILE *fileOut,char *cadFile)
{//Escribe sectores
 int i;
 int cont=0;
 int tope=(42*11*512)-1;
 int contInterno=0;
 fprintf(fileOut,"const unsigned char gb_dsk_discdat_%s[]={\n",cadFile);
 for (int i=0;i<42;i++)
  for (int j=0;j<11;j++)
   for (int k=0;k<512;k++)
   {
    fprintf(fileOut,"0x%02x",discdat[i][j][k]);
    if (contInterno<tope)
     fprintf(fileOut,",");
    contInterno++; 
    cont++;
    if (cont>15) {cont=0; fprintf(fileOut,"\n");}
   } 
 fprintf(fileOut,"\n};\n\n");    
}


//**********************************************
void WriteFileDSK(unsigned char num,char *cadPath,char *cadFile)
{
 unsigned char contLine=0;
 FILE *auxWrite = NULL;
 FILE *auxRead = NULL;
 long auxSize=0;
 char cadDest[2048];
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,cadFile);
 sprintf(cadDest,"output//dataFlash//dsk//dsk%s.h",cadFile);
 auxWrite = fopen(cadDest,"w+");
 if (auxWrite!= NULL)
 {
  fprintf(auxWrite,"#ifndef DSK_%s_H\n",cadFile);
  fprintf(auxWrite," #define DSK_%s_H\n",cadFile);   
  fprintf(auxWrite,"//DSK %s\n\n",cadFile);
  
  WriteHexSects(auxWrite,cadFile);
  WriteHexDiscid(auxWrite,cadFile);
  WriteHexDiscdat(auxWrite,cadFile);
  
  fprintf(auxWrite,"#endif\n");
  
/*  fprintf(auxWrite,"const unsigned char gb_rom_%s_data[]={\n",cadFile);
  //printf ("Cadori %s\n",cadOri);
  auxRead = fopen(cadOri,"rb");
  if (auxRead!=NULL)
  {
   auxSize = GetSizeFile(cadOri);
   //printf ("Tam %d",auxSize);
   fread(gb_bufferFile,1,auxSize,auxRead);
   for (long i=0;i<auxSize;i++)
   {
    fprintf(auxWrite,"0x%02X",gb_bufferFile[i]);
    if (i<(auxSize-1))
     fprintf(auxWrite,",");
    contLine++;
    if (contLine>15)
    {
     contLine=0;
     fprintf(auxWrite,"\n");
    }
   }
   fclose(auxRead);
  }
  
  fprintf(auxWrite,"};\n");
*/  
  fclose(auxWrite);
 }
}

//**************************************************
int sizeFile(char *cadFile)
{
 int aReturn=0;
 FILE *f=fopen(cadFile,"rb");
 if (f!= NULL)
 {
  fseek(f, 0, SEEK_END);
  aReturn = ftell(f);
  fseek(f, 0, SEEK_SET); 
  fclose(f);
 }
 return aReturn;
}

//**************************************************
int loadROMEXTRA(char *cadPath,char *discname)
{
 int aReturnSize=0;
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,discname); 

 aReturnSize= sizeFile(cadOri);
      
 FILE *f=fopen(cadOri,"rb");
 if (!f) return 0;
 //fread(gb_datROMEXTRA,16384,1,f);
 fread(gb_datROMEXTRA,aReturnSize,1,f);
 fclose(f);
 
 return aReturnSize;
}

//********************************************************
void WriteHexROMEXTRA(FILE *fileOut,char *cadFile, int aSize)
{//Escribe HEX ROMEXTRA
 int i;
 int cont=0;
 //int tope=16384;
 int tope=aSize;
 int contInterno=0;
 fprintf(fileOut,"const unsigned char gb_romextra_dat_%s[]={\n",cadFile);
 for (int i=0;i<tope;i++)
 {
  fprintf(fileOut,"0x%02x",gb_datROMEXTRA[i]);
  if (contInterno<tope)
   fprintf(fileOut,",");
  contInterno++; 
  cont++;
  if (cont>15) {cont=0; fprintf(fileOut,"\n");}
 }
 fprintf(fileOut,"\n};\n\n");    
}

//**********************************************
void WriteFileROMEXTRA(unsigned char num,char *cadPath,char *cadFile, int aSize)
{
 unsigned char contLine=0;
 FILE *auxWrite = NULL;
 FILE *auxRead = NULL;
 long auxSize=0;
 char cadDest[2048];
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,cadFile);
 sprintf(cadDest,"output//dataFlash//romextra//romextra%s.h",cadFile);
 auxWrite = fopen(cadDest,"w+");
 if (auxWrite!= NULL)
 {
  fprintf(auxWrite,"#ifndef ROMEXTRA_%s_H\n",cadFile);
  fprintf(auxWrite," #define ROMEXTRA_%s_H\n",cadFile);   
  //fprintf(auxWrite,"//ROMEXTRA %s 16384 bytes\n\n",cadFile);
  fprintf(auxWrite,"//ROMEXTRA %s %d bytes\n\n",cadFile,aSize);
  
  WriteHexROMEXTRA(auxWrite,cadFile,aSize);
  //WriteHexSects(auxWrite,cadFile);
  //WriteHexDiscid(auxWrite,cadFile);
  //WriteHexDiscdat(auxWrite,cadFile);
  
  fprintf(auxWrite,"#endif\n");
  fclose(auxWrite);
 }
}

//**********************************************
void WriteHeadH_ROMEXTRA(char *titleArray, unsigned char totalTitle)
{
 if (gb_fileWrite == NULL)
  return;
 fprintf(gb_fileWrite,"#ifndef _GBROMEXTRA_H\n");
 fprintf(gb_fileWrite," #define _GBROMEXTRA_H\n"); 
 fprintf(gb_fileWrite,"\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) != 0)
   fprintf(gb_fileWrite," #include \"romextra/romextra%s.h\"\n",gb_titles[i]);
 }
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," #define max_list_romextra %d\n\n",gb_contRom);
 fprintf(gb_fileWrite," static const char * gb_list_romextra_title[max_list_romextra]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  \"\"");
  else 
   fprintf(gb_fileWrite,"  \"%s\"",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");
 }
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n"); 

 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," static const unsigned char * gb_list_romextra_data[max_list_romextra]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  NULL");
  else 
   fprintf(gb_fileWrite,"  gb_romextra_dat_%s",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");   
 }  
 fprintf(gb_fileWrite,"\n };\n");  
 
 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite,"#endif\n");     
}

//**********************************************
void WriteHeadH_DSK(char *titleArray, unsigned char totalTitle)
{//Cabecera 255 DSKs
 if (gb_fileWrite == NULL)
  return;
 fprintf(gb_fileWrite,"#ifndef _GBDSK_H\n");
 fprintf(gb_fileWrite," #define _GBDSK_H\n"); 
 fprintf(gb_fileWrite,"\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) != 0)
   fprintf(gb_fileWrite," #include \"dsk/dsk%s.h\"\n",gb_titles[i]);
 }
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," #define max_list_dsk %d\n\n",gb_contRom);
 fprintf(gb_fileWrite," static const char * gb_list_dsk_title[max_list_dsk]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  \"\"");
  else 
   fprintf(gb_fileWrite,"  \"%s\"",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");
 }
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," static const unsigned char * gb_list_dsk_sects[max_list_dsk]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  NULL");
  else 
   fprintf(gb_fileWrite,"  gb_dsk_discsects_%s",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");   
 }  
 fprintf(gb_fileWrite,"\n };\n");
 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," static const unsigned char * gb_list_dsk_discid[max_list_dsk]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  NULL");
  else 
   fprintf(gb_fileWrite,"  gb_dsk_discid_%s",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");   
 }  
 fprintf(gb_fileWrite,"\n };\n");
 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," static const unsigned char * gb_list_dsk_discdat[max_list_dsk]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  NULL");
  else 
   fprintf(gb_fileWrite,"  gb_dsk_discdat_%s",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");   
 }  
 fprintf(gb_fileWrite,"\n };\n");  
 
 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite,"#endif\n");
}

//**********************************************
void listFilesRecursively(char *basePath)
{
    int auxSizeFileROM=0;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);    
    if (!dir)
        return; // Unable to open directory stream
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);

            switch (gbTipoFichero)
            {
             case fichDSK: 
              ClearDSK();
              loaddsk(basePath,dp->d_name);
              break;
             case fichROM:
              memset(gb_datROMEXTRA,0,65535); //Reset buffer rom
              auxSizeFileROM = loadROMEXTRA(basePath,dp->d_name);
              break;
            }
            RemoveExt(dp->d_name);            
            strcpy(gb_titles[gb_contRom],dp->d_name);
            switch (gbTipoFichero)
            {
             case fichDSK: WriteFileDSK(gb_contRom,basePath,dp->d_name); break;
             case fichROM: WriteFileROMEXTRA(gb_contRom,basePath,dp->d_name,auxSizeFileROM); break;
            }
            
                        
            gb_contRom++;
            if (gb_contRom > (maxDSK-1))
             return;
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            listFilesRecursively(path);
        }
    }
    closedir(dir);
}

//********
//* MAIN *
//********
int main(int argc, char**argv)
{
 char path[100];    //Directory path to list files
 ShowHelp();

 //Procesar DSKS
 InitTitles();
 gb_fileWrite = fopen("output//dataFlash//gbdsk.h","w+");
 if (gb_fileWrite!= NULL)
 {
  printf("DSK files\n");
  gbTipoFichero= fichDSK;
  gb_contRom= 0; //Reset contador ficheros
  listFilesRecursively("input//dsks");
  WriteHeadH_DSK(NULL,gb_contRom);
  fclose(gb_fileWrite);
 }
 
 //Procesar ROMS EXTRA
 InitTitles();
 gb_fileWrite = fopen("output//dataFlash//gbromextra.h","w+");
 if (gb_fileWrite!= NULL)
 {
  printf("\nROMEXTRA files\n");
  gbTipoFichero= fichROM;                      
  gb_contRom= 0; //Reset contador ficheros
  listFilesRecursively("input//romextra");
  WriteHeadH_ROMEXTRA(NULL,gb_contRom);
  fclose(gb_fileWrite);
 } 
 
  //printf("Enter path to list files: ");    // Input path from user
  //scanf("%s", path);
  //listFilesRecursively(path);
 
 return 0;
}
