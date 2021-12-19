# dosboxece-opl
DosBoxECE with a hardware OPL implementation for OPL2LPT, OPL3LPT, OPL2 Audio and OPL3 Duo! support.

## Work In Progress
This project is still a work in progress. That said, it should compile and work if all dependencies are met.

### Required libraries
The following librariers are required in addition to those required to build DOSBoxECE vanilla.
 
* `libieee1284-dev`

The current build target is Linux only. This is mainly due to the parallel port requirement.

### TODO
OPL over the LPT port doesn't seem to have many issues. However, there are some hardware combinations of microcontrollers and USB controllers that can cause delays, stutters, or loss of audio due to the use of the serial connection and throughput needed.

Future plans are to also incorporate the use of a FTDI - FT232H USB allowing for general purpose USB to GPIO, SPI, or I2C for lower latency along with higher throughput.

#### Reference Source code and/or inspiration:

* DosBox Enhanced Community Edition
  * https://yesterplay.net/dosboxece/
* OPL2 Audio and OPL3 Duo! Source
  * https://github.com/DhrBaksteen/ArduinoOPL2
* Munt MT-32 emulator
  * https://github.com/munt/munt
* FluidSynth - sound font synthesizer
  * https://github.com/FluidSynth/fluidsynth
* Dosbox-X Source
  * https://github.com/joncampbell123/dosbox-x
* Vincent Bernat's work with OPL2/OPL3LPT
  * https://github.com/vincentbernat/dosbox/branches
  * https://vincent.bernat.ch/en/blog/2018-opl2-audio-board
* tio - Simple TTY terminal
  * https://github.com/tio/tio
 
#### Reference Hardware:

* OPL2LPT and OPL3LPT devices
  * https://www.serdashop.com/OPL2LPT
  * https://www.serdashop.com/OPL3LPT
* OPL2 Audio and OPL3 Duo! audio devices
  * https://www.tindie.com/products/cheerful/opl2-audio-board/
  * https://www.tindie.com/products/cheerful/opl3-duo/
