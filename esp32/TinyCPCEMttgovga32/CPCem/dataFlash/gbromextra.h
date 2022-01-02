#ifndef _GBROMEXTRA_H
 #define _GBROMEXTRA_H

 #include "romextra/romextraastro.h"
 #include "romextra/romextraahhh.h"
 #include "romextra/romextraairwolf.h"
 #include "romextra/romextraanarchy.h"
 #include "romextra/romextraarkanoid.h"
 #include "romextra/romextraaxiens.h"
 #include "romextra/romextrabraxx.h"
 #include "romextra/romextrabrucelee.h"

 #define max_list_romextra 8

 static const char * gb_list_romextra_title[max_list_romextra]={
  "astro",
  "ahhh",
  "airwolf",
  "anarchy",
  "arkanoid",
  "axiens",
  "braxx",
  "brucelee"
 };


 static const unsigned char * gb_list_romextra_data[max_list_romextra]={
  gb_romextra_dat_astro,
  gb_romextra_dat_ahhh,
  gb_romextra_dat_airwolf,
  gb_romextra_dat_anarchy,
  gb_romextra_dat_arkanoid,
  gb_romextra_dat_axiens,
  gb_romextra_dat_braxx,
  gb_romextra_dat_brucelee
 };

#endif
