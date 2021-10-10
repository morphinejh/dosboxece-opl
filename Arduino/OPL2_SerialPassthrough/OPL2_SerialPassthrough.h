/*___________________________________________________________ */
/*| Pin definitions for Leonardo(32u4) and ProMicro pinouts. |*/
/*| change pin definitions for your architecture             |*/
/*___________________________________________________________|*/
#define PIN_RESET  8//PB4

#define PIN_ADDR   9//PB5
#define ADDR_HIGH PORTB |= B00100000
#define ADDR_LOW  PORTB &= B11011111

#define PIN_LATCH 10//PB6
#define LATCH_HIGH PORTB |= B01000000
#define LATCH_LOW  PORTB &= B10111111
/*____________________________________________________________*/


/*___________________________________________________________ */
/*| Function definitions.                                    |*/
/*___________________________________________________________|*/
inline void sendOPL(byte, byte);
inline void fastSendOPL(byte, byte);
