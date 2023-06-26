/*
 * main.h
 *
 * Created: 3/4/2023 9:59:37 PM
 *  Author: Victor
 */ 

#ifndef MAIN_H_
#define MAIN_H_
//--------------------------------------------- Modules -----------------------------------------------
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

// #include "adc.h"
// #include "hotend.h"
// #include "display.h"
// #include "button.h"
// #include "hardwarefail.h"
#include "timer.h"
//--------------------------------------------- Modules -----------------------------------------------

//----------------------------------------- Arduino pins -------------------------------------------
// #define SCLK 13
// #define RCLK 11
// #define DIO 12

// #define butt_up_pin 18            //A4
// #define butt_ok_pin 17            //A3
// #define butt_down_pin 16          //A2
// #define hotAir_termocuple_pin 15  //A1
// #define sold_termocuple_pin 14    //A0

// #define reed_sw_pin 7
// #define hotAir_heat_coil_pin 6
// #define sold_heat_coil_pin 5
// #define fan_pin 3

// #define hardw_fail_pin 4
//----------------------------------------- Arduino pins -------------------------------------------

//----------------------------------------- EEPROM adress for: -------------------------------------
// #define soldsettemp 0    //word, 2 byte
// #define hotairsettemp 2  //word, 2 byte
// #define soldstate 4      //byte, 1 byte
// #define hotairstate 5    //byte, 1 byte
// #define setfan 6         //byte, 1 byte

// #define soldkp 7   //float, 4 byte
// #define soldki 11  //float, 4 byte
// #define soldkd 15  //float, 4 byte
// #define hAirkp 19  //float, 4 byte
// #define hAirki 23  //float, 4 byte
// #define hAirkd 27  //float, 4 byte

// #define soldoutMin 31    //word, 2 byte
// #define soldoutMax 33    //word, 2 byte
// #define hotairoutMin 35  //word, 2 byte
// #define hotairoutMax 37  //word, 2 byte

// #define dispbright 39  //byte, 1 byte
//----------------------------------------- EEPROM adress ------------------------------------------

//--------------------------------------- Pozition on disp -----------------------------------------
#define soldPoz 0
#define hotAirPoz 3
#define fanPoz 6
//--------------------------------------- Pozition on disp -----------------------------------------

//--------------------------------------
#define maxSetableTemp 450
#define minSetableTemp 50
// #define HIGH 1
// #define LOW 0
// #define true 1
// #define false 0
//--------------------------------------


//************************************************************************************************************************************

void settingMenu() {  //---------------- Settings menu //----------------
  uint8_t key = 1;
  uint32_t prevshow = millis();

  sold.setTempZero();
  hotAir.setTempZero();

  //---------------- Sett menu message  //----------------
  while (millis() - prevshow < 1000) {
    display.clear();
    display.print(0, _s, _e, _t, _t, _i, _n, _g);
    display.update();
  }
  //---------------- Sett menu message  //----------------

  //---------------- Setting menu //----------------
  while (key < 9) {
    sold.setTempZero();
    hotAir.setTempZero();
    //-------------------------------------hardw_fail_protect();

    if (buttDown.check(500)) {
      key++;
      if (key > 5)
        key = 1;
    }
    if (buttUp.check(500)) {
      key--;
      if (key < 1)
        key = 5;
    }

    switch (key) {
        //---------------- Case 1 Brightness setting //----------------
      case 1:
        display.clear();
        display.print(0, _b, _r, _t);
        display.printNum(3, display.getbright());

        if (buttCaseOk.check()) {
          bool key1 = 1;
          while (key1) {
            display.clear();

            display.print(0, _b, _r, _t);
            display.blinkNum(3, display.getbright());

            if (buttUp.check(300) and
                display.getbright() < 15) {  // If butt pressed
              display.setbright(display.getbright() +
                                1);  // Increase brightness
            }
            if (buttDown.check(300) and
                display.getbright() > 1) {  // If butt pressed
              display.setbright(display.getbright() -
                                1);  // Decrease brightness
            }
            if (buttCaseOk.check())
              key1 = 0;
            display.update();
          }
          eeprom_write_byte(dispbright,
                            display.getbright());  // Keep in eeprom
        }
        display.update();
        break;
        //---------------- Case 1 Brightness setting //----------------

        //---------------- Case 2 Temperature calibration //----------------
      case 2:
        display.clear();
        display.print(0, _c, _a, _l);

        if (buttCaseOk.check()) {
          uint8_t key1 = 1;

          while (key1 < 10) {
            display.clear();

            if (buttDown.check(500)) {
              key1++;
              if (key1 > 3)
                key1 = 1;
            }
            if (buttUp.check(500)) {
              key1--;
              if (key1 < 1)
                key1 = 3;
            }

            switch (key1) {
              case 1:  //---------------- Solder Temp calibration
                display.print(0, _t, _e, _n, _p, _empty, _empty, _s);
                if (buttCaseOk.check()) {
                  uint8_t key2 = 1;
                  uint16_t tempMin = eeprom_read_word(soldoutMin);
                  uint16_t tempMax = eeprom_read_word(soldoutMax);

                  while (key2 < 3) {
                    display.clear();

                    switch (key2) {
                      case 1:
                        if (buttUp.check(25) and
                            tempMin < sold.getInMin() +
                                          50) {  // If buttUp is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMin++;             // Increase Temp
                        }
                        if (buttDown.check(25) and
                            tempMin > sold.getInMin() -
                                          50) {  // If buttDown is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMin--;             // Decrease Temp
                        }

                        sold.setTempCrude(sold.getInMin());  // run sold temp to
                                                             // soldInMin
                        display.printNum(0, sold.getInMin());
                        display.blinkNum(3, tempMin);

                        if (buttCaseOk.check())
                          key2 = 2;
                        break;

                      case 2:
                        if (buttUp.check(25) and
                            tempMax < sold.getInMax() +
                                          50) {  // If buttUp is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMax++;             // Increase Temp
                        }
                        if (buttDown.check(25) and
                            tempMax > sold.getInMax() -
                                          50) {  // If buttDown is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMax--;             // Decrease Temp
                        }

                        sold.setTempCrude(sold.getInMax());  // run sold temp to
                                                             // soldInMax
                        display.printNum(0, sold.getInMax());
                        display.blinkNum(3, tempMax);

                        if (buttCaseOk.check())
                          key2 = 10;
                        break;
                    }
                    display.update();
                  }
                  if (eeprom_read_word(soldoutMin) !=
                      tempMin) {  // If temp was changed
                    eeprom_write_word(soldoutMin,
                                      tempMin);  // Keep in eeprom
                    sold.setOutMin(tempMin);
                  }
                  if (eeprom_read_word(soldoutMax) !=
                      tempMax) {  // If temp was changed
                    eeprom_write_word(soldoutMax,
                                      tempMax);  // Keep in eeprom
                    sold.setOutMax(tempMax);
                  }

                  sold.setTempZero();    // Stop solderin iron
                  hotAir.setTempZero();  // Stop hotair gun
                }
                break;  //---------------- Solder Temp calibration

              case 2:  //---------------- hotAir Temp calibration
                display.print(0, _t, _e, _n, _p, _empty, _empty, _a);
                if (buttCaseOk.check()) {
                  uint8_t key2 = 1;
                  uint16_t tempMin = eeprom_read_word(hotairoutMin);
                  uint16_t tempMax = eeprom_read_word(hotairoutMax);
                  while (key2 < 3) {
                    display.clear();

                    switch (key2) {
                      case 1:
                        if (buttUp.check(25) and
                            tempMin < hotAir.getInMin() +
                                          50) {  // If buttUp is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMin++;             // Increase Temp
                        }
                        if (buttDown.check(25) and
                            tempMin > hotAir.getInMin() -
                                          50) {  // If buttDown is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMin--;             // Decrease Temp
                        }

                        hotAir.setTempCrude(
                            hotAir.getInMin());  // run hotair temp
                                                 // to hotairInMin
                        display.printNum(0, hotAir.getInMin());
                        display.blinkNum(3, tempMin);

                        if (buttCaseOk.check())
                          key2 = 2;
                        break;

                      case 2:
                        if (buttUp.check(25) and
                            tempMax < hotAir.getInMax() +
                                          50) {  // If buttUp is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMax++;             // Increase Temp
                        }
                        if (buttDown.check(25) and
                            tempMax > hotAir.getInMax() -
                                          50) {  // If buttDown is presed
                                                 // (return true every
                                                 // 25ms) and temp is true
                          tempMax--;             // Decrease Temp
                        }

                        hotAir.setTempCrude(
                            hotAir.getInMax());  // run hotair temp
                                                 // to hotairInMax
                        display.printNum(0, hotAir.getInMax());
                        display.blinkNum(3, tempMax);

                        if (buttCaseOk.check())
                          key2 = 10;
                        break;
                    }
                    display.update();
                  }
                  if (eeprom_read_word(hotairoutMin) !=
                      tempMin) {  // If temp was changed
                    eeprom_write_word(hotairoutMin,
                                      tempMin);  // Keep in eeprom
                    hotAir.setOutMin(tempMin);
                  }
                  if (eeprom_read_word(hotairoutMax) !=
                      tempMax) {  // If temp was changed
                    eeprom_write_word(hotairoutMax,
                                      tempMax);  // Keep in eeprom
                    hotAir.setOutMax(tempMax);
                  }

                  sold.setTempZero();    // Stop solderin iron
                  hotAir.setTempZero();  // Stop hotair gun
                }
                break;  //---------------- hotAir Temp calibration

              case 3:  //---------------- Exit Temp calibration
                display.print(4, _o, _u, _t);
                if (buttCaseOk.check()) {
                  key1 = 10;
                }
                break;  //---------------- Exit Temp calibration
            }
            display.update();
          }
        }
        display.update();
        break;
        //---------------- Case 2 Temperature calibration //----------------

        //---------------- Case 3 PID calibration //----------------
      case 3:
        display.clear();
        display.print(0, _p, _i, _d);
        if (buttCaseOk.check()) {
          uint8_t key1 = 1;
          while (key1 < 10) {
            if (buttDown.check(500)) {
              key1++;
              if (key1 > 9)
                key1 = 1;
            }
            if (buttUp.check(500)) {
              key1--;
              if (key1 < 1)
                key1 = 9;
            }
            display.clear();
            switch (key1) {
              case 1:
                display.print(0, _a, _u, _t, _o, _empty, _empty, _s);
                if (buttCaseOk.check()) {
                  prevshow = millis();
                  while (millis() - prevshow < 500) {
                    display.clear();
                    display.print(0, _n, _v, _a, _l, _b, _l, _e);
                    display.update();
                  }
                }
                break;
              case 2:
                display.print(0, _a, _u, _t, _o, _empty, _empty, _a);
                if (buttCaseOk.check()) {
                  prevshow = millis();
                  while (millis() - prevshow < 500) {
                    display.clear();
                    display.print(0, _n, _v, _a, _l, _b, _l, _e);
                    display.update();
                  }
                }
                break;
              case 3:
                display.print(0, _p, _i, _d, _empty, _s, _empty, _p);
                break;
              case 4:
                display.print(0, _p, _i, _d, _empty, _s, _empty, _i);
                break;
              case 5:
                display.print(0, _p, _i, _d, _empty, _s, _empty, _d);
                break;
              case 6:
                display.print(0, _p, _i, _d, _empty, _a, _empty, _p);
                break;
              case 7:
                display.print(0, _p, _i, _d, _empty, _a, _empty, _i);
                break;
              case 8:
                display.print(0, _p, _i, _d, _empty, _a, _empty, _d);
                break;

              case 9:  //---------------- Exit PID calibration
                display.print(4, _o, _u, _t);
                if (buttCaseOk.check()) {
                  key1 = 10;
                }
                break;  //---------------- Exit PID calibration
            }
            display.update();
          }
        }
        display.update();
        break;
        //---------------- Case 3 PID calibration //----------------

        //---------------- Case 4 Reset Temp calibration //----------------
      case 4:
        display.clear();
        display.print(0, _r, _s, _t);
        if (buttCaseOk.check()) {
          uint8_t key1 = 1;
          while (key1 < 10) {
            if (buttDown.check(500)) {
              key1++;
              if (key1 > 4)
                key1 = 1;
            }
            if (buttUp.check(500)) {
              key1--;
              if (key1 < 1)
                key1 = 3;
            }
            display.clear();
            switch (key1) {
              case 1:  //---------------- Reset sold temp calibration
                display.print(0, _r, _s, _t, _empty, _t, _empty, _s);
                if (buttCaseOk.check()) {
                  uint8_t key2 = 1;
                  while (key2 < 3) {
                    if (buttDown.check(500)) {
                      key2++;
                      if (key2 > 2)
                        key2 = 1;
                    }
                    if (buttUp.check(500)) {
                      key2--;
                      if (key2 < 1)
                        key2 = 2;
                    }
                    display.clear();
                    display.print(0, _r, _s, _t, _empty, _s);
                    switch (key2) {
                      case 1:
                        display.blinkSimb(6, _n);
                        if (buttCaseOk.check()) {
                          key2 = 10;
                        }
                        break;
                      case 2:
                        display.blinkSimb(6, _y);
                        if (buttCaseOk.check()) {
                          eeprom_write_word(soldoutMin, sold.getInMin());
                          eeprom_write_word(soldoutMax, sold.getInMax());
                          key2 = 10;
                        }
                        break;
                    }
                    display.update();
                  }
                }
                break;  //---------------- Reset sold temp calibration

              case 2:  //---------------- Reset hotAir temp calibration
                display.print(0, _r, _s, _t, _empty, _t, _empty, _a);
                if (buttCaseOk.check()) {
                  uint8_t key2 = 1;
                  while (key2 < 3) {
                    if (buttDown.check(500)) {
                      key2++;
                      if (key2 > 3)
                        key2 = 1;
                    }
                    if (buttUp.check(500)) {
                      key2--;
                      if (key2 < 1)
                        key2 = 3;
                    }
                    display.clear();
                    display.print(0, _r, _s, _t, _empty, _a);
                    switch (key2) {
                      case 1:
                        display.blinkSimb(6, _n);
                        if (buttCaseOk.check()) {
                          key2 = 10;
                        }
                        break;
                      case 2:
                        display.blinkSimb(6, _y);
                        if (buttCaseOk.check()) {
                          eeprom_write_word(hotairoutMin, hotAir.getInMin());
                          eeprom_write_word(hotairoutMax, hotAir.getInMax());
                          key2 = 10;
                        }
                        break;
                    }
                    display.update();
                  }
                }
                break;  //---------------- Reset hotAir temp calibration

              case 3:  //---------------- Out Reset temp calibration
                display.print(0, _f, _a, _c, _t, _r, _s, _t);
                if (buttCaseOk.check()) {
                  eeprom_write_word(soldsettemp, 50);
                  eeprom_write_word(hotairsettemp, 50);
                  eeprom_write_byte(soldstate, HIGH);
                  eeprom_write_byte(hotairstate, HIGH);
                  eeprom_write_byte(setfan, 4);
                  eeprom_write_float(soldkp, 5);
                  eeprom_write_float(soldki, 0.5);
                  eeprom_write_float(soldkd, 0.01);
                  eeprom_write_float(hAirkp, 3);
                  eeprom_write_float(hAirki, 0.3);
                  eeprom_write_float(hAirkd, 0.03);
                  eeprom_write_word(soldoutMin, 211);
                  eeprom_write_word(soldoutMax, 410);
                  eeprom_write_word(hotairoutMin, 210);
                  eeprom_write_word(hotairoutMax, 401);
                  eeprom_write_byte(dispbright, 15);
                  while (1) {
                    display.print(0, _r, _e, _s, _t, _a, _r, _t);
                    display.update();
                  }
                }
                break;  //---------------- Out Reset temp calibration

              case 4:  //---------------- Out Reset temp calibration
                display.print(4, _o, _u, _t);
                if (buttCaseOk.check()) {
                  key1 = 10;
                }
                break;  //---------------- Out Reset temp calibration
            }
            display.update();
          }
        }
        display.update();
        break;
        //---------------- Case 4 Reset Temp calibration //----------------

        //---------------- Case 5 //----------------
      case 5:
        display.clear();
        display.print(0, _o, _u, _t);
        display.update();
        if (buttCaseOk.check()) {
          key = 10;
        }
        break;
        //---------------- Case 5 //----------------
    }
  }  //---------------- Setting menu //----------------
}  //---------------- End Settings menu //----------------

//************************************************************************************************************************************

//---------------- Protection against hardware failures //----------------
void hardw_fail_protect() {
  if (soldProtect.checkforfail(sold.getRealTemp(), sold.getSetTemp()) or
      sold.getRealTemp() > maxSetableTemp + 20) {
    while (1) {
      digitalWrite(hardw_fail_pin, LOW);
      digitalWrite(hotAir_heat_coil_pin, LOW);
      digitalWrite(sold_heat_coil_pin, LOW);
      display.clear();
      display.blinkSimb(soldPoz, _e, _r, _r);
      display.update();
    }
  }
  if (hotAirProtect.checkforfail(hotAir.getRealTemp(), hotAir.getSetTemp()) or
      hotAir.getRealTemp() > maxSetableTemp + 20) {
    while (1) {
      digitalWrite(hardw_fail_pin, LOW);
      digitalWrite(hotAir_heat_coil_pin, LOW);
      digitalWrite(sold_heat_coil_pin, LOW);
      display.clear();
      display.blinkSimb(hotAirPoz, _e, _r, _r);
      display.update();
    }
  }
}  //---------------- Protection against hardware failures //----------------

//************************************************************************************************************************************

void menu() {  //---------------- //---------------- Menu //----------------
  uint8_t key = 1;
  uint32_t prev_menu = millis();        // Keep last time for menu
  uint32_t prev_state_hold = millis();  // Keep last time for state change

  uint32_t prev_hold_key;

  uint16_t prevSoldTemp = sold.getSetTemp(),
           prevAirTemp =
               hotAir
                   .getSetTemp();  // Keep last temp to save in EEPROM new temp
  uint8_t prevFan =
      hotAir.getFan();  // Keep last fan val to save in EEPROM new val

  bool state_flag_hold = 0, key_flag = 0,
       key_flag_hold = 0;  // (State change) flag and (Key change) flags

  while (millis() - prev_menu < 5000 and key < 4) {
    hardw_fail_protect();

    //---------------- Keychange //----------------

    // once in 200ms If butt_ok is presed and it wasn't pressed before...
    if (!digitalRead(butt_ok_pin) and !key_flag and
        millis() - prev_menu > 200) {
      prev_menu = millis();  // Reset timer menu()
      key_flag = 1;          // Allow permission to change -> (key++)
    }
    // If butt_ok is releaset and have permision (key_flag) and it wasn't
    // held before (in sold/hotAir state change)...
    if (digitalRead(butt_ok_pin) and key_flag and !key_flag_hold) {
      prev_menu = millis();  // Reset timer menu()
      key_flag = 0;          // Reset permission to change -> (key++)
      key++;                 // Increase key by one unit
    } else if (digitalRead(butt_ok_pin) and key_flag and
               key_flag_hold) {  // But if it WAS held before (in sold/hotAir
                                 // state change)
      prev_menu = millis();      // Reset timer menu()
      key_flag = 0;              // Reset permission to change -> (key++)
      key_flag_hold = 0;  // Reset flag_hold and perm to change -> (key++)
    }
    //---------------- Key change //----------------

    switch (key) {
      case 1:  //---------------- Case1 //----------------

        sold.setTempZero();  // Set sold temp zero

        //---------------- Sold state change //----------------
        if (buttCaseOk.check() and
            !state_flag_hold) {        // If ok is pressed and count is reset
          state_flag_hold = 1;         // Start count for change state ...
          prev_state_hold = millis();  // Reset hold timer
        }
        if (buttCaseOk.check() and state_flag_hold and
            millis() - prev_state_hold >
                1000) {          // If ok is pressed and held more than 1 sec
          key_flag_hold = 1;     // Set flag_hold for (see uper - Key change)
          state_flag_hold = 0;   // Reset count for change state ...
          prev_menu = millis();  // Reset timer menu()
          sold.setState(!sold.getState());  // ... change sold state
          eeprom_write_byte(soldstate,
                            sold.getState());  // Keep sold state in EEPROM
        }
        //---------------- Sold state change //----------------

        //---------------- SetTemp change //----------------
        if (buttUp.check(25) and
            sold.getSetTemp() <
                maxSetableTemp) {  // If buttUp is presed (return true every
                                   // 25ms) and sold setTemp is <480
          prev_menu = millis();    // Reset timer menu()
          sold.setSetTemp(sold.getSetTemp() + 1);  // Increase setTemp
        }
        if (buttDown.check(25) and
            sold.getSetTemp() >
                minSetableTemp) {  // If buttDown is presed (return true
                                   // every 25ms) and sold setTemp is >50
          prev_menu = millis();    // Reset timer menu()
          sold.setSetTemp(sold.getSetTemp() - 1);  // Decrease setTemp
        }
        //---------------- SetTemp change //----------------

        //---------------- Fill sold disp //----------------
        if (sold.getState()) {
          display.blinkNum(soldPoz, sold.getSetTemp());
        } else {
          display.blinkSimb(soldPoz, _o, _f, _f);
        }
        //---------------- Fill sold disp //----------------

        //---------------- HotAir normal work //----------------
        if (hotAir.getState()) {
          hotAir.setTemp();
          display.printNum(hotAirPoz, hotAir.getRealTemp());
          display.print1Num(fanPoz, hotAir.getFan());
        } else {
          hotAir.setTempZero();
          display.print(hotAirPoz, _o, _f, _f);
          display.print1Num(fanPoz, hotAir.getFan());
        }
        //---------------- HotAir normal work //----------------

        display.update();

        break;  //---------------- Case1 //----------------

      case 2:  //---------------- Case2 //----------------

        hotAir.setTempZero();  // Set hotAir temp zero

        //---------------- hotAir state change //----------------
        if (buttCaseOk.check() and
            !state_flag_hold) {        // If ok is pressed and count is reset
          state_flag_hold = 1;         // Start count for change state ...
          prev_state_hold = millis();  // Reset hold timer
        }
        if (buttCaseOk.check() and state_flag_hold and
            millis() - prev_state_hold >
                1000) {          // If ok is pressed and held more than 1 sec
          key_flag_hold = 1;     // Set flag_hold for (see uper - Key change)
          state_flag_hold = 0;   // Reset count for change state ...
          prev_menu = millis();  // Reset timer menu()
          hotAir.setState(!hotAir.getState());  // ... change hotAir state
          eeprom_write_byte(hotairstate,
                            hotAir.getState());  // Keep hotAir state in EEPROM
        }
        //---------------- hotAir state change //----------------

        //---------------- SetTemp change //----------------
        if (buttUp.check(25) and hotAir.getSetTemp() < maxSetableTemp) {
          prev_menu = millis();  // Reset timer menu()
          hotAir.setSetTemp(hotAir.getSetTemp() + 1);
        }
        if (buttDown.check(25) and hotAir.getSetTemp() > minSetableTemp) {
          prev_menu = millis();  // Reset timer menu()
          hotAir.setSetTemp(hotAir.getSetTemp() - 1);
        }
        //---------------- SetTemp change //----------------

        //---------------- Fill hotAir disp //----------------
        if (hotAir.getState()) {
          display.blinkNum(hotAirPoz, hotAir.getSetTemp());
        } else {
          display.blinkSimb(hotAirPoz, _o, _f, _f);
        }
        //---------------- Fill hotAir disp //----------------

        //---------------- Sold normal work //----------------
        if (sold.getState()) {
          sold.setTemp();
          display.printNum(soldPoz, sold.getRealTemp());
        } else {
          sold.setTempZero();
          display.print(soldPoz, _o, _f, _f);
        }

        display.print1Num(fanPoz, hotAir.getFan());
        //---------------- Sold normal work //----------------

        display.update();

        break;  //---------------- Case2 //----------------

      case 3:  //---------------- Case3 //----------------
               //---------------- Fan change //----------------
        if (buttUp.check(100) and hotAir.getFan() < 9) {
          prev_menu = millis();  // Reset timer menu()
          hotAir.setFan(hotAir.getFan() + 1);
        }
        if (buttDown.check(100) and hotAir.getFan() > 1) {
          prev_menu = millis();  // Reset timer menu()
          hotAir.setFan(hotAir.getFan() - 1);
        }
        //---------------- Fan change//----------------

        //---------------- Fill fan disp  //----------------
        display.blink1Num(fanPoz, hotAir.getFan());
        //---------------- Fill fan disp  //----------------

        //---------------- Sold and hotAir normal work  //----------------
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
        } else {
          hotAir.setTempZero();
          display.print(hotAirPoz, _o, _f, _f);
        }
        //---------------- Sold and hotAir normal work  //----------------

        display.update();

        break;  //---------------- Case3  //----------------
    }
  }
  //---------------- Save changes in EEPROM //----------------
  if (prevSoldTemp != sold.getSetTemp())
    eeprom_write_word(soldsettemp, sold.getSetTemp());
  if (prevAirTemp != hotAir.getSetTemp())
    eeprom_write_word(hotairsettemp, hotAir.getSetTemp());
  if (prevFan != hotAir.getFan())
    eeprom_write_byte(setfan, hotAir.getFan());
  //---------------- Save changes in EEPROM //----------------
}  //---------------- //---------------- Endmenu1  //----------------
   //*********************************************************************************************************************************

#endif/* MAIN_H_ */