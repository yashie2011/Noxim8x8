/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __NOXIMPROCESSINGELEMENT_H__
#define __NOXIMPROCESSINGELEMENT_H__


#include <queue>
#include <iomanip>
#include <systemc.h>
#include "NoximMain.h"
#include "NoximGlobalTrafficTable.h"
#include "benchmark.h"
#include "constants.h"
using namespace std;

SC_MODULE(NoximProcessingElement)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the PE
    sc_in < bool > reset;	// The reset signal for the PE

    sc_in < NoximFlit > flit_rx[SLICES];	// The input channel
    sc_in < bool > req_rx[SLICES];	// The request associated with the input channel
    sc_out < bool > ack_rx[SLICES];	// The outgoing ack signal associated with the input channel

    sc_out < NoximFlit > flit_tx[SLICES];	// The output channel
    sc_out < bool > req_tx[SLICES];	// The request associated with the output channel
    sc_in < bool > ack_tx[SLICES];	// The outgoing ack signal associated with the output channel

  //  sc_in < int >free_slots_neighbor;

    // Registers
    int local_id;		// Unique identification number
    bool current_level_rx[SLICES];	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx[SLICES];	// Current level for Alternating Bit Protocol (ABP)
    queue < NoximPacket > packet_queue[SLICES];	// Local queue of packets
    bool transmittedAtPreviousCycle;	// Used for distributions with memory
    int tx_flits[SLICES];
    int rx_flits[SLICES];

    double sent_requests;
    comm temp_comm ;
    vector <comm> ret_comm;
    typedef struct reply_info{
    	double return_time;
    	int data_size;
    	int dest_id;
    	double ack_msg;

    	reply_info(){
    		return_time=0;
    		data_size=0;
    		dest_id = 0;
    		ack_msg = 0;
    	}

    }reply_data;

    vector < reply_data > reply_queue;
    queue <NoximPacket> interface_buf;

   struct compare{
	   bool operator()(reply_data first, reply_data second){
		   return (first.return_time < second.return_time);
	   }
   };

   bool send;
   double last_packet;

    // Functions
    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process
    bool canShot();	// True when the packet must be shot
    NoximFlit nextFlit(int slice);	// Take the next flit of the current packet
   // NoximFlit nextFlit_view();   //  Added to lookup for the flit in the queue
    NoximPacket trafficRandom();	// Random destination distribution
    NoximPacket trafficTranspose1();	// Transpose 1 destination distribution
    NoximPacket trafficTranspose2();	// Transpose 2 destination distribution
    NoximPacket trafficBitReversal();	// Bit-reversal destination distribution
    NoximPacket trafficShuffle();	// Shuffle destination distribution
    NoximPacket trafficButterfly();	// Butterfly destination distribution
    NoximPacket trafficBenchmark(); // Benchmark traffic

    void setUseLowVoltagePath(NoximPacket& packet);

    NoximGlobalTrafficTable *traffic_table;	// Reference to the Global traffic Table
    bool never_transmit;	// true if the PE does not transmit any packet
    //  (valid only for the table based traffic)
    int sim_stop;   // To stop after execution completes
    int computation_time; // Waits for a computation time before sending next request
    int computation_clock;
    bool start_clock;

    void fixRanges(const NoximCoord, NoximCoord &);	// Fix the ranges of the destination
    int randInt(int min, int max);	// Extracts a random integer number between min and max
    int getRandomSize();	// Returns a random size in flits for the packet
    void setBit(int &x, int w, int v);
    int getBit(int x, int w);
    double log2ceil(double x);
    int get_slice(NoximCoord src_coord, NoximCoord dest_coord);
    int get_reply_time();
    void push_packet();
    void sim_stop_poll();
    bool reply_queue_full();
    inline int get_sim_Stop(){ return sim_stop; };

    benchmark              &b_mark;
    // Constructor
    SC_HAS_PROCESS(NoximProcessingElement);
    NoximProcessingElement(sc_module_name name, benchmark &_b_mark):
    	b_mark(_b_mark) {

	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(sim_stop_poll);
	sensitive << reset;
	sensitive << clock.pos();


	send = true;
	last_packet = 0;
	sent_requests=0;

    }

};

#endif
