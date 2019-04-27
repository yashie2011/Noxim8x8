#ifndef _FC_ROUTER
#define _FC_ROUTER

/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the router
 */


#include <systemc.h>
#include "NoximMain.h"
#include "NoximBuffer.h"
#include "NoximStats.h"
#include "NoximGlobalRoutingTable.h"
#include "NoximLocalRoutingTable.h"
#include "NoximReservationTable.h"
#include <iostream>
#include <fstream>
using namespace std;

extern unsigned int drained_volume;
extern ofstream slice_0_trace;
extern ofstream slice_1_trace;
extern ofstream slice_2_trace;
extern ofstream slice_3_trace;
extern ofstream slice_4_trace;

extern bool fc_buffer_full[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];

SC_MODULE(fc_router)
{

    // I/O Ports
    sc_in_clk clock;		                  // The input clock for the router
    sc_in <bool> reset;                           // The reset signal for the router

    // PE connection ports
    sc_in <NoximFlit> flit_rx;	  // The input channels (including local one)
    sc_in <bool> req_rx;	  // The requests associated with the input channels
    sc_out <bool> ack_rx;	  // The outgoing ack signals associated with the input channels

    sc_out <NoximFlit> flit_tx;   // The output channels (including local one)
    sc_out <bool> req_tx;	  // The requests associated with the output channels
    sc_in <bool> ack_tx;	  // The outgoing ack signals associated with the output channels\

    // Data transfer enable ports
    sc_out <bool> enable_tx[2];
    sc_in <bool> enable_rx[2];

    // flit data ports
    sc_in <NoximFlit> fast_flit_rx[2];	  // The input channels (including local one)
    sc_out <NoximFlit> fast_flit_tx[2];   // The output channels (including local one)

    /*sc_out <int> free_slots[DIRECTIONS + 1];
    sc_in <int> free_slots_neighbor[DIRECTIONS + 1];

    // Neighbor-on-Path related I/O
    sc_out < NoximNoP_data > NoP_data_out[DIRECTIONS];
    sc_in < NoximNoP_data > NoP_data_in[DIRECTIONS]; */

    // Registers

    /*
       NoximCoord position;                     // Router position inside the mesh
     */
    int local_id;		                // Unique ID
    int slice_id;
    double prev_flit;
    double prev_flit_ax;
    NoximBuffer outgoing_buffer;	        // Buffer to store flits from PE
    NoximBuffer incoming_buffer;            // Buffer to store flits going to PE

    bool current_level_rx;	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx;	// Current level for Alternating Bit Protocol (ABP)

    NoximStats stats;		                // Statistics
    NoximLocalRoutingTable routing_table;	// Routing table
    NoximReservationTable reservation_table;	// Switch reservation table

    unsigned long routed_flits;
    int pipeline_latency;               // Added for DA2 router model
    bool send_enable;
    int fast_line;
    // Functions
    double in_flits;

    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process
    void pe_rxprocess();
    void pe_txprocess();

    void configure(const int _id, const double _warm_up_time,
		   const unsigned int _max_buffer_size,
		   NoximGlobalRoutingTable & grt, int slice);

    unsigned long getRoutedFlits();	// Returns the number of routed flits
    unsigned int getFlitsCount();	// Returns the number of flits into the router
    double getPower();		        // Returns the total power dissipated by the router
    inline void set_send_enable(){send_enable = true;};
    inline void reset_send_enable(){send_enable = false;};
    inline void set_fast_line(int x){fast_line = x;};
    inline bool Is_buffer_full(){return incoming_buffer.IsFull();};

    // Constructor

    SC_CTOR(fc_router) {

    incoming_buffer.SetMaxBufferSize(160);
    outgoing_buffer.SetMaxBufferSize(160);
    prev_flit = 0;
    prev_flit_ax = 0;

	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(pe_rxprocess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(pe_txprocess);
	sensitive << reset;
	sensitive << clock.pos();
    }

  private:

    /*
    // performs actual routing + selection
    int route(const NoximRouteData & route_data);

    // wrappers

    vector < int >routingFunction(const NoximRouteData & route_data);*/

  public:

    unsigned int local_drained;
    void ShowBuffersStats(std::ostream & out);
};

#endif
