#include "../serialport/libserial.h"
#include "setup.h"
#include "opl2board.h"

#include <string>
#include "SDL.h"

static int serial_thread(void*);
// Linux headers for comport
extern "C"{
#include <fcntl.h>	// Contains file controls like O_RDWR
#include <termios.h>// Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
extern int setspeed2(int fd, int baudrate);
extern size_t commPortwrite(int* fd, const void *buf, size_t count);
//int readPort(int* serial_port);
}

OPL2AudioBoard::~OPL2AudioBoard(){

}

OPL2AudioBoard::OPL2AudioBoard() {
	goodComm=false;
}

void OPL2AudioBoard::connect(const char* port, long unsigned int* oplbaud) {
	int r;
	
	std::string path="/dev/";
	path.append(port);
	
	//O_RDWR
	fd = open(path.c_str(), O_WRONLY);
	//Save initials settings;
	tcgetattr(fd, &old_tty);
	r = setspeed2(fd, (*oplbaud));

	if(r){
		printf("OPL2 Audio Board failed to open com port.\n");
		close(fd);
	} else {
		printf("OPL2 Audio Board comport %s: opened at %ld baud. (%d)\n",path.c_str(),(*oplbaud),fd);
		goodComm=true;
	}
}

void OPL2AudioBoard::disconnect() {
	if (goodComm) {
		if(tcsetattr(fd, TCSANOW, &old_tty)!=0){
			 printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		}
		close(fd);
		goodComm=false;
	}
}

void OPL2AudioBoard::reset() {
	#if OPL2_AUDIO_BOARD_DEBUG
		printf("OPL2 Audio Board: Reset\n");
	#endif

	for (uint8_t i = 0x00; i < 0xFF; i++) {
		if (i >= 0x40 && i <= 0x55) {
			// Set channel volumes to minimum.
			write(i, 0x3F);
		} else {
			write(i, 0x00);
		}
	}
}

void OPL2AudioBoard::write(uint8_t reg, uint8_t val) {
	if (goodComm) {
		size_t written=0;
		uint8_t bytes[3];

		bytes[0]= (reg >> 6) | 0x80;
		bytes[1]= (((reg & 0x3f) << 1) | (val >> 7)) & 0x7F;
		bytes[2]= (val & 0x7f);
		written=sizeof(bytes);
		
		commPortwrite(&fd, (const void*)bytes, written);
		tcdrain(fd);
		
		#if OPL2_AUDIO_BOARD_DEBUG
			printf("OPL2 Audio Board: Write %d --> %d\n", val, reg);
			//TODO: Remove regVal once testing complete.
			printf("Written: 0x%.8X\n",regVal);
		#endif
	}
}
