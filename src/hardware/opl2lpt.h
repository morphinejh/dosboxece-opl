/* -*- c++ -*- */
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

#if C_OPL2LPT

#include <queue>

#include "SDL_thread.h"
#include "adlib.h"
#include "dosbox.h"

namespace OPL2LPT {

	struct Handler : public Adlib::Handler {
	private:
		std::string pportName;
		Adlib::Mode mode;
		/* Thread management for OPL2LPT */
		SDL_Thread *thread;
		SDL_mutex *lock;
		SDL_cond *cond;
		std::queue<Bit32u> eventQueue;

	public:
		virtual Bit32u WriteAddr( Bit32u port, Bit8u val );
		virtual void WriteReg( Bit32u addr, Bit8u val );
		virtual void Generate( MixerChannel* chan, Bitu samples );
		virtual void Init( Bitu rate );
		int WriteThread();
		explicit Handler(std::string name, Adlib::Mode mode);
		~Handler();
	};

};		//Namespace

#endif
