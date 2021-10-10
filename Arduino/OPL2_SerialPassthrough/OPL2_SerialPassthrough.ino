/**
 * For DosBoxECE-OPL OPL2Audio serial pass-through
 *
 *REQUIRES: Arduino OPL2 Library - https://github.com/DhrBaksteen/ArduinoOPL2
 *
 * OPL2 board is connected as follows:
 * Pin  8 - Reset
 * Pin  9 - A0
 * Pin 10 - Latch
 * Pin #  - Data    -MOSI of board being used (orignal Arduino Pin-11, ProMini Pin-16 )
 * Pin #  - Shift   -SCK of board being used (original Arduino Pin-13, ProMini Pin-15 )
 *
 * Code by Jason Hill 2021-10-10 adapted from https://github.com/DhrBaksteen/ArduinoOPL2
 * Pins can be adapted as neccesary, but make sure to use hardware SPI pins for best results.
 * Refer to DhrBaksteen's github wiki for more help wiring it and using his library.
 */


//Optimize for speed over size
#pragma GCC optimize ("-O2")
#pragma GCC push_options

#include <SPI.h>
#include <OPL2.h>
#include "OPL2_SerialPassthrough.h"
#define SerData Serial

OPL2 opl2;
byte state, reg, value, bank, x0, x1, x2;

void setup() {
  SerData.begin(115200);
  opl2.begin();
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
        reg = ((x0 & 0x03) << 6) | ((x1 & 0x7E) >> 1);
        value = ((x1 & 0x01) << 7) | (x2 & 0x7F);
        //opl2.write(reg, value);
        //sendOPL(reg, value);
        fastSendOPL(reg, value);
        state=0;
        break;
      default:
        state=0;
        break;
    }
  }  
}

inline void sendOPL(byte reg, byte value) {
  //Arduino Digital Write is ~3.5us to transititon from high to low
  //on a typical 16Mhz Arduino

  //Transition of PIN_ADDR from low to high ~20us @ 8Mhz SPI (~21us at 4Mhz SI)
  // Write OPL2 address.
  digitalWrite(PIN_ADDR, LOW);
  SPI.transfer(reg);
  digitalWrite(PIN_LATCH, LOW);
  delayMicroseconds(16);
  digitalWrite(PIN_LATCH, HIGH);
  delayMicroseconds(16);

  // Write OPL2 data.
  digitalWrite(PIN_ADDR, HIGH);
  SPI.transfer(value);
  digitalWrite(PIN_LATCH, LOW);
  delayMicroseconds(4);
  digitalWrite(PIN_LATCH, HIGH);
  delayMicroseconds(92);
}

//Fast implementation for specific hardware. See `OPL2_SerialPassthrough.h`
//All PORTx addressed MUST be adapted for other Arduino models.
inline void fastSendOPL(byte reg, byte value) {

  ADDR_LOW;
  
  SPI.transfer(reg);

  LATCH_LOW;
  delayMicroseconds(4);
  LATCH_HIGH;

  ADDR_HIGH;
  
  SPI.transfer(value);
  
  delayMicroseconds(4);
  LATCH_LOW;
  delayMicroseconds(23);
  LATCH_HIGH;
}
