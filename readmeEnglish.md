

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
 <li>DSK support with 44 tracks and 11 sectors</li>
 <li>Emulates only the CRTC 0</li>
 <li>Supports 0, 1 and 2 video mode</li>
 <li>VGA 400x300</li>
 <li>VGA 320x200 with or without borders</li>
 <li>CPC 464 and 664 emulation</li>
 <li>CPC 6128 (unstable)</li>
</ul> 
