
# Tiny ESP32 AMSTRAD CPC
Port del emulador de PC de Tom Walker (AMSTRAD CPC) a la placata TTGO VGA32 v1.2 con ESP32.
<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewCPC464.gif'></center>
He realizado varias modificaciones:
<ul>
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
