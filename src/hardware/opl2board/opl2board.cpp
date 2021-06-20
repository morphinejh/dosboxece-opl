#include "opl2board.h"
#include <string>

static int serial_thread(void*);

// Linux headers for comport
extern "C"{
#include <fcntl.h>	// Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), close()
extern int setspeed2(int fd, int baudrate);
extern size_t commPortwrite(int* fd, const void *buf, size_t count);
//Not used or tested but included for future
int readPort(int* serial_port);
}

OPL2AudioBoard::~OPL2AudioBoard(){
		SDL_LockMutex(lock);
		SDL_CondSignal(cond);
		SDL_UnlockMutex(lock);
		//Thread already completed before destructor if created. See disconnect()
		SDL_DestroyCond(cond);
		SDL_DestroyMutex(lock);
}

OPL2AudioBoard::OPL2AudioBoard() {
	goodComm=false;
	lock=SDL_CreateMutex();
	cond=SDL_CreateCond();
}

void OPL2AudioBoard::connect(const char* port, long unsigned int* oplbaud) {
	int r;
	
	std::string path="/dev/";
	path.append(port);
	
	//O_RDWR
	fd = open(path.c_str(), O_WRONLY);
	r = setspeed2(fd, (*oplbaud));

	if(r){
		printf("OPL2 Audio Board failed to open com port.\n");
		close(fd);
	} else {
		printf("OPL2 Audio Board comport %s: opened at %ld baud. (%d)\n",path.c_str(),(*oplbaud),fd);
		goodComm=true;
		thread = SDL_CreateThread(serial_thread, this);
	}
}

void OPL2AudioBoard::disconnect() {
	//Ask the thread to quit
	eventQueue.push(EXIT_CODE);
	SDL_WaitThread(thread, nullptr);
	
	if (goodComm) {
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
		uint8_t bytes[3];
		uint32_t regVal32;

		/*This uses the Nuke.YKT protocol
		 * A-Address
		 * B-Data		 * 
		  Bit	7	6	5	4	3	2	1	0
		Byte0	1	0	0	0	A9	A8	A7	A6
		Byte1	0	A5	A4	A3	A2	A1	A0	D7
		Byte2	0	D6	D5	D4	D3	D2	D1	D0
		*Adapted for OPL2 only - see OPL3DuoAudio.cpp for OPL3
		*/
		bytes[0]= (reg >> 6) | 0x80;
		bytes[1]= (((reg & 0x3f) << 1) | (val >> 7)) & 0x7F;
		bytes[2]= (val & 0x7f);
		//Format:
		//[0x00 - 0x00 -	0x00	-	0x00]
		//[EXIT - A9-6 -	A5-0D7	-	D6-0]
		regVal32 = 0;
		regVal32 |= bytes[2];
		regVal32 <<=8;
		regVal32 |= bytes[1];
		regVal32 <<=8;
		regVal32 |= bytes[0];
		
		SDL_LockMutex(lock);
		eventQueue.push(regVal32);
		SDL_CondSignal(cond);
		SDL_UnlockMutex(lock);

		#if OPL2_AUDIO_BOARD_DEBUG
			printf("OPL2 Audio Board: Write %d --> %d\n", val, reg);
		#endif
	}
}

int OPL2AudioBoard::serial_write(){
	uint32_t event;
	uint8_t bytes[3];

	while(true){
		SDL_LockMutex(lock);
		while (eventQueue.empty()) {
			SDL_CondWait(cond, lock);
		}
		event = eventQueue.front();
		eventQueue.pop();
		SDL_UnlockMutex(lock);
		
		if(event & EXIT_CODE){
			break;
		}
		else {
			bytes[0]=0x000000FF&event;
			bytes[1]=0x000000FF&(event>>8);
			bytes[2]=0x000000FF&(event>>16);
			commPortwrite(&fd, (const void*)bytes, sizeof(bytes)),sizeof(bytes);
			tcdrain(fd);
		}
	}
	return 0;
}

static int serial_thread(void* ptr){
	//Pass a pointer to the class object.
	OPL2AudioBoard *self = static_cast<OPL2AudioBoard*>(ptr);
	self->serial_write();
	return 0;
}
