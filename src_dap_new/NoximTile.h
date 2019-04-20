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
#include "fc_router.h"
#include "db_node.h"
using namespace std;

SC_MODULE(NoximTile)
{

    // I/O Ports
    sc_in_clk clock;		                // The input clock for the tile
    sc_in <bool> reset;	                        // The reset signal for the tile

    // I/O ports for slice 0
    sc_in <NoximFlit> flit_rx[DIRECTIONS];	// The input channels
    sc_in <bool> req_rx[DIRECTIONS];	        // The requests associated with the input channels
    sc_out <bool> ack_rx[DIRECTIONS];	        // The outgoing ack signals associated with the input channels

    sc_out <NoximFlit> flit_tx[DIRECTIONS];	// The output channels
    sc_out <bool> req_tx[DIRECTIONS];	        // The requests associated with the output channels
    sc_in <bool> ack_tx [DIRECTIONS];	        // The outgoing ack signals associated with the output channels

    // I/O ports for slice 1

    sc_in <NoximFlit>fc_flit_rx[2];
    sc_out <NoximFlit>fc_flit_tx[2];
    sc_in <bool> enable_rx[2];
    sc_out<bool> enable_tx[2];

   /* sc_out <int> free_slots[DIRECTIONS];
    sc_in <int> free_slots_neighbor[DIRECTIONS];

    // NoP related I/O
    sc_out < NoximNoP_data > NoP_data_out[DIRECTIONS];
    sc_in < NoximNoP_data > NoP_data_in[DIRECTIONS]; */

    // Signals
    sc_signal <NoximFlit> flit_rx_local;	// The input channels
    sc_signal <bool> req_rx_local;              // The requests associated with the input channels
    sc_signal <bool> ack_rx_local;	        // The outgoing ack signals associated with the input channels

    sc_signal <NoximFlit> flit_tx_local;	// The output channels
    sc_signal <bool> req_tx_local;	        // The requests associated with the output channels
    sc_signal <bool> ack_tx_local;	        // The outgoing ack signals associated with the output channels

    //fc_Signals
    sc_signal <NoximFlit> fc_flit_rx_local;	// The input channels
     sc_signal <bool> fc_req_rx_local;              // The requests associated with the input channels
     sc_signal <bool> fc_ack_rx_local;	        // The outgoing ack signals associated with the input channels

     sc_signal <NoximFlit> fc_flit_tx_local;	// The output channels
     sc_signal <bool> fc_req_tx_local;	        // The requests associated with the output channels
     sc_signal <bool> fc_ack_tx_local;	        // The outgoing ack signals associated with the output channels


    //sc_signal <int> free_slots_local;
    //sc_signal <int> free_slots_neighbor_local;

    /*sc_signal <NoximNoP_data> NoP_data_in_sharing[SLICES];   // Added this to connect the multiple NoP_data ports of routers to single input port of Router
    sc_signal <NoximNoP_data> NoP_data_out_sharing[SLICES];
    sc_signal <int> free_slots_sharing[SLICES];      // Added for sharing the single free slots of the tile between different routers
    sc_signal <int> free_slots_sharing_neighbor[SLICES]; */

    // Instances
    NoximRouter* r;		                // Router instance   // Changed for accommodating slices
    fc_router* fcr;

    NoximProcessingElement *pe;	                // Processing Element instance

    // Constructor
    SQLiteDB *trace_db_p;

    NoximTile(sc_module_name name, SQLiteDB *_trace_db_p):
    	  trace_db_p(_trace_db_p){

	// Router pin assignments  -- Looping over the slices
   for (int j = 0; j < 1; j++){

	   std::string name = "Router";
	   std::string result;
	   std:stringstream sstm;
	   sstm << name<<j;
	   result = sstm.str();     // interesting library

	r = new NoximRouter("Router");
	r->clock(clock);
	r->reset(reset);
	for (int i = 0; i < DIRECTIONS; i++) {
	    r->flit_rx[i] (flit_rx[i]);
	    r->req_rx[i] (req_rx[i]);
	    r->ack_rx[i] (ack_rx[i]);

	    r->flit_tx[i] (flit_tx[i]);
	    r->req_tx[i] (req_tx[i]);
	    r->ack_tx[i] (ack_tx[i]);

	    /*r[j]->free_slots[i] (free_slots_sharing[i]);
	    r[j]->free_slots_neighbor[i] (free_slots_sharing_neighbor[i]);

	    // NoP 
	    r[j]->NoP_data_out[i] (NoP_data_out_sharing[i]);
	    r[j]->NoP_data_in[i] (NoP_data_in_sharing[i]); */
	}

	r->flit_rx[DIRECTION_LOCAL] (flit_tx_local);
	r->req_rx[DIRECTION_LOCAL] (req_tx_local);
	r->ack_rx[DIRECTION_LOCAL] (ack_tx_local);

	r->flit_tx[DIRECTION_LOCAL] (flit_rx_local);
	r->req_tx[DIRECTION_LOCAL] (req_rx_local);
	r->ack_tx[DIRECTION_LOCAL] (ack_rx_local);

	/*r[j]->free_slots[DIRECTION_LOCAL] (free_slots_local);
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

   // fc_router of slice 1 pin assignments
   fcr = new fc_router("fc_router");
   fcr->clock(clock);
   fcr->reset(reset);
   for(int i=0; i<SLICES; i++){
	   fcr->fast_flit_rx[i](fc_flit_rx[i]);
	   fcr->fast_flit_tx[i](fc_flit_tx[i]);
	   fcr->enable_rx[i](enable_rx[i]);
	   fcr->enable_tx[i](enable_tx[i]);
   }
   fcr->flit_tx(fc_flit_rx_local);
   fcr->req_tx(fc_req_rx_local);
   fcr->ack_tx(fc_ack_rx_local);

   fcr->flit_rx(fc_flit_tx_local);
   fcr->req_rx(fc_req_tx_local);
   fcr->ack_rx(fc_ack_tx_local);


	// Processing Element pin assignments

	pe = new NoximProcessingElement("ProcessingElement", trace_db_p);
	pe->clock(clock);
	pe->reset(reset);
	// pe connections with normal router of slice 0
	pe->flit_rx[0](flit_rx_local);
	pe->req_rx[0](req_rx_local);
	pe->ack_rx[0](ack_rx_local);

	pe->flit_tx[0](flit_tx_local);
	pe->req_tx[0](req_tx_local);
	pe->ack_tx[0](ack_tx_local);

	//pe connections with fc_router of slice 1
	pe->flit_rx[1](fc_flit_rx_local);
	pe->req_rx[1](fc_req_rx_local);
	pe->ack_rx[1](fc_ack_rx_local);

	pe->flit_tx[1](fc_flit_tx_local);
	pe->req_tx[1](fc_req_tx_local);
	pe->ack_tx[1](fc_ack_tx_local);


	//pe->free_slots_neighbor(free_slots_neighbor_local);

    }

};

#endif
