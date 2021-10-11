/**
 * For DosBoxECE-OPL OPL3Duo! serial pass-through
 *
 * REQUIRES: Arduino OPL2 Library - https://github.com/DhrBaksteen/ArduinoOPL2
 *
 * OPL3 board is connected as follows:
 * Pin  6 - A2
 * Pin  7 - A1
 * Pin  8 - A0
 * Pin  9 - /IC
 * Pin 10 - /WR
 * Pin #  - Data    -MOSI of board being used (orignal Arduino Pin-11, ProMini Pin-16 )
 * Pin #  - Shift   -SCK of board being used (original Arduino Pin-13, ProMini Pin-15 )
 *
 * Code by Jason Hill 2021-10-10 adapted from https://github.com/DhrBaksteen/ArduinoOPL2
 * Pins can be adapted as neccesary, but make sure to use hardware SPI pins for best results.
 * Refer to DhrBaksteen's github wiki for more help wiring it and using his library.
 */

//Optimize for speed over size
#pragma GCC optimize ("O2")
#pragma GCC push_options


#include <SPI.h>
#include <OPL3Duo.h>
#include "OPL3_SerialPassthrough.h"
#define SerData Serial

OPL3Duo opl3Duo;

byte state, reg, value, bank, x0, x1, x2;

void setup() {
  SerData.begin(115200);
  opl3Duo.begin();

  //PIN_UNIT selects the OPL3, onlyl need one for OPL3
  //..but this could emulate a SoundBlaster Pro with dual OPL2 chips.
  pinMode(PIN_UNIT, OUTPUT);
  digitalWrite(PIN_UNIT, LOW);

  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_ADDR, OUTPUT);
  pinMode(PIN_RESET,OUTPUT);
  digitalWrite(PIN_LATCH,HIGH);
  digitalWrite(PIN_RESET,HIGH);
  digitalWrite(PIN_ADDR, LOW);

  state=0;
}

void loop() {
  if(SerData.available()) {
    switch(state){
      case 0:
        x0=SerData.read();
        if(x0 & 0x80){
          state=1;
        }
        else {
          state=0;
        }
        break;
      case 1:
        x1= SerData.read();
        state=2;
        break;
      case 2:
        x2= SerData.read();
        bank  = ( x0 & 0x0C ) >> 2;
        reg   = ( (x0 & 0x03) << 6) | ( (x1 & 0x7E) >> 1);
        value = ( (x1 & 0x01) << 7) | (x2 & 0x7F);
        /*sendOPL averages ~37us to complete*/
        sendOPL(bank, reg, value);
        /*fastSendOPL averages ~7us to complete but is hardware dependent*/
        //fastSendOPL(bank, reg, value);
        state=0;
        break;
      default:
        state=0;
        break;
    }
  }  
}

//Reduced instructions to simpliest form from OPL3.H for fastest speed.
inline void sendOPL(byte bank, byte reg, byte value) {
  //Arduino Digital Write is ~3.5us to transititon from high to low
  //on a typical 16Mhz Arduino

  //Transition of PIN_ADDR from low to high ~20us @ 8Mhz SPI (~21us at 4Mhz SI)
  digitalWrite(PIN_ADDR, LOW);
  digitalWrite(PIN_BANK, (bank & 0x01) ? HIGH : LOW);
  SPI.transfer(reg);
  //On ProMicro (32u4 5V) low-high transition takes 5.2us
  digitalWrite(PIN_LATCH, LOW);
  digitalWrite(PIN_LATCH, HIGH);
  digitalWrite(PIN_ADDR, HIGH);

  SPI.transfer(value);
  //On ProMicro (32u4 5V) low-high transition takes 5.2us
  digitalWrite(PIN_LATCH, LOW);
  digitalWrite(PIN_LATCH, HIGH);
}

//Fast implementation for specific hardware. See `OPL3_SerialPassthrough.h`
//All PORTx addressed MUST be adapted for other Arduino models.
inline void fastSendOPL(byte bank, byte reg, byte value) {

  ADDR_LOW;
  
  if (bank & 0x01)
    BANK_HIGH;
  else
    BANK_LOW;
  SPI.transfer(reg);
  
  LATCH_LOW;
  delayMicroseconds(2);
  LATCH_HIGH;

  ADDR_HIGH;
  
  SPI.transfer(value);

  delayMicroseconds(2);
  LATCH_LOW;
  delayMicroseconds(2);
  LATCH_HIGH;
}
