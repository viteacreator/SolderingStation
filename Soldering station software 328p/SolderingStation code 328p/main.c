/*
 * SolderingStation code 328p.c
 *
 * Created: 3/4/2023 9:58:18 PM
 * Author : Victor
 */
#include "main.h"

int main(void) {
  TCCR2B = 0b01;  // x1, Set PWM freq 31.4 kHz pin 3 and 11
  TCCR2A = 0b01;  // Phase PWM
  int u;          // sdas
                  // asd

  digitalWrite(hardw_fail_pin, HIGH);
  timerinit();
  adcInit(INTERNAL);  // Internal ref voltage 1.1V for ADC, 1.1V = 1024

  //---------------- Read data from EEPROM //----------------
  sold.setState((uint8_t)eeprom_read_byte(soldstate));
  sold.setSetTemp(eeprom_read_word(soldsettemp));

  hotAir.setState((uint8_t)eeprom_read_byte(hotairstate));
  hotAir.setSetTemp(eeprom_read_word(hotairsettemp));
  hotAir.setFan(eeprom_read_byte(setfan));

  sold.setLiniarTemp(
      200, 400, eeprom_read_word(soldoutMin),
      eeprom_read_word(soldoutMax));  // inMin, inMax, outMin, outMax
  hotAir.setLiniarTemp(
      200, 400, eeprom_read_word(hotairoutMin),
      eeprom_read_word(hotairoutMax));  // inMin, inMax, outMin, outMax

  display.setbright(eeprom_read_byte(dispbright));

  //--------------------------
  sold.setPID(eeprom_read_float(soldkp), eeprom_read_float(soldki),
              eeprom_read_float(soldkd));
  hotAir.setPID(eeprom_read_float(hAirkp), eeprom_read_float(hAirki),
                eeprom_read_float(hAirkd));
  //---------------- Read data from EEPROM //----------------

  //---------------- Some variables //----------------
  uint8_t sett_flag;
  uint32_t sett_prev = millis();
  //---------------- Some variables //----------------

  //---------------- hw fail prot init //----------------
  for (int i = 0; i < 5;) {
    if (millis() - sett_prev > 20) {
      sold.readTemp();
      sold.liniarTemp();
      hotAir.readTemp();
      hotAir.liniarTemp();
      sett_prev = millis();
      Serial.println(i);
      Serial.println(sold.getRealTemp());
      Serial.println(hotAir.getRealTemp());
      i++;
    }
  }
  soldProtect.begin(sold.getRealTemp());
  hotAirProtect.begin(hotAir.getRealTemp());
  //---------------- hw fail prot init //----------------

  while (1) {  //---------------- Loop //----------------

    //    settingMenu();//-------^^^^^^^^^^^^^^^^^^^^^^^^^

    //---------------- Enter in Menu //----------------
    if (buttCaseOk.check()) {
      menu();
    }
    //---------------- Enter in Menu //----------------

    //---------------- Enter in Settings //----------------
    if ((buttUp.click() || buttDown.click()) && !sett_flag) {
      sett_flag = 1;
      sett_prev = millis();
      //      Serial.println("1");
    }
    if (!(buttUp.click() || buttDown.click()) && sett_flag) {
      sett_flag = 0;
      //      Serial.println("2");
    }
    if (buttUp.click() and buttDown.click() && millis() - sett_prev > 4000 &&
        sett_flag) {
      settingMenu();
      sett_flag = 0;
      //      Serial.println("3");
    }
    //---------------- Enter in Settings //----------------

    //---------------- Soldering station normal work //----------------
    if (sold.getState()) {
      sold.setTemp();
      display.printNum(soldPoz, sold.getRealTemp());
    } else {
      sold.setTempZero();
      display.print(soldPoz, _o, _f, _f);
    }
    if (hotAir.getState()) {
      hotAir.setTemp();
      display.printNum(hotAirPoz, hotAir.getRealTemp());
      display.print1Num(fanPoz, hotAir.getFan());
    } else {
      hotAir.setTempZero();
      display.print(hotAirPoz, _o, _f, _f);
      display.print1Num(fanPoz, hotAir.getFan());
    }
    display.update();
    //---------------- Soldering station normal work //----------------

    //---------------- Hardware fail protection //----------------
    // hardw_fail_protect();
    //---------------- Hardware fail protection //----------------

  }  //---------------- End Loop //----------------
}
