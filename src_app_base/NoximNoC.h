/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file represents the top-level testbench
 */

#ifndef __NOXIMNOC_H__
#define __NOXIMNOC_H__

#include <systemc.h>
#include "NoximTile.h"
#include "NoximGlobalRoutingTable.h"
#include "NoximGlobalTrafficTable.h"
#include "benchmark.h"
#include "constants.h"

using namespace std;

SC_MODULE(NoximNoC)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the NoC
    sc_in < bool > reset;	// The reset signal for the NoC

    // Signals
    sc_signal <bool> req_to_east[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_west[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_south[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_north[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    sc_signal <bool> ack_to_east[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_west[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_south[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_north[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    sc_signal <NoximFlit> flit_to_east[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <NoximFlit> flit_to_west[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <NoximFlit> flit_to_south[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <NoximFlit> flit_to_north[SLICES][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

   /* sc_signal <int> free_slots_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];

    // NoP
    sc_signal <NoximNoP_data> NoP_data_to_east[MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_west[MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_south[MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_north[MAX_STATIC_DIM][MAX_STATIC_DIM]; */

    // Matrix of tiles
    NoximTile *t[MAX_STATIC_DIM][MAX_STATIC_DIM];
    benchmark *b_marks[MAX_STATIC_DIM][MAX_STATIC_DIM];

    // Global tables
    NoximGlobalRoutingTable grtable;
    NoximGlobalTrafficTable gttable;

    int sim_Stop;
    double error;
    void sim_stop_poller();
    //---------- Mau experiment <start>
    void flitsMonitor() {

	if (!reset.read()) {
	    //      if ((int)sc_simulation_time() % 5)
	    //        return;

	    unsigned int count = 0;
	    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++)
		for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++)
			for(int k=0; k< SLICES; k++)
		    count += t[i][j]->r[k]->getFlitsCount();
	    cout << count << endl;
	}
    }
    //---------- Mau experiment <stop>
    //benchmark   &b_mark;
    // Constructor

    SC_HAS_PROCESS(NoximNoC);
    NoximNoC(sc_module_name name) {

    	SC_METHOD(sim_stop_poller);
    	sensitive(reset);
    	sensitive_pos(clock);
	//---------- Mau experiment <start>
	/*
	   SC_METHOD(flitsMonitor);
	   sensitive(reset);
	   sensitive_pos(clock);
	 */
	//---------- Mau experiment <stop>

	// Build the Mesh
	buildMesh();

    }


    // Support methods
    NoximTile *searchNode(const int id) const;

  private:

    void buildMesh();


};

#endif
