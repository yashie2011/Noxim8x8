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


#include <deque>
#include <queue>
#include <iomanip>
#include <systemc.h>
#include "NoximMain.h"
#include "NoximGlobalTrafficTable.h"
//#include "benchmark.h"
#include "constants.h"
#include "db_node.h"
#include "NoximPower.h"

#define DRAM_BANK_QUEUES 4

using namespace std;

class memory_controller{

private:
	int tCAS;
	int tRAS;
	int tRCD;
	int max_queue_depth;
	//vector<std::pair<int, int>> active_bank_rows;
	int last_bank_q;


public:
	vector< std::queue<NoximPacket> > bank_queues;
	memory_controller();
	~memory_controller();
	bool get_packets(deque <NoximPacket>& interface_buf);
	bool push_packet(NoximFlit hd_flit);

};


class NoximProcessingElement : public sc_module
{

public:
    // I/O Ports
    sc_in_clk clock;		// The input clock for the PE
    sc_in < bool > reset;	// The reset signal for the PE

    sc_in < NoximFlit > flit_rx ;	// The input channel
    sc_in < bool > req_rx ;	// The request associated with the input channel
    sc_out < bool > ack_rx ;	// The outgoing ack signal associated with the input channel

    sc_out < NoximFlit > flit_tx ;	// The output channel
    sc_out < bool > req_tx ;	// The request associated with the output channel
    sc_in < bool > ack_tx ;	// The outgoing ack signal associated with the output channel

  //  sc_in < int >free_slots_neighbor;

    // Registers
    int local_id;		// Unique identification number
    bool current_level_rx ;	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx ;	// Current level for Alternating Bit Protocol (ABP)
    queue < NoximPacket > packet_queue ;	// Local queue of packets
    bool transmittedAtPreviousCycle;	// Used for distributions with memory
    int tx_flits ;
    int rx_flits ;



    double sent_packets;
    double received_packets;
    comm temp_comm ;
    vector <comm> ret_comm;
    typedef struct reply_info{
    	double return_time;
    	int data_size;
    	int dest_id;
    	double ack_msg;
    	bool approximable;
    	int row,col,bank;

    	reply_info(){
    		return_time=0;
    		data_size=0;
    		dest_id = 0;
    		ack_msg = 0;
    		row = 0;
    		col = 0;
    		bank = 0;
    	}

    }reply_data;

    struct core_id_map *cmap;
    SQLiteDB* trace_db_p;
    memory_controller *mem_c;

    vector < reply_data > reply_queue;
    deque <NoximPacket> interface_buf;

   struct compare{
	   bool operator()(reply_data first, reply_data second){
		   return (first.return_time < second.return_time);
	   }
   };

   bool send;
   double last_packet;
   NoximPower pe_pwr;

    // Functions
    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process
    bool canShot();	// True when the packet must be shot
    NoximFlit nextFlit();	// Take the next flit of the current packet
   // NoximFlit nextFlit_view();   //  Added to lookup for the flit in the queue
    NoximPacket trafficRandom();	// Random destination distribution
    NoximPacket trafficTranspose1();	// Transpose 1 destination distribution
    NoximPacket trafficTranspose2();	// Transpose 2 destination distribution
    NoximPacket trafficBitReversal();	// Bit-reversal destination distribution
    NoximPacket trafficShuffle();	// Shuffle destination distribution
    NoximPacket trafficButterfly();	// Butterfly destination distribution
    NoximPacket trafficBenchmark(); // Benchmark traffic
    NoximPacket trafficDB();

    void setUseLowVoltagePath(NoximPacket& packet);

    NoximGlobalTrafficTable *traffic_table;	// Reference to the Global traffic Table
    bool never_transmit;	// true if the PE does not transmit any packet
    //  (valid only for the table based traffic)

    int computation_clock;
    bool start_clock;
    double num_reqs;
    int max_buffer_size;

    void fixRanges(const NoximCoord, NoximCoord &);	// Fix the ranges of the destination
    int randInt(int min, int max);	// Extracts a random integer number between min and max
    int getRandomSize();	// Returns a random size in flits for the packet
    void setBit(int &x, int w, int v);
    int getBit(int x, int w);
    double log2ceil(double x);
    int get_reply_time();
    void push_packet();
    //void sim_stop_poll();
    bool reply_queue_full();
    inline double get_sent_packets(){ return sent_packets; };
    inline double get_recv_packets(){return received_packets;};
    int get_reply_queue_size();
    double get_num_reqs(){return num_reqs; };
    void reset_num_reqs(){num_reqs = 0;};
    NoximPacket get_packet_from_db(SQLiteDB* PSQLite);
    memc_data* get_mem_comm_from_trace( double ts, const char *table_name);
    virtual void end_of_simulation();
    void approximate(NoximPacket& pkt, deque <NoximPacket>& interface_buf); // To approximate the traffic at MC

    // Constructor
    SC_HAS_PROCESS(NoximProcessingElement);
    NoximProcessingElement(sc_module_name name, SQLiteDB* trace_db_p):
    	trace_db_p(trace_db_p) {

    	send = true;
    	last_packet = 0;
    	sent_packets=0;
    	received_packets = 0;
    	never_transmit = false;
    	start_clock = false;
    	transmittedAtPreviousCycle = false;
    	cmap = new core_id_map;
    	max_buffer_size = 0;
    	mem_c = new memory_controller;


	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();


    }

    ~NoximProcessingElement(){
    	delete cmap;
    	delete mem_c;
    }
};

#endif
