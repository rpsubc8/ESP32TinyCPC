

# Tiny ESP32 AMSTRAD CPC
Port from Tom Walker's PC emulator (AMSTRAD CPC) to TTGO VGA32 v1.2 board with ESP32.
<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewCPC464.gif'></center>
I have made several modifications:
<ul>
 <li>x86 PC to ESP32 port</li>
 <li>PSRAM is not used, it works in ESP32 with 520 KB of RAM (TTGO VGA32)</li> 
 <li>Use of a single core</li>
 <li>Low-income OSD</li>
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>Emulation of AY8912</li>
 <li>Screen adjustment X</li>
 <li>Emulation video speed menu, sound, keyboard</li>
 <li>Support for 8 and 64 color mode</li>
 <li>DSK support with 42 tracks and 11 sectors</li>
 <li>Emulates only the CRTC 0</li>
 <li>Supports 0, 1 and 2 video mode</li>
 <li>VGA 400x300</li>
 <li>VGA 320x200 with or without borders</li>
 <li>CPC 464 and 664 emulation</li>
 <li>CPC 6128 (unstable)</li>
</ul>

  
<br>
<h1>Requirements</h1>
Required:
 <ul>
  <li>Visual Studio 1.48.1 PLATFORM 2.2.0</li>
  <li>Arduino IDE 1.8.11</li>
  <li>Arduino fabgl version 0.9.0 (PLATFORMIO included)</li>
  <li>Arduino bitluni version 0.3.3 (included)</li>
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/ttgovga32v12.jpg'></center>
<br>
 
 
<h1>PlatformIO</h1>
PLATFORM 2.2.0 must be installed from the Visual Studio extensions.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewPlatformIOinstall.gif'></center>
Then the working directory <b>TinyCPCEMttgovga32</b> will be selected.
We must modify the file <b>platformio.ini</b> the option <b>upload_port</b> to select the COM port where we have our TTGO VGA32 board.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewPlatformIO.gif'></center>
Then we will proceed to compile and upload to the board. No partitions are used, so we must upload all the compiled binary.
Everything is prepared so we don't have to install the bitluni libraries. However, if we need the sound options, we will need to install the <b>fabgl</b> libraries, only for the Arduino IDE, since for PLATFORM I have left the libraries in the project.



<br>
<h1>Arduino IDE</h1>
The whole project is compatible with the structure of Arduino 1.8.11.
We only have to open the <b>CPCem.ino</b> from the <b>CPCem</b> directory.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card url manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewArduinoIDElibrary.gif'></center>
For the normal mode, the project is already prepared, so that no bitluni library is needed. However, if we need the sound support, it is required to install the libraries <b>fabgl 0.9.0</b>.
We can do it from the library manager.
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition at the time of uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and restart it in loop mode.



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
 <li><b>use_lib_vga8colors:</b> Force to use RGB 8 color mode (3 pins). It consumes little ram and is very fast, but only takes 8 colors, compared to 64 in normal mode (6 pins RRGGBB).</li>
 <li><b>use_lib_vga_low_memory:</b> Experimental mode with low video RAM consumption, but slower. 
 <li><b>use_lib_ultrafast_vga:</b> Ultra fast experimental mode, which achieves almost double the video access speed.</li>
 <li><b>use_lib_sound_ay8912:</b> A 3 channel mixer is used in dirty mode, emulating AY8912. It consumes quite a lot of ram. The fabgl library 0.9.0</li> is required</li>
 <li><b>use_lib_log_serial:</b> Logs are sent by serial port usb</li>
 <li><b>usb_lib_optimice_checkchange_bankswitch:</b> Only switches banks when they are different, gaining speed.</li>
 <li><b>use_lib_128k:</b> Allows to use 128K mode, including 6128 rom at compilation, as well as 2 blocks of 64KB memory. It is in the testing phase, and as it requires more RAM, you have to use other low RAM consumption configuration options.</li>
 <li><b>use_lib_cheat_128k:</b> 128 KB experimental mode.</li> 
 <li><b>gb_ms_keyboard:</b> You must specify the number of milliseconds of polling for the keyboard.</li>
 <li><b>gb_ms_sound:</b> You must specify the number of milliseconds of polling for the sound.</li>
 <li><b>gb_frame_crt_skip:</b> If it is 1 it skips a frame.</li>
 <li><b>gb_delay_emulate_ms:</b> Millisecond wait for each completed frame.</li>
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
</ul>


<br><br>
<h1>Tool dsk2h</h1>
I have created a very basic tool, to convert .dsk files to .h in ready mode to be processed by the emulator. We only have to leave the .dsk files in the folder <b>dsks</b> and execute the file <b>dsk2h.exe</b>, so that an output will be generated in the directory <b>dataFlash</b>. For testing purposes, 2 files have been left in the <b>dsks</b> directory, which it is recommended to delete in case a new custom list is made. It is also recommended to delete the files in the directory <b>CPCem\dataFlash\dsk</b> to have a clean project.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyCPC/tree/main/tools/dsk2h'>Tool dsk2h</a>
<br><br>
<pre>
 dsks/
 dataFlash/
   dsk/
</pre>
Then we must copy the directory <b>dataFlash</b> into the project <b>TinyCPCEMttgovga32\CPCem</b> overwriting the previous dataFlash folder. It is recommended to clean up the project and recompile it.<br>
This tool is very simple, and does not control errors, so it is recommended to leave you the files with very simple names and as simple as possible.<br>
The project in PLATFORM.IO is prepared for 2 MB of Flash. If we need the 4MB of flash, we will have to modify the entry in the file <b>platform.ini</b>
<pre>board_build.partitions = huge_app.csv</pre>
In the Arduino IDE, we must choose the option <b>Partition Scheme (Huge APP)</b>.


<br><br>
<h1>CPM</h1>
To execute the CPM Operating System, the DSK is required (not included in this project), and press the key |.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewCPM.gif'></center>
The <b>Tilde</b> key is taken out with the combination <b>SHIFT</b> + <b>F10</b>.
