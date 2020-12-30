#ifndef _GBDSK_H
 #define _GBDSK_H
 

 #include "dsk/dskCorsairTrainer.h"
 #include "dsk/dskDragonAttack.h"
 #include "dsk/dskBabasPalace.h"
 #include "dsk/dskAmsthrees.h" 
 #include "dsk/dskSokoban.h"
 #include "dsk/dskMaxDeskt.h"
 #include "dsk/dskAmxMouse1.h"
 #include "dsk/dskAmxMouse2.h"

 //#include "dsk/dskSokoban.h"
 //#include "dsk/dskPrinceOfPersia.h"
 //#include "dsk/dskBatmangroupDemo.h"
 //#include "dsk/dskBatmangroupDemo2.h"
 //#include "dsk/dskBatmangroupDemo3.h"
 //#include "dsk/dskKraftwerkAutobahn.h"
// #include "dsk/dskMadmix.h"
 /*#include "dsk/dskAbadia.h"
 #include "dsk/dskCybernoid.h"
 #include "dsk/dsksaboteur.h"
 #include "dsk/dskAtlantis.h"
 #include "dsk/dskGameover.h"
 #include "dsk/dskMeganova.h"
 #include "dsk/dskEliminator.h"
 #include "dsk/dskMazemania.h" */
 
 //#define max_list_dsk 10
 #define max_list_dsk 8
 
 static const char * gb_list_dsk_title[max_list_dsk]={  
  "Corsair Trainer",
  "Dragon Attack",
  "Babas Palace",
  "Amsthrees",       
  "Sokoban",
  "Max Deskt",
  "AMX Mouse 1",
  "AMX Mouse 2"  
  //"Batman Group",
  //"Batman Group2",
  //"Batman Group3",
 // "Prince Of Persia",
 // "Sokoban"
 /*,
  "Abadia",
  "Cybernoid",
  "Saboteur",
  "Atlantis",
  "Game Over",
  "Meganova",
  "Eliminator",
  "Maze Mania"  */
  //"Kraftwerk Autobahn"//,
  
  //"Madmix"
 };
 
 static const int gb_list_dsk_size[max_list_dsk]={
  194816,
  194816,
  204544,
  204544,
  194816,
  194816,
  194816,
  194816    
  //225536,
  //225536
  /*,
  225536,
  204544,
  194816,
  194816,
  194816,
  194816,
  194816,
  194816
  //,
  //194816,
  //194816//,*/
  //204544
 };
 
 static const unsigned char * gb_list_dsk_sects[max_list_dsk]={
  gb_dsk_discsects_corsairTrainer,
  gb_dsk_discsects_dragonAttack,
  gb_dsk_discsects_babasPalace,
  gb_dsk_discsects_amsthrees,
  gb_dsk_discsects_sokoban,
  gb_dsk_discsects_maxdeskt,
  gb_dsk_discsects_amxmouse1,
  gb_dsk_discsects_amxmouse2  
  //gb_dsk_discsects_batmangroupdemo,
  //gb_dsk_discsects_batmangroupdemo2,
  //gb_dsk_discsects_batmangroupdemo3,
  //gb_dsk_discsects_princeofpersia,
  //gb_dsk_discsects_sokoban
  /*,
  gb_dsk_discsects_abadia,
  gb_dsk_discsects_cybernoid,  
  gb_dsk_discsects_saboteur,
  gb_dsk_discsects_atlantis,
  gb_dsk_discsects_gameover,
  gb_dsk_discsects_meganova,
  gb_dsk_discsects_eliminator,
  gb_dsk_discsects_mazemania  
  //gb_dsk_discsects_kraftwerkAutobahn*/
 }; 

 static const unsigned char * gb_list_dsk_discid[max_list_dsk]={
  gb_dsk_discid_corsairTrainer,
  gb_dsk_discid_dragonAttack,
  gb_dsk_discid_babasPalace,
  gb_dsk_discid_amsthrees,
  gb_dsk_discid_sokoban,
  gb_dsk_discid_maxdeskt,
  gb_dsk_discid_amxmouse1,
  gb_dsk_discid_amxmouse2,  
  //gb_dsk_discid_batmangroupdemo,
  //gb_dsk_discid_batmangroupdemo2,
  //gb_dsk_discid_batmangroupdemo3,
  //gb_dsk_discid_princeofpersia,
  //gb_dsk_discid_sokoban
  /*,
  gb_dsk_discid_abadia,
  gb_dsk_discid_cybernoid,
  gb_dsk_discid_saboteur,
  gb_dsk_discid_atlantis,
  gb_dsk_discid_gameover,
  gb_dsk_discid_meganova,
  gb_dsk_discid_eliminator,
  gb_dsk_discid_mazemania,  
  //gb_dsk_discid_kraftwerkAutobahn*/
 };
 
 static const unsigned char * gb_list_dsk_discdat[max_list_dsk]={
  gb_dsk_discdat_corsairTrainer,
  gb_dsk_discdat_dragonAttack,
  gb_dsk_discdat_babasPalace,
  gb_dsk_discdat_amsthrees,
  gb_dsk_discdat_sokoban,
  gb_dsk_discdat_maxdeskt,
  gb_dsk_discdat_amxmouse1,
  gb_dsk_discdat_amxmouse2  
  //gb_dsk_discdat_batmangroupdemo,
  //gb_dsk_discdat_batmangroupdemo2,
  //gb_dsk_discdat_batmangroupdemo3,
  //gb_dsk_discdat_princeofpersia,
  //gb_dsk_discdat_sokoban
  /*,
  gb_dsk_discdat_abadia,
  gb_dsk_discdat_cybernoid,
  gb_dsk_discdat_saboteur,
  gb_dsk_discdat_atlantis,
  gb_dsk_discdat_gameover,
  gb_dsk_discdat_meganova,
  gb_dsk_discdat_eliminator,
  gb_dsk_discdat_mazemania  
  //gb_dsk_discdat_kraftwerkAutobahn//,
  //gb_dsk_madmix*/
 };
 
#endif
 
