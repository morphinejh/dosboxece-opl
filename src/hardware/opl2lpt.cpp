/*
 *  Copyright (C) 2002-2017  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ieee1284.h>
#include <math.h>
#include <unistd.h>

#include "adlib.h"
#include "dosbox.h"
#include "cpu.h"
#include "opl2lpt.h"

#define C_DEBUG 0
#define byte uint8_t

#define PP_NOT_STROBE   0x1
#define PP_NOT_AUTOFD   0x2
#define PP_INIT         0x4
#define PP_NOT_SELECT   0x8

uint16_t lastAddr=0;

const uint8_t a0_0 = PP_NOT_SELECT | PP_NOT_STROBE | PP_INIT;
const uint8_t a0_1 = PP_NOT_SELECT | PP_NOT_STROBE; //No PP_INIT means write something.
const uint8_t a0_2 = PP_NOT_SELECT | PP_NOT_STROBE | PP_INIT;

const uint8_t a1_0 = PP_NOT_STROBE | PP_INIT;
const uint8_t a1_1 = PP_NOT_STROBE;
const uint8_t a1_2 = PP_NOT_STROBE | PP_INIT;

const uint8_t d_0  = PP_NOT_SELECT | PP_INIT;
const uint8_t d_1  = PP_NOT_SELECT;
const uint8_t d_2  = PP_NOT_SELECT | PP_INIT;
	
#if C_OPL2LPT

static void opl2lpt_shutdown(struct parport *pport) {
	if (pport) {
		ieee1284_close(pport);
	}
}

static void opl2lpt_lpt_write(struct parport *pport, Bit16u addr, Bit8u data){

#if C_DEBUG
		LOG_MSG("--direct----    Addr 0x%" PRIx16 ", data 0x%" PRIx8, addr, data);
#endif
	
	//Only need to change A1 for writing >0xff address, not data.
	ieee1284_write_data(pport, addr);
	if (addr < 0x100) {
		//Register Address for OPL2
		ieee1284_write_control(pport, a0_0 ^ C1284_INVERTED);
		ieee1284_write_control(pport, a0_1 ^ C1284_INVERTED);
		ieee1284_write_control(pport, a0_2 ^ C1284_INVERTED);
	} else {
		//Port Address for OPl3
		ieee1284_write_control(pport, a1_0 ^ C1284_INVERTED);
		ieee1284_write_control(pport, a1_1 ^ C1284_INVERTED);
		ieee1284_write_control(pport, a1_2 ^ C1284_INVERTED);
	}

	ieee1284_write_data(pport, data);
	ieee1284_write_control(pport, d_0 ^ C1284_INVERTED);
	ieee1284_write_control(pport, d_1 ^ C1284_INVERTED);
	ieee1284_write_control(pport, d_2 ^ C1284_INVERTED);
}

static void opl2lpt_reset(struct parport *pport, Adlib::Mode mode) {
	LOG_MSG("OPL2LPT: reset %s chip", mode == Adlib::MODE_OPL2 ? "OPL2" : "OPL3");
	for (int i = 0; i < 256; i ++) {
		opl2lpt_lpt_write(pport, i, 0);
	}
	if (mode == Adlib::MODE_OPL3) {
		for (int i = 256; i < 512; i ++) {
			opl2lpt_lpt_write(pport, i, 0);
		}
	}
}

static struct parport *opl2lpt_init(std::string name, Adlib::Mode mode) {
	struct parport_list parports = {};
	struct parport *pport;

	LOG_MSG("OPL2LPT: name given: %s", name.c_str());

	// Look for available parallel ports
	if (ieee1284_find_ports(&parports, 0) != E1284_OK) {
		LOG_MSG("OPL2LPT: cannot find parallel ports");
		return nullptr;
	}
	
	for (int i = 0; i < parports.portc; i++) {
		if (name == "" || name == parports.portv[i]->name) {
			int caps = CAP1284_RAW;
			pport = parports.portv[i];
			if (ieee1284_open(pport, 0, &caps) != E1284_OK) {
				LOG_MSG("OPL2LPT: cannot open parallel port %s", pport->name);
			}
			if (ieee1284_claim(pport) != E1284_OK) {
				LOG_MSG("OPL2LPT: cannot claim parallel port %s", pport->name);
				ieee1284_close(pport);
				continue;
			}
			opl2lpt_reset(pport, mode);
			LOG_MSG("OPL2LPT: found parallel port: %s", pport->name);
			ieee1284_free_ports(&parports);
			return pport;
		}
	}
	ieee1284_free_ports(&parports);
	LOG_MSG("OPL2LPT: cannot find parallel port %s", name.c_str());
	return nullptr;
}

static int opl2lpt_thread(void *ptr) {
	OPL2LPT::Handler *self = static_cast<OPL2LPT::Handler *>(ptr);
	self->WriteThread();
	return 0;
}

namespace OPL2LPT {

	const Bit32u EventQuit = 0x80000000;
	const Bit32u EventAddr = 0x20000000;
	const Bit32u EventReg  = 0x30000000;

	Bit32u combineVal;

	void Handler::WriteReg(Bit32u reg, Bit8u val) {
		combineVal  =(reg << 8);
		combineVal &=0xFFFFF00;
		combineVal |= val;

		SDL_LockMutex(lock);
		eventQueue.push(EventReg | combineVal);
		SDL_CondSignal(cond);
		SDL_UnlockMutex(lock);
	}

	Bit32u Handler::WriteAddr(Bit32u port, Bit8u val) {
		/*Certain port numbers mean Register/Address is above 0xFF in YMF262.
		  Adlib Gold and Soundblaster 16 YMF262 ports "Advanced FM"
		*/
		uint32_t reg = val;
		
		if( port==0x38a || port==0x222 || port==0x242 || port==0x262 || port==0x282){
			reg |= 0x100;
		}
		
		return reg;
	}

	void Handler::Generate(MixerChannel* chan, Bitu samples) {
		/* No need to generate sound */
		chan->enabled = false;
		return;
	}

	int Handler::WriteThread() {
		struct parport *pport = opl2lpt_init(pportName, mode);
		Bit32u event;
		Bit16u addr;
		Bit8u data;
		
		while (true) {
			SDL_LockMutex(lock);
			while (eventQueue.empty()) {
				SDL_CondWait(cond, lock);
			}
			event = eventQueue.front();
			eventQueue.pop();
			SDL_UnlockMutex(lock);

			if ((event & 0x80000000) == EventQuit) {
				LOG_MSG("OPL2LPT: quit sound thread");
				break;
			}
			
			addr = (event & 0x1FF00)>>8;
			data = event & 0xFF;
			
			switch (event & 0xf0000000) {
			case EventReg:
				opl2lpt_lpt_write(pport, addr, data);
				break;
			case EventAddr:
			default:
				LOG_MSG("OPL2LPT: got unknown event 0x%" PRIx16, event);
			}
		}

		if (pport) {
			opl2lpt_reset(pport, mode);
			opl2lpt_shutdown(pport);
		}
		return 0;
	}

	void Handler::Init(Bitu rate) {
		thread = SDL_CreateThread(opl2lpt_thread, this);
		if (!thread) {
			LOG_MSG("OPL2LPT: unable to create thread: %s", SDL_GetError());
		}
	}

	Handler::Handler(std::string name, Adlib::Mode mode) : pportName(name), mode(mode), lock(SDL_CreateMutex()), cond(SDL_CreateCond()) {
		LOG_MSG("OPL2LPT: name passed: %s", name.c_str());
	}

	Handler::~Handler() {
		SDL_LockMutex(lock);
		eventQueue.push(EventQuit);
		SDL_CondSignal(cond);
		SDL_UnlockMutex(lock);
		SDL_WaitThread(thread, nullptr);
		SDL_DestroyCond(cond);
		SDL_DestroyMutex(lock);
	}

};

#endif
