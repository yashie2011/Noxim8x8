/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#ifndef __NOXIMTILE_H__
#define __NOXIMTILE_H__

#include <systemc.h>
#include "NoximRouter.h"
#include "NoximProcessingElement.h"
using namespace std;

SC_MODULE(NoximTile)
{

    // I/O Ports
    sc_in_clk clock;		                // The input clock for the tile
    sc_in <bool> reset;	                        // The reset signal for the tile

    sc_in <NoximFlit> flit_rx[SLICES][DIRECTIONS];	// The input channels
    sc_in <bool> req_rx[SLICES][DIRECTIONS];	        // The requests associated with the input channels
    sc_out <bool> ack_rx[SLICES][DIRECTIONS];	        // The outgoing ack signals associated with the input channels

    sc_out <NoximFlit> flit_tx[SLICES][DIRECTIONS];	// The output channels
    sc_out <bool> req_tx[SLICES][DIRECTIONS];	        // The requests associated with the output channels
    sc_in <bool> ack_tx[SLICES][DIRECTIONS];	        // The outgoing ack signals associated with the output channels

   /* sc_out <int> free_slots[DIRECTIONS];
    sc_in <int> free_slots_neighbor[DIRECTIONS];

    // NoP related I/O
    sc_out < NoximNoP_data > NoP_data_out[DIRECTIONS];
    sc_in < NoximNoP_data > NoP_data_in[DIRECTIONS]; */

    // Signals
    sc_signal <NoximFlit> flit_rx_local[SLICES];	// The input channels
    sc_signal <bool> req_rx_local[SLICES];              // The requests associated with the input channels
    sc_signal <bool> ack_rx_local[SLICES];	        // The outgoing ack signals associated with the input channels

    sc_signal <NoximFlit> flit_tx_local[SLICES];	// The output channels
    sc_signal <bool> req_tx_local[SLICES];	        // The requests associated with the output channels
    sc_signal <bool> ack_tx_local[SLICES];	        // The outgoing ack signals associated with the output channels

    //sc_signal <double> time_stamp_up[DIRECTIONS+1];
    //sc_signal <double> time_stamp_down[DIRECTIONS+1];

    //sc_signal <int> free_slots_local;
    //sc_signal <int> free_slots_neighbor_local;

    /*sc_signal <NoximNoP_data> NoP_data_in_sharing[SLICES];   // Added this to connect the multiple NoP_data ports of routers to single input port of Router
    sc_signal <NoximNoP_data> NoP_data_out_sharing[SLICES];
    sc_signal <int> free_slots_sharing[SLICES];      // Added for sharing the single free slots of the tile between different routers
    sc_signal <int> free_slots_sharing_neighbor[SLICES]; */

    // Instances
    NoximRouter* r[SLICES];		                // Router instance   // Changed for accommodating slices

    NoximProcessingElement *pe;	                // Processing Element instance

    // Constructor
    benchmark              &b_mark;

    NoximTile(sc_module_name name, benchmark &_b_mark):
    	  b_mark(_b_mark){

	// Router pin assignments  -- Looping over the slices
   for (int j = 0; j < SLICES; j++){

	   std::string name = "Router";
	   std::string result;
	   std:stringstream sstm;
	   sstm << name<<j;
	   result = sstm.str();     // interesting library

	r[j] = new NoximRouter("Router");
	r[j]->clock(clock);
	r[j]->reset(reset);
	for (int i = 0; i < DIRECTIONS; i++) {
	    r[j]->flit_rx[i] (flit_rx[j][i]);
	    r[j]->req_rx[i] (req_rx[j][i]);
	    r[j]->ack_rx[i] (ack_rx[j][i]);

	    r[j]->flit_tx[i] (flit_tx[j][i]);
	    r[j]->req_tx[i] (req_tx[j][i]);
	    r[j]->ack_tx[i] (ack_tx[j][i]);

	    /*r[1]->time_stamp_in[i] (time_stamp_up[i]);
	    r[1]->time_stamp_out[i] (time_stamp_down[i]);

	    r[0]->time_stamp_in[i] (time_stamp_down[i]);
	    r[0]->time_stamp_out[i] (time_stamp_up[i]);

	    r[j]->free_slots[i] (free_slots_sharing[i]);
	    r[j]->free_slots_neighbor[i] (free_slots_sharing_neighbor[i]);

	    // NoP 
	    r[j]->NoP_data_out[i] (NoP_data_out_sharing[i]);
	    r[j]->NoP_data_in[i] (NoP_data_in_sharing[i]); */
	}

	r[j]->flit_rx[DIRECTION_LOCAL] (flit_tx_local[j]);
	r[j]->req_rx[DIRECTION_LOCAL] (req_tx_local[j]);
	r[j]->ack_rx[DIRECTION_LOCAL] (ack_tx_local[j]);

	r[j]->flit_tx[DIRECTION_LOCAL] (flit_rx_local[j]);
	r[j]->req_tx[DIRECTION_LOCAL] (req_rx_local[j]);
	r[j]->ack_tx[DIRECTION_LOCAL] (ack_rx_local[j]);

	/*r[1]->time_stamp_in[DIRECTION_LOCAL] (time_stamp_up[DIRECTION_LOCAL]);
    r[1]->time_stamp_out[DIRECTION_LOCAL] (time_stamp_down[DIRECTION_LOCAL]);

    r[0]->time_stamp_in[DIRECTION_LOCAL] (time_stamp_down[DIRECTION_LOCAL]);
    r[0]->time_stamp_out[DIRECTION_LOCAL] (time_stamp_up[DIRECTION_LOCAL]);
	r[j]->free_slots[DIRECTION_LOCAL] (free_slots_local);
	r[j]->free_slots_neighbor[DIRECTION_LOCAL]
	    (free_slots_neighbor_local); */
	}  // Looping through Slices

   // Free-slots and NoP ports assigning
  /* for (int i = 0; i < DIRECTIONS; i++) {
	   this->NoP_data_in[i](NoP_data_in_sharing[i]);
	   this->free_slots[i](free_slots_sharing[i]);
	   this->NoP_data_out[i](NoP_data_out_sharing[i]);
	   this->free_slots_neighbor[i](free_slots_sharing_neighbor[i]);
   } */

	// Processing Element pin assignments
	pe = new NoximProcessingElement("ProcessingElement", b_mark);
	pe->clock(clock);
	pe->reset(reset);
	// LOOPING OVER THE SLICES CONNECTION
	for(int k=0; k< SLICES; k++){
	pe->flit_rx[k](flit_rx_local[k]);
	pe->req_rx[k](req_rx_local[k]);
	pe->ack_rx[k](ack_rx_local[k]);

	pe->flit_tx[k](flit_tx_local[k]);
	pe->req_tx[k](req_tx_local[k]);
	pe->ack_tx[k](ack_tx_local[k]);
	}

	//pe->free_slots_neighbor(free_slots_neighbor_local);

    }

};

#endif
