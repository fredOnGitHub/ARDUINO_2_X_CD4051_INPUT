/*******************************************************
Nom ......... : fredOnGithub
Role ........ : Controlleur Midi USB (ou série) avec l'Arduino Uno 
Auteur ...... : fredOnGithub
Version ..... : V1 du 26/8/2021
Licence ..... : GPL-3.0 License 

Compilation :___

********************************************************/

//https://defkey.com/fr/arduino-ide-raccourcis-clavier

//#define TEST
#define SERIE

#define sp Serial.print
#define spn Serial.println

#ifdef SERIE
//#define BAUD 9600 //n'affiche pas en double au début % à 115200
#define BAUD 115200 //n'affiche pas en double au début % à 115200
#else
#define BAUD 31250 // for MOCO lufa (for example)
#endif

#include "constants.h"

enum STATE {ENABLE = 0, DISABLE = 1};

enum COMPONENT_TYPE {
  OUT_OF_ORDER,
  CD4051_ANALOG, CD4051_BUTTON
};

enum MIDI_MESSAGE_TYPE {
  CC = 0xB0 , NOTE_ON = 0x90 , PITCH_BEND = 0xE0
};


/*********************************************************************/

struct B {
  const COMPONENT_TYPE ct;
  const byte midi_channel;
  const MIDI_MESSAGE_TYPE mess_type;
  const byte midi_note;
  bool button_is_pull_up;
  int old_value;//1023 max > 255
  int value;//1023 max > 255
  unsigned long last_debounce_time;
};


/*********************************************************************/

const byte NCD = 2;

struct D {
  byte read_pin;
  byte out_pins[3];
  byte inhib[NCD];
  B b[8 * NCD];
};


D sd = {
  A0,
  {8, 9, 10},
  {6, 7},
  {
    {CD4051_ANALOG, CHAN8, NOTE_ON, CONTROL11},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},

    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {OUT_OF_ORDER, 0, 0, 0},
    {CD4051_BUTTON, CHAN8, NOTE_ON, CONTROL13},
  },
};

/*********************************************************************/

void setup() {
  Serial.begin(BAUD);
  //  fill_TB();//avant tout (déjà fait)
  sd_init();
}


//void loop () {
//  unsigned long t2 = millis();
//  sd_update_state();
//  Serial.println(millis() - t2);
////  delay(1000);
//}

void loop () {
  sd_update_state();
}
