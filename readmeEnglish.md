

# Tiny ESP32 AMSTRAD CPC
<br>
Port from Tom Walker's PC emulator (AMSTRAD CPC) to TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4) board with ESP32.
<br><br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewCPC464.gif'></center>
I have made several modifications:
<ul>
 <li>x86 PC to ESP32 port</li>
 <li>PSRAM is not used, it works in ESP32 with 520 KB of RAM (TTGO VGA32 v1.2)</li> 
 <li>Use of a single core</li>
 <li>Low-income OSD</li>
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>AY8912 emulation (fabgl library) reduced version of <b>dcrespo3d</b>(David Crespo Tascón)</li>
 <li>Screen adjustment X</li>
 <li>Emulation CPU speed menu (AUTO), sound, keyboard</li>
 <li>Support for 8 and 64 color mode (reduced version by Ricardo Massaro).</li>
 <li>DSK support with 42 tracks and 11 sectors</li>
 <li>Emulates only the CRTC 0</li>
 <li>Supports 0, 1 and 2 video mode</li>
 <li>VGA 400x300</li>
 <li>VGA 320x200 with or without borders</li>
 <li>CPC 464 and 664 emulation</li>
 <li>CPC 6128 emulation (unstable)</li>
 <li>AMX Mouse emulation (reduced library by Rob Kent jazzycamel)</li>
 <li>It is now possible to have 400x300 64 color video mode, 128 KB mode, as well as mouse and sound all active.</li>
 <li>Green monochrome palette to simulate GT65</li>
</ul>

  
<br>
<h1>Requirements</h1>
Required:
 <ul>
  <li>TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4)</li>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.1 Espressif32 v3.3.2</li>
  <li>Arduino IDE 1.8.11 Espressif System 1.0.6</li>
  <li>Arduino fabgl 0.9.0 reduced library (included in PLATFORMIO project)</li>
  <li>Arduino bitluni 0.3.3 reduced library (included in project)</li>
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/ttgovga32v12.jpg'></center>
<br>
 
 
<h1>PlatformIO</h1>
PLATFORMIO 2.2.1 must be installed from the Visual Studio extensions. Espressif32 v3.3.2 is also required. 
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewPlatformIOinstall.gif'></center>
Then the working directory <b>TinyCPCEMttgovga32</b> will be selected.
We must modify the file <b>platformio.ini</b> the option <b>upload_port</b> to select the COM port where we have our TTGO VGA32 board.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewPlatformIO.gif'></center>
Then we will proceed to compile and upload to the board. No partitions are used, so we must upload the whole compiled binary. It's all set up so you don't have to install the bitluni and fabgl libraries. 



<br>
<h1>Arduino IDE</h1>
The whole project is compatible with the structure of Arduino 1.8.11.
We only have to open the <b>CPCem.ino</b> from the <b>CPCem</b> directory.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card url manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
The project is now ready, so no bitluni or fabgl libraries are needed. We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and it will restart in loop mode. 



<br>
<h1>Usability</h1>
The following actions are allowed from the menu (key F1):
 <ul>
  <li>Select Machine allows you to choose CPC 464, 664 or 6128.</li>
  <li>Select DSK allows to choose the disk.</li>
  <li>X screen offset</li> 
  <li>Allows you to skip a frame</li>
  <li>Change the milliseconds of polling for video, keyboard and sound</li>
  <li>Change the milliseconds of waiting in each frame</li>
  <li>Sound Volume (100%, 75,%, 50%, 25%, 5%)</li>
  <li>Sound active or muted</li>
  <li>CPU wait in AUTO mode (set 20 ms per real frame) or the wait in ms of your choice</li>
  <li>Mouse Detect, allows to re-initialize the mouse, if it has been disconnected while hot</li>
  <li>Mouse ON and OFF, allows you to disable the mouse reading. This way the emulation is faster.</li> 
 </ul>
 There is a basic OSD of low resources, that is to say, very simple, that is visualized by pressing the key <b>F1</b>.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewOSD.gif'></center>
 Files must be converted to .h in hexadecimal. You can use the online tool:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a>
 The DSK are in DUMP format to save memory, and you must use the dsk2h tool to generate the necessary .h.

 
 
<br>
<h1>Options</h1>
The file <b>gbConfig.h</b> options are selected:
<ul>
 <li><b>use_lib_400x300:</b> 400x300 video mode is used.</li>
 <li><b>use_lib_320x200_video_border:</b> 320x200 video mode with borders is used, so the screen size is halved. This mode consumes less RAM than 400x300 and is faster.</li>
 <li><b>use_lib_320x200_video_noborder:</b> 320x200 video mode without borders is used. In this mode the screen is not halved, but the borders are lost. This mode consumes less RAM than 400x300 and is faster.</li>
 <li><b>use_lib_vga8colors:</b> Forces to use RGB 8-color mode (3 pins). Outputs 8 colors, as opposed to 64 in normal mode (6 pins RRGGBB).</li>
 <li><b>use_lib_sound_ay8912:</b> It uses a 3-channel mixer in dirty mode, emulating the AY8912. It consumes some RAM. It requires the reduced fabgl 0.9.0 library, already included in the project.</li>
 <li><b>use_lib_log_serial:</b> Logs are sent by serial port usb</li>
 <li><b>usb_lib_optimice_checkchange_bankswitch:</b> Only switches banks when they are different, gaining speed.</li>
 <li><b>use_lib_128k:</b> Allows to use 128K mode, including 6128 rom at compilation, as well as 2 blocks of 64KB memory. It is in the testing phase, and as it requires more RAM, you have to use other low RAM consumption configuration options.</li>
 <li><b>use_lib_cheat_128k:</b> 128 KB experimental mode.</li> 
 <li><b>gb_ms_keyboard:</b> You must specify the number of milliseconds of polling for the keyboard.</li>
 <li><b>gb_ms_sound:</b> You must specify the number of milliseconds of polling for the sound.</li>
 <li><b>gb_frame_crt_skip:</b> If it is 1 it skips a frame.</li>
 <li><b>gb_delay_emulate_ms:</b> Millisecond wait for each completed frame.</li>
 <li><b>use_lib_amx_mouse:</b> A PS/2 mouse is used as if it were an AMX mouse (emulated). The use of the mouse consumes some CPU and memory.</li>
 <li><b>gb_delay_init_ms:</b> Specifies a number of initial milliseconds to wait for the mouse to start up in order to detect it correctly. </li>
 <li><b>use_lib_amx_mouse_lefthanded:</b> Left-handed mouse (swaps buttons)</li>
 <li><b>gb_ms_mouse:</b> Sampling in milliseconds of each mouse reading.</li>
 <li><b>use_lib_lookup_znptable:</b> Uses look-up table in FLASH for BCD calculations. It consumes 256 bytes.</li>
 <li><b>use_lib_lookup_znptable16:</b> Uses look-up table in FLASH for BCD calculations. Consumes 64 KBs.</li> 
</ul>



<br>
<h1>Test applications</h1>
When starting the ESP32, it is run in CPC464 mode by loading the BASIC.
We can choose the following games:
<ul>
 <li><b>Corsair Trainer</b></li>
 <li><b>Dragon Attack</b></li>
 <li><b>Babas Palace</b></li>
 <li><b>Amsthrees</b></li>
 <li><b>Sokoban</b></li>
 <li><b>Max Desk:</b> mouse test</li>
 <li><b>AMX mouse:</b> Image editor with mouse</li> 
</ul>

Everything has been compacted into several discs:
<pre> 
 Disk 1 
  babaspalace
   cod0.bin 40K
   cod1.bin 2K
   disc.bas 1K
   intro.bin 9K
   loader.bin 1K
   screen.scr 17K
  amsthree
   amsthre3.scr 17K
   amsthree.bin 38K
   loader.bas 1K
  sokoban
   sokoban.bin 9K
   sokoban.dat 19K
 
 
 Disk 2
  corsair trainer
   code.bin 16K
   codex.bin 16K
   data.bin 17K
   disc.bin 8K
   intro.bin 17K
   menuexo.bin  3K
  amxmousev2
   amx.bas 1K
   art.bin 9K
   art.icn 2K
   char.bin 1K
   colhats.bin 1K
   demloa.bin 1K
   demo.rs 9K
   icondes.bas 10K
   monmc.bin 1K
   mspr.bin 2K 
   ocode.bin 1K
   pat1.pcn 1K
   patdes.bas 8K
   pgen.bas 11K
   rom.icn 2K
   rsxb.bin 2K
   scrload.bas 2K
   vdutab.o 1K


 Disk 3
  Max desk hide
   maxdesk 1k
   maxdesk.n01 1k
   maxdesk.n02 34k
   maxdesk.n04 17k
   maxdesk.n05 2k
   sysvars.bin 1k
   dragon attack
   block1.bin 5K
   block2.bin 4K
   block3.bin 5K
   block4.bin 5K
   block5.bin 5K
   block6.bin 5K
   block7.bin 5K
   block8.bin 5K
   code.bin 28K
   disc.bin 1k
   disc-ni.bin 1k
</pre>
<br><br>

<br>
<h1>AMX Mouse</h1>
In order to use a mouse as if it were an AMX Mouse, it is required to activate the support in the configuration file.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewAmxMouse.gif'></center>
Left-handed mouse configuration is allowed (also from the OSD), as well as sampling times, all in the <b>gbConfig.h</b> file.

<br><br>
<h1>DIY circuito</h1>
Si no queremos usar una placa TTGO VGA32 v1.x, podemos construirla siguiendo el esquema de fabgl:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/fabglcircuit.gif'></center>



<br><br>
<h1>Tool dsk2h</h1>
I have created a very basic tool, to convert .dsk files to .h, as well as roms in ready mode to be processed by the emulator. We just need to leave the .dsk files in the <b>dsks</b> folder, as well as .rom in <b>romextra</b> and run the file <b>dsk2h.exe</b>, so that an output will be generated in the directory <b>dataFlash</b>. For testing purposes, 2 files have been left in the <b>dsks</b> directory, which it is recommended to delete in case a new custom list is made. It is also recommended to delete the files in the directory <b>CPCem\dataFlash\dsk</b> to have a clean project.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyCPC/tree/main/tools/dsk2h'>Tool dsk2h</a>
<br><br>
<pre>
 input/
  dsks/
  romextra/
 output/ 
  dataFlash/
   dsk/
   romextra/
</pre>
Then we must copy the directory <b>dataFlash</b> into the project <b>TinyCPCEMttgovga32\CPCem</b> overwriting the previous dataFlash folder. It is recommended to clean up the project and recompile it.<br>
This tool is very simple, and does not control errors, so it is recommended to leave you the files with very simple names and as simple as possible.<br>
The project in PLATFORM.IO is prepared for 2 MB of Flash. If we need the 4MB of flash, we will have to modify the entry in the file <b>platform.ini</b>
<pre>board_build.partitions = huge_app.csv</pre>
In the Arduino IDE, we must choose the option <b>Partition Scheme (Huge APP)</b>.


<br><br>
<h1>Load ROMS</h1>
Games in ROM format (16 KB), can be loaded in LOW and HIGH (slot 0 to 15). When the <b>Run ROM</b> option is selected, the name of the ROM to be invoked as RSX command is autowritten. This RSX command matches the name of the rom file that was generated, so we must give it the exact .ROM file name that is invoked.
<br>
When we want to load another game, we must reset, either by selecting the Machine menu or Reset.
<br>
If we want to launch the game <b> arkanoid </b>, we must choose from the menu <b> Load or Run ROM </b>, then <b> Run ROM </b> and finally it will ask us to select <b> Arkanoid </b>. At the end, we select <b> High ROM (0..15) </b>, where we put for example slot 1, and in a couple of seconds, <b> | arkanoid </b> will be written.
<br>
If we choose just <b> Load ROM </b>, it will do the same as above, but without launching the RSX <b> | arkanoid </b>. This file name must match the actual name of the internal game.
<br>
If we have several ROMS games, for example <b> 3weeks.rom and 3weeks2.rom </b>, we must load 3weeks.rom in slot 1 and 3weeks2.rom in slot 2, and finally launch the main one, which in this case would be <b> 3weeks.rom </b>
<br><br>
More information about ROMS:
<a href='https://www.cpcwiki.eu/index.php/ROM_List'>https://www.cpcwiki.eu/index.php/ROM_List</a>


<br><br>
<h1>Monochrome</h1>
In 64-color mode, a choice of 3 types of green monochrome color palette has been added. Since the DAC is 6-bit, only 11 levels of green are allowed in addition to black.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/paletaVerde.gif'></center>
Therefore, we cannot achieve the same quality as a GT65 monitor.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/wecLemans.gif'></center>


<br><br>
<h1>CPM</h1>
To execute the CPM Operating System, the DSK is required (not included in this project), and press the key |.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewCPM.gif'></center>
The <b>Tilde</b> key is taken out with the combination <b>SHIFT</b> + <b>F10</b>.
