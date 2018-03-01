/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __TOEKNISSUER_H__
#define __TOKENISSUER_H__

#include <mutex>
#include "constants.h"
#include "NoximMain.h"


using namespace std;

//typedef pair<int, int> reservation;

class token_issuer{
	// static arrays are not optimum. Always use dynamic arrays, best way to manage memory

public:
	static int tokens [SLICES][MEM_CONTROLLERS];
	
	std::mutex mtx;

	bool set_token(int memc_id, NoximCoord src_coord, int core_id);
	
	bool reset_token(int memc_id, NoximCoord src_coord);

	token_issuer();

};

#endif
