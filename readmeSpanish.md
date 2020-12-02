
# Tiny ESP32 AMSTRAD CPC
Port del emulador de PC de Tom Walker (AMSTRAD CPC) a la placa TTGO VGA32 v1.2 con ESP32.
<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewCPC464.gif'></center>
He realizado varias modificaciones:
<ul>
 <li>Portado de x86 PC a ESP32</li>
 <li>No se usa PSRAM, funcionando en ESP32 de 520 KB de RAM (TTGO VGA32)</li> 
 <li>Uso de un sólo core</li>
 <li>OSD de bajos recursos</li>
 <li>Creado proyecto compatible con Arduino IDE y Platform IO</li>
 <li>Emulación de AY8912</li>
 <li>Ajuste de pantalla X</li>
 <li>Menú de velocidad de video de emulación, sonido, teclado</li>
 <li>Soporte para modo 8 y 64 colores.</li>
 <li>Soporte DSK de 44 tracks y 11 sectores.</li>
 <li>VGA 400x300</li>
 <li>VGA 320x200 con o sin bordes</li>
 <li>Emulación CPC 464 y 664</li>
 <li>Emulacion CPC 6128 (inestable)</li>
</ul> 
  
<br>
<h1>Requerimientos</h1>
Se requiere:
 <ul>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.0</li>
  <li>Arduino IDE 1.8.11</li>
  <li>Librería Arduino fabgl 0.9.0</li>
  <li>Librería Arduino bitluni 0.3.3</li>
 </ul>
 
 
<h1>PlatformIO</h1>
Se debe instalar el PLATFORMIO 2.2.0 desde las extensiones del Visual Studio.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewPlatformIOinstall.gif'></center>
Luego se seleccionará el directorio de trabajo <b>TinyCPCEMttgovga32</b>.
Debemos modificar el fichero <b>platformio.ini</b> la opción <b>upload_port</b> para seleccionar el puerto COM donde tenemos nuestra placa TTGO VGA32.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewPlatformIO.gif'></center>
Luego procederemos a compilar y subir a la placa. No se usa particiones, así que debemos subir todo el binario compilado.
Está todo preparado para no tener que instalar las librerias de bitluni. Sin embargo, si necesitamos las opciones de sonido y ratón, necesitaremos instalar las librerias de <b>fabgl</b>.


<br>
<h1>Arduino IDE</h1>
Todo el proyecto es compatible con la estructura de Arduino 1.8.11.
Tan sólo tenemos que abrir el <b>ZX-ESPectrum.ino</b> del directorio <b>ZX-ESPectrum</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewArduinoIDEpreferences.gif'></center>
Debemos instalar las extensiones de spressif en el gestor de urls adicionales de tarjetas <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyZXSpectrum/main/preview/previewArduinoIDElibrary.gif'></center>
Para el modo normal, ya está preparado el proyecto, de forma que no se necesita ninguna librería de bitluni. Sin embargo, si necesitamos el soporte de ratón y sonido, se requiere instalar las librerias <b>fabgl 0.9.0</b>.
Lo podemos hacer desde el administrador de bibliotecas.
Debemos desactivar la opción de PSRAM, y en caso de superar 1 MB de binario, seleccionar 4 MB de partición a la hora de subir. Aunque el código no use PSRAM, si la opción está activa y nuestro ESP32 no dispone de ella, se generará una excepción y reinicio del mismo en modo bucle.



<br>
<h1>Usabilidad</h1>
Se permiten las siguientes accciones desde el menú (tecla F1):
 <ul>
  <li>Seleccionar Máquina permite elegir CPC 464, 664 o 6128.</li>  
  <li>Seleccionar DSK permite elegir el disco.</li>
  <li>Offset X de la pantalla</li>
  <li>Permite saltar un frame</li>
  <li>Cambiar los milisegundos de polling para video, teclado y sonido</li>
  <li>Cambiar los milisegundos de espera en cada frame</li>
 </ul>
 Se dispone de un OSD básico de bajos recursos, es decir, muy simple, que se visualiza pulsando la tecla <b>F1</b>.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewOSD.gif'></center>
 Los ficheros deben ser convertidos a .h en hexadecimal. Puede usarse la herramienta online:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a>
 Los DSK por ahora estan en formato DUMP. En breves dejaré la tool para generarlos.
 
 
<br>
<h1>Opciones</h1>
El archivo <b>gbConfig.h</b> se seleccionan las opciones:
<ul>
 <li><b>use_lib_400x300:</b> Se usa modo de vídeo 400x300.</li> 
 <li><b>use_lib_320x200_video_border:</b> Se usa modo de vídeo 320x200 con bordes, de manera que se reduce el tamaño de la pantalla a la mitad. Este modo consume menos RAM que el 400x300 y es más rápido.</li> 
 <li><b>use_lib_320x200_video_noborder:</b> Se usa modo de vídeo 320x200 sin bordes. En este modo no se reduce la pantalla a la mitad, pero se pierden los bordes. Este modo consume menos RAM que el 400x300 y es más rápido.</li>
 <li><b>use_lib_vga8colors:</b> Obliga a usar RGB modo de 8 colores (3 pines). Consume poca ram y es muy rápido, pero sólo saca 8 colores, frente a los 64 del modo normal (6 pines RRGGBB).</li>
 <li><b>use_lib_vga_low_memory:</b> Modo experimental de bajo consumo de RAM de video, pero más lento.</li> 
 <li><b>use_lib_ultrafast_vga:</b> Modo experimental ultra rápido, que se consigue casi el doble de velocidad de acceso a video.</li>
 <li><b>use_lib_sound_ay8912:</b> Se utiliza un mezclador de 3 canales en modo dirty, emulando el AY8912. Consume bastante ram. Se requiere la librería fabgl 0.9.0</li>    
 <li><b>use_lib_log_serial:</b> Se envian logs por puerto serie usb</li>
 <li><b>usb_lib_optimice_checkchange_bankswitch:</b> Sólo conmuta bancos cuando son distintos, ganando velocidad.</li>
 <li><b>use_lib_128k:</b> Permite usar el modo 128K, incluyendo la rom del 6128 en la compilación, así como 2 bloques de memoria de 64KB. Está en fase de pruebas, y al requerir más RAM, se tiene que usar otra opciones de configuración de bajo consumo de RAM.</li>
 <li><b>use_lib_cheat_128k:</b> Modo experimental de 128 KB.</li> 
 <li><b>gb_ms_keyboard:</b> Se debe especificar el número de milisegundos de polling para el teclado.</li>
 <li><b>gb_ms_sound:</b> Se debe especificar el número de milisegundos de polling para el sonido.</li>
 <li><b>gb_frame_crt_skip:</b> Si es 1 se salta un frame.</li>
 <li><b>gb_delay_emulate_ms:</b> Milisegundos de espera por cada frame completado.</li>
</ul>



<br>
<h1>Aplicaciones Test</h1>
Al arrancar el ESP32, se realiza en modo CPC464 cargando el BASIC.
Podemos elegir los siguientes juegos:
<ul>
 <li><b>Corsair Trainer</b></li>
 <li><b>Dragon Attack</b></li>
 <li><b>Babas Palace</b></li>
 <li><b>Amsthrees</b></li>
 <li><b>Sokoban</b></li>
</ul>