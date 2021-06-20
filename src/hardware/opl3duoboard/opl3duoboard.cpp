#include "opl3duoboard.h"
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

Opl3DuoBoard::Opl3DuoBoard() {
	goodComm=false;
	lock=SDL_CreateMutex();
	cond=SDL_CreateCond();
}

Opl3DuoBoard::~Opl3DuoBoard(){
		SDL_LockMutex(lock);
		SDL_CondSignal(cond);
		SDL_UnlockMutex(lock);
		//Thread already completed before destructor. See disconnect()
		SDL_DestroyCond(cond);
		SDL_DestroyMutex(lock);
}

void Opl3DuoBoard::connect(const char* port, long unsigned int* oplbaud) {
	int r;
	
	std::string path="/dev/";
	path.append(port);
	
	//O_RDWR
	fd = open(path.c_str(), O_WRONLY);
	r = setspeed2(fd, (*oplbaud));

	if(r){
		printf("OPL3Duo Audio Board failed to open com port.\n");
		close(fd);
	} else {
		printf("OPL3Duo Audio Board comport %s: opened at %ld baud. (%d)\n",path.c_str(),(*oplbaud),fd);
		goodComm=true;
		thread = SDL_CreateThread(serial_thread, this);
	}
}

void Opl3DuoBoard::disconnect() {
	//Ask the thread to quit
	eventQueue.push(EXIT_CODE);
	SDL_WaitThread(thread, nullptr);
	
	if (goodComm) {
		close(fd);
		goodComm=false;
	}
}

void Opl3DuoBoard::reset() {
	#if OPL3_DUO_BOARD_DEBUG
		printf("OPL3 Duo! Board: Reset\n");
	#endif
	
    write(0x105, 0x01);     // Enable OPL3 mode
    write(0x104, 0x00);     // Disable all 4-op channels

    // Clear all registers of banks 0 and 1
    for (uint16_t i = 0x000; i <= 0x100; i += 0x100) {
        for(uint16_t j = 0x20; j <= 0xF5; j++) {
            write(i + j, j >= 0x40 && j <= 0x55 ? 0xFF : 0x00);
        }
    }

    write(0x105, 0x00);     // Disable OPL3 mode
    write(0x01, 0x00);      // Clear waveform select
    write(0x08, 0x00);      // Clear CSW and N-S
}

void Opl3DuoBoard::write(uint32_t reg, uint8_t val) {
	
	//reg is 32 bit. 
	
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
		
		//For OPL3 need to set bit A8 and|or A9.
		//A8 determines the bank of the OPL3 chip. For OPL2 this bit is always 0.
		//A9 determines the synth unit of the OPL3 Duo. 0 addresses the left chip, 1 addresses the right chip.
		
		//bytes[0] = (lastRegAddr >> 6) & 0x7;
		//bytes[0] |= 0x80;
		bytes[0] = (reg >> 6) & 0x07;
		bytes[0]|= 0x80;
		bytes[1]= (((reg & 0x3f) << 1) | (val >> 7)) & 0x7F;
		bytes[2]= (val & 0x7f);
		//bytes[1]= (((lastRegAddr & 0x3f) << 1) | (val >> 7)) & 0x7F;
		//bytes[2]= (val & 0x7f);
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
		//printf("Pushed: 0x%.2X, 0x%.2X | 0x%.4X, 0x%.4X, 0x%.4X\n", reg, val, bytes[0],bytes[1], bytes[2]);
		SDL_CondSignal(cond);
		SDL_UnlockMutex(lock);

		#if OPL2_AUDIO_BOARD_DEBUG
			printf("OPL3Duo Audio Board: Write %d --> %d\n", val, reg);
		#endif
	}
}

int Opl3DuoBoard::serial_write(){
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
			//printf("Popped: 0x%.4X, 0x%.4X, 0x%.4X\n", bytes[0],bytes[1], bytes[2]);
			commPortwrite(&fd, (const void*)bytes, sizeof(bytes)),sizeof(bytes);
			tcdrain(fd);
		}
	}
	return 0;
}

static int serial_thread(void* ptr){
	//Pass a pointer to the class object.
	Opl3DuoBoard *self = static_cast<Opl3DuoBoard*>(ptr);
	self->serial_write();
	return 0;
}
