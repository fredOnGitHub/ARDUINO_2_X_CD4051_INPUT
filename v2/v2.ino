
/*******************************************************
  Nom ......... : fredOnGithub
  Role ........ : Controlleur Midi USB (ou série) avec l'Arduino Uno
  Auteur ...... : fredOnGithub
  Version ..... : V2 du 26/8/2021
  Licence ..... : GPL-3.0 License

  Compilation :___

********************************************************/

//#define USB

#ifdef USB
#define BAUD 31250 // for MOCO lufa (for example)
#else
//#define BAUD 9600
#define BAUD 115200
#endif

const byte CANAL = 13u;
#define ENABLE 0
#define DISABLE 1
#define MESSAGE_CC 0xB0
#define MESSAGE_NOTE_ON 0x90
#define us unsigned short//2 octets
#define INH_1 6
#define INH_2 7
#define PIN_A 8
#define PIN_B 9
#define PIN_C 10
#define PIN_LECTURE A0

struct BOUTON {
  byte pinINH;
  byte place;
  us etat;
  bool verrou;
};
struct POT {
  byte pinINH;
  byte place;
  int av;//ancienne valeur de pot : 2 octets
};
struct CD4051_ {
  byte read_pin;
  byte out_pins[3];
  byte inhib[2];//2 composants CD4051
};

/*************************************************************/
const byte /*0~255*/ N_POTs = 1;
const byte /*0~255*/ N_BOUTONs = 1;

BOUTON BOUTONs [ N_BOUTONs];
POT POTs [ N_POTs];

byte n;
byte note = 0;
us *pt_etat = 0;
bool *pt_verrou;
int r;

/*************************************************************/
void setup() {
  
  Serial.begin(BAUD);
  
  POTs[0] = {INH_1, 0, 0};
  BOUTONs[0] = {INH_2, 7, 0, false};
  
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  
  pinMode(INH_1, OUTPUT);
  pinMode(INH_2, OUTPUT);
  digitalWrite(INH_1, DISABLE);
  digitalWrite(INH_2, DISABLE);
}
int f(byte inh, byte place) {
  digitalWrite(inh, ENABLE);
  digitalWrite(PIN_A , (place & 0b001));
  digitalWrite(PIN_B , (place & 0b010) >> 1);
  digitalWrite(PIN_C, (place & 0b100) >> 2);
  int r = analogRead(PIN_LECTURE);
  digitalWrite(inh, DISABLE);
  return r;
}
void envoie_midi(byte a, byte b, byte c) {
  Serial.write(a);
  Serial.write(b);
  Serial.write(c);
}
void gere_pots(void) {
  for (n = 0; n < N_POTs; n++) {
    r = f(POTs[n].pinINH, POTs[n].place);
    if (abs(r - POTs[n].av) > 8) {/* POT */
      POTs[n].av = r;
      envoie_midi(MESSAGE_CC + CANAL, note, r / 8);
    }
    note++;
  }
}
void gere_boutons_hold(void) {
  for (n = 0; n < N_BOUTONs; n++) {
    pt_etat = &(BOUTONs[n].etat);
    r = f( BOUTONs[n].pinINH, BOUTONs[n].place);
    if (r > 512) {/*car diviser par 1023 -> 0 si 1022. Normalement pas de chlaouche*/
      *pt_etat = (*pt_etat << 1) | 0xe000;
    }
    else {
      /*introduction d'un 1 (0x0001) qui va se décaler vers la gauche*/
      //https://my.eng.utah.edu/~cs5780/debouncing.pdf
      *pt_etat = (*pt_etat << 1) | 0xe000 | 0x0001;
    }
    if (*pt_etat == 0xf000) {/*l'action se fait une fois le bouton libre après relâchement*/
      envoie_midi(MESSAGE_NOTE_ON + CANAL, note, 0x40);/*fin de la gestion du rebond*/
    }
    note++;
  }
}
void gere_boutons_direct(void) {
  for (n = 0; n < N_BOUTONs; n++) {
    pt_etat = &(BOUTONs[n].etat);
    pt_verrou = &(BOUTONs[n].verrou);
    r = f( BOUTONs[n].pinINH, BOUTONs[n].place);
    if (r > 512) {/*car diviser par 1023 -> 0 si 1022. Normalement pas de chlaouche*/
      *pt_etat = (*pt_etat << 1) | 0xe000;/*la plupart du temps on est ici*/
    }
    else {/*appui ou rebond détecté*/
      if ( ! (*pt_verrou)) {
        *pt_verrou = true;
        envoie_midi(MESSAGE_NOTE_ON + CANAL, note, 0x40);
      }
      /*introduction d'un 1 (0x0001) qui va se décaler vers la gauche*/
      // voir https://my.eng.utah.edu/~cs5780/debouncing.pdf
      *pt_etat = (*pt_etat << 1) | 0xe000 | 0x0001;
    }
    if (*pt_etat == 0xf000) {
      *pt_verrou = false;/*fin de la gestion du rebond*/
    }
    note++;
  }
}
void loop() {
  note = 0;
  gere_pots();
  //  gere_boutons_direct();
  gere_boutons_hold();
}


/********************************************************************/
//TEST de rapidité : dans les 40 ms c'est bien
//unsigned long t = 0;
//bool ok;
//void gere_boutons(void) {
//  for (n = 0; n < N_BOUTONs; n++) {
//    pt_etat = &(BOUTONs[n].etat);
//    r = f( BOUTONs[n].pinINH, BOUTONs[n].place);
//    //    Serial.println(BOUTONs[n].etat, BIN); delay(10);
//    if (r > 512) {
//      *pt_etat = (*pt_etat << 1) | 0xe000;
//      /*car diviser par 1023 peut parfois donner 0 (vu)*/
//    }
//    else {
//      if (!ok) {
//        Serial.println("ok");
//        ok = true;
//        t = millis();
//      }
//      //https://my.eng.utah.edu/~cs5780/debouncing.pdf
//      *pt_etat = (*pt_etat << 1) | 0xe000 | 0x0001;
//    }
//    if (*pt_etat == 0xf000) {
//      Serial.println(millis() - t);
//      ok = false;
//    }
//    note++;
//  }
//}

//void loop() {//TESTs
//  t = millis();
//  for (int ii = 0; ii < 100; ii++) {
//    note = 0;
//    gere_pots();
//    gere_boutons();
//  }
//  Serial.println(millis() - t, DEC);
//  delay(1000);
//}
/*
  PULL UP
  1111111111111111
  1111111111111111
  1111111111111111
  1111111111111110
  1111111111111100
  1111111111111000
  1111111111110000
  1111111111100000
  1111111111000000
  1111111110000000
  1111111100000000
  1111111000000000
  1111110000000000
  1111100000000000
  1111000000000000
  1110000000000000

  PULL DOWN non résolu car relachement trop vite
  1111111111111111
  1111111111111111
  1111111111111111
  1111111111111110
  1111111111111100
  1111111111111000
  1111111111110000
  1111111111100001
  1111111111000011
  1111111110000111
  1111111100001111
  1111111000011111
  1111110000111111
  1111100001111111
  1111000011111111
  1110000111111111
  1110001111111111
  1110011111111111
  1110111111111111
  1111111111111111

  Conclusion :
  1110000000000000 non trouvé
*/
