/*
 * main.h
 *
 * Created: 3/4/2023 9:59:37 PM
 *  Author: Victor
 */ 


#ifndef MAIN_H_
#define MAIN_H_
//--------------------------------------------- Libs -----------------------------------------------
#include <avr/eeprom.h>

#include "hotend.h"
#include "display.h"
#include "button.h"
#include "hardwarefail.h"
//--------------------------------------------- Libs -----------------------------------------------

//----------------------------------------- Arduino pins -------------------------------------------
#define SCLK 13
#define RCLK 11
#define DIO 12

#define butt_up_pin 18            //A4
#define butt_ok_pin 17            //A3
#define butt_down_pin 16          //A2
#define hotAir_termocuple_pin 15  //A1
#define sold_termocuple_pin 14    //A0

#define reed_sw_pin 7
#define hotAir_heat_coil_pin 6
#define sold_heat_coil_pin 5
#define fan_pin 3

#define hardw_fail_pin 4
//----------------------------------------- Arduino pins -------------------------------------------

//----------------------------------------- EEPROM adress for: -------------------------------------
#define soldsettemp 0    //word, 2 byte
#define hotairsettemp 2  //word, 2 byte
#define soldstate 4      //byte, 1 byte
#define hotairstate 5    //byte, 1 byte
#define setfan 6         //byte, 1 byte

#define soldkp 7   //float, 4 byte
#define soldki 11  //float, 4 byte
#define soldkd 15  //float, 4 byte
#define hAirkp 19  //float, 4 byte
#define hAirki 23  //float, 4 byte
#define hAirkd 27  //float, 4 byte

#define soldoutMin 31    //word, 2 byte
#define soldoutMax 33    //word, 2 byte
#define hotairoutMin 35  //word, 2 byte
#define hotairoutMax 37  //word, 2 byte

#define dispbright 39  //byte, 1 byte
//----------------------------------------- EEPROM adress ------------------------------------------

//--------------------------------------- Pozition on disp -----------------------------------------
#define soldPoz 0
#define hotAirPoz 3
#define fanPoz 6
//--------------------------------------- Pozition on disp -----------------------------------------

#endif /* MAIN_H_ */