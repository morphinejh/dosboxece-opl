extern "C"{
#include <termios.h> // Contains POSIX terminal control definitions
}

#include <queue>
#include "SDL_thread.h"

#define EXIT_CODE 0xFF000000

#ifndef OPL2_AUDIO_BOARD
	#define OPL2_AUDIO_BOARD
	// Output debug information to the DosBox console if set to 1
	#define OPL2_AUDIO_BOARD_DEBUG 0

	class OPL2AudioBoard {
		public:
			OPL2AudioBoard();
			void connect(const char* port, long unsigned int* oplbaud);
			void disconnect();
			void reset();
			void write(uint8_t reg, uint8_t val);
			int serial_write();
			~OPL2AudioBoard();
			
		private:
			termios old_tty;
			int fd;
			bool goodComm;
			/* Serial write function for thread */
			SDL_Thread *thread;
			SDL_mutex *lock;
			SDL_cond *cond;
			std::queue<uint32_t> eventQueue;
	};
#endif
