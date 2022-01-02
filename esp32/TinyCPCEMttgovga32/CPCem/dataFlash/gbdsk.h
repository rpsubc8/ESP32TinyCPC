#ifndef _GBDSK_H
 #define _GBDSK_H
 
 #include "dsk/dskdisco1.h"
 #include "dsk/dskdisco2.h"
 #include "dsk/dskdisco3.h"
 #include "dsk/dskAmxMouse1.h"

 #define max_list_dsk 4

 static const char * gb_list_dsk_title[max_list_dsk]={  
  "disco 1",
  "disco 2",
  "disco 3",
  "AMX Mouse 1"
 };

 static const unsigned char * gb_list_dsk_sects[max_list_dsk]={  
  gb_dsk_discsects_disco1,
  gb_dsk_discsects_disco2,
  gb_dsk_discsects_disco3,
  gb_dsk_discsects_amxmouse1
 };

 static const unsigned char * gb_list_dsk_discid[max_list_dsk]={  
  gb_dsk_discid_disco1,
  gb_dsk_discid_disco2,
  gb_dsk_discid_disco3,
  gb_dsk_discid_amxmouse1
 };

 static const unsigned char * gb_list_dsk_discdat[max_list_dsk]={  
  gb_dsk_discdat_disco1,
  gb_dsk_discdat_disco2,
  gb_dsk_discdat_disco3,
  gb_dsk_discdat_amxmouse1
 };

#endif
