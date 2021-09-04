- [Use v2 directly](#use-v2-directly)
  - [2 Modes : hold and direct](#2-modes--hold-and-direct)
- [ARDUINO WITH 2_X_CD4051_INPUT : V1](#arduino-with-2_x_cd4051_input--v1)
  - [From this schematic :](#from-this-schematic-)
  - [Compilation with #define TEST](#compilation-with-define-test)
  - [Compilation without #define TEST](#compilation-without-define-test)
  - [Compilation with #define SERIE](#compilation-with-define-serie)
  - [Compilation without #define SERIE](#compilation-without-define-serie)
  - [Program that helps too](#program-that-helps-too)

# Use v2 directly 
to obtain a simpler program with the intelligence of optimized debouncing management. See https://my.eng.utah.edu/~cs5780/debouncing.pdf (also in documentation).

In v2 **pull up is used and has to be used**. If you use pull down, maybe you'll get.

<p>PULL DOWN non r&eacute;solu car rel&acirc;chement trop vite<br />1111111111111111<br />1111111111111111<br />1111111111111111<br />1111111111111110<br />1111111111111100<br />1111111111111000<br />1111111111110000<br />1111111111100001<br />1111111111000011<br />1111111110000111<br />1111111100001111<br />1111111000011111<br />1111110000111111<br />1111100001111111<br />1111000011111111<br />1110000111111111<br />1110001111111111<br />1110011111111111<br />1110111111111111<br />1111111111111111</p>

  and never you'll get the matching <br>
  1111000000000000<br>
  because you release the button too quickly

<br><br>

<p>PULL UP<br />1111111111111111<br />1111111111111111<br />1111111111111111<br />1111111111111110<br />1111111111111100<br />1111111111111000<br />1111111111110000<br />1111111111100000<br />1111111111000000<br />1111111110000000<br />1111111100000000<br />1111111000000000<br />1111110000000000<br />1111100000000000<br />1111000000000000<br /></p>NICE !! and always it gets this matching


With test we get ~40ms : it is good enough.

## 2 Modes : hold and direct

Hold : action begins after complete hold (button free).

Direct : action starting directly when a push is detected (and the time of managing debounce is over).

<br><br>

# ARDUINO WITH 2_X_CD4051_INPUT : V1

## From this schematic :
![image.png](./documentation/CD4051_schema.png)

## Compilation with #define TEST
To see the output in a serial consol.

## Compilation without #define TEST
To use the Arduino like a USB controller.

## Compilation with #define SERIE
To use the USB serial controler with "Hailess Midi" and "Loop Midi" (for example).

## Compilation without #define SERIE
To use the Arduino like a USB Midi controller if you have flashed it with "DFU-programmer".
See https://github.com/fredOnGitHub/ARDUINO_EN_USB_MIDI

## Program that helps too
https://htmled.it/editeur-html/ to translate text to html online