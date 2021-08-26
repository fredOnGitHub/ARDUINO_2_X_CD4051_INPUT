//byte TB[8][3];
byte TB[8][3] = {
  {0, 0, 0},
  {0, 0, 1},
  {0, 1, 0},
  {0, 1, 1},
  {1, 0, 0},
  {1, 0, 1},
  {1, 1, 0},
  {1, 1, 1}
};

void fill_TB() {
  //        abc = 111 => 7 on lit la 8ème entrée
  //        abc = 000 => 0 on lit la 1ère entrée
  //        (i & B00000010) 2ème bit à 1 ? => 2
  //        a = (1 && (i & B00000010)) => alors mettre le 2 à 1
  for (int i = 0; i < 8; i++) {
    TB[i][0] = (1 && (i & B00000001));
    TB[i][1] = (1 && (i & B00000010));
    TB[i][2] = (1 && (i & B00000100));
  }
}

bool is_HIGH(int reading) {
  if (reading > 512) {
    return true;
  }
  else {
    return false;
  }
}

void sd_init() {
  if (NCD) {
    pinMode(sd.out_pins[0], OUTPUT);
    pinMode(sd.out_pins[1], OUTPUT);
    pinMode(sd.out_pins[2], OUTPUT);
    for (byte k = 0; k < NCD; k++) {
      pinMode (sd.inhib[k], OUTPUT);
    }

    B *x;
    int i, n, r;
    for (n = 0; n < NCD; n++) {
      sd_write_all_inh_at_state(DISABLE);
      digitalWrite (sd.inhib[n], ENABLE);
      for (i = 0; i < 8; i++) {
        x = & sd.b[i + n * 8];
        if (x->ct != OUT_OF_ORDER) {
          digitalWrite(sd.out_pins[0], TB[i][0]);
          digitalWrite(sd.out_pins[1], TB[i][1]);
          digitalWrite(sd.out_pins[2], TB[i][2]);

          r = analogRead(sd.read_pin);

          switch (x->ct) {
            case CD4051_ANALOG: {
                x->old_value = x->value = r;
                break;
              }
            case CD4051_BUTTON: {
                x->old_value = x->value = is_HIGH(r);
                if (x->old_value) {
                  x->button_is_pull_up = true;
                } else {
                  x->button_is_pull_up = false;
                }
                break;
              }
            default: {
                break;
              }
          }
        }
      }
    }
  }
}

void sd_write_all_inh_at_state(byte b) {
  for (byte k = 0; k < NCD; k++) {
    digitalWrite (sd.inhib[k], b);
  }
}

void sd_print_state() {
  B *x;
  const byte p = 8 * NCD - 1;
  byte i;
  sp(" [ ");
  for (i = 0; i < 8 * NCD; i++) {
    x  = sd.b + i;
    switch (x->ct) {
      case OUT_OF_ORDER: {
          sp("");
          break;
        }
      default: {
          sp(x->value, DEC);
          break;
        }
    }
    if (i != p) {
      sp(", ");
    }
  }
  sp(" ] ");
  spn();
}

void sd_update_state() {
  byte n, i;
  B *x;
  for (n = 0; n < NCD; n++) {
    sd_write_all_inh_at_state(DISABLE);
    digitalWrite (sd.inhib[n], ENABLE);
    for (i = 0; i < 8; i++) {
      x = & sd.b[i + n * 8];
      if (x->ct != OUT_OF_ORDER) {
        digitalWrite(sd.out_pins[0], TB[i][0]);
        digitalWrite(sd.out_pins[1], TB[i][1]);
        digitalWrite(sd.out_pins[2], TB[i][2]);
        b_manage(x, analogRead(sd.read_pin));
      }
    }
  }
}

void b_manage(B * x, int reading) {
  switch (x->ct) {
    case OUT_OF_ORDER: {
        break;
      }
    case CD4051_ANALOG: { //https://rules.sonarsource.com/c/RSPEC-1151
        x->value = reading;
        int tmp = (x->old_value - x->value);
        if (tmp >= 8 || tmp <= -8) {
          x->old_value = x->value;
#if defined (TEST)
          sd_print_state();
          sp(x->mess_type + x->midi_channel, DEC); sp(" ");
          sp(x->midi_note, DEC); sp(" ");//key note
          sp(x->value / 8, DEC); spn(" ");//a velocity
#else
          Serial.write(x->mess_type + x->midi_channel);
          Serial.write(x->midi_note);//key note
          Serial.write(x->value / 8);//10 bits => (0..1023)/8 == (0..127)
#endif
        }
        break;
      }
    case CD4051_BUTTON: {
        reading = is_HIGH(reading);

        if (reading != x->old_value) {
          x->last_debounce_time = millis();
        }
        if ((millis() - x->last_debounce_time) > 20) {//20 est un bon compromis: appuis rapide

          if (reading != x->value) {

            x->value = reading;

            if ( (x->button_is_pull_up && x->value == LOW) ||
                 (!x->button_is_pull_up && x->value == HIGH) )
            {
#if defined (TEST)
              sd_print_state();
//              sp(x->mess_type + x->midi_channel, DEC); sp(" ");
//              sp(x->midi_note, DEC); sp(" ");//key note
//              sp(0x40, DEC); spn(" ");//a velocity
#else
              Serial.write(x->mess_type + x->midi_channel);
              Serial.write(x->midi_note);//key note
              Serial.write(0x40); //a velocity
#endif
            } else {
#if defined (TEST)
//              sd_print_state();
#endif
            }
          }
        }
        x->old_value = reading;
        break;
      }
    default: {
        break;
      }
  }
}
