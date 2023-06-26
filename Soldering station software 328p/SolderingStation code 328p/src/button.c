/*
 * button.c
 *
 * Created: 3/4/2023 10:02:38 PM
 *  Author: Victor
 */

void buttinit(struct button* butt, uint8_t pin) {
  butt->_pin = pin;
  pinMode(butt->_pin, INPUT_PULLUP);
}

// bool click() {
//   bool btnState = digitalRead(_pin);
//   if (!btnState && !_flag && millis() - _tmr >= 100) {
//     _flag = true;
//     _tmr = millis();
//     return true;
//   }
//   if (!btnState && _flag &&
//       millis() - _tmr >= 100) {  // If butt is press more than 100ms...
//     _tmr_hold = millis();
//     return true;  //... return true every hold_delay ms
//   }
//   if (btnState && _flag) {
//     _flag = false;
//     _tmr = millis();
//   }
//   return false;
// }
uint8_t check(struct button* butt, uint8_t hold_delay) {
  uint8_t btnState = digitalRead(butt->_pin);

  if (!btnState && !butt->_flag && millis() - butt->_tmr >= 100) {
    butt->_flag = true;
    butt->_tmr = millis();
    return true;
  }
  if (!btnState && butt->_flag && millis() - butt->_tmr >= 500 &&
      millis() - butt->_tmr_hold > hold_delay) {
    // If butt is press more than 500ms...
    butt->_tmr_hold = millis();
    return true;  //... return true every hold_delay ms
  }
  if (btnState && butt->_flag) {
    butt->_flag = false;
    butt->_tmr = millis();
  }
  return false;
}

// uint8_t check(struct button* butt) {
//   return check(0);
// }