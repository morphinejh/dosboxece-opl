/*___________________________________________________________ */
/*| Pin definitions for Leonardo(32u4) and ProMicro pinouts. |*/
/*| change pin definitions for your architecture             |*/
/*___________________________________________________________|*/
#define PIN_UNIT  6 //PD7
#define PIN_RESET 9 //PB5

#define PIN_BANK  7 //PE6
#define BANK_HIGH PORTE |= B01000000
#define BANK_LOW  PORTE &= B10111111


#define PIN_ADDR  8 //PB4
#define ADDR_HIGH PORTB |= B00010000
#define ADDR_LOW  PORTB &= B11101111

#define PIN_LATCH 10//PB6
#define LATCH_HIGH PORTB |= B01000000
#define LATCH_LOW  PORTB &= B10111111
/*____________________________________________________________*/


/*___________________________________________________________ */
/*| Function definitions.                                    |*/
/*___________________________________________________________|*/
inline void sendOPL(byte, byte, byte);
inline void fastSendOPL(byte, byte, byte);
