extern "C"{
#include <termios.h> // Contains POSIX terminal control definitions
}

#include <queue>
#include "SDL_thread.h"

#define EXIT_CODE 0xFF000000

#ifndef OPL3_DUO_BOARD
    #define OPL3_DUO_BOARD
	// Output debug information to the DosBox console if set to 1
	#define OPL3_DUO_BOARD_DEBUG 1

	class Opl3DuoBoard {
		public:
			Opl3DuoBoard();
			void connect(const char* port, long unsigned int* oplbaud);
			void disconnect();
			void reset();
			void write(uint32_t reg, uint8_t val);
			int serial_write();
			~Opl3DuoBoard();

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
