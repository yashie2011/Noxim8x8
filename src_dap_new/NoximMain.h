/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
 */

#ifndef __NOXIMMAIN_H__
#define __NOXIMMAIN_H__

#include <cassert>
#include <systemc.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "benchmark.h"
#include <map>
#include <deque>

using namespace std;

// Define the directions as numbers
#define DIRECTIONS             4
#define SLICES				   2      // Added this for base work implementation
#define DIRECTION_NORTH        0
#define DIRECTION_EAST         1
#define DIRECTION_SOUTH        2
#define DIRECTION_WEST         3
#define DIRECTION_LOCAL        4

// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1
#define PACKET_DROP           -1
// Routing algorithms
#define ROUTING_XY             0
#define ROUTING_WEST_FIRST     1
#define ROUTING_NORTH_LAST     2
#define ROUTING_NEGATIVE_FIRST 3
#define ROUTING_ODD_EVEN       4
#define ROUTING_DYAD           5
#define ROUTING_FULLY_ADAPTIVE 8
#define ROUTING_TABLE_BASED    9
#define INVALID_ROUTING       -1

// Selection strategies
#define SEL_RANDOM             0
#define SEL_BUFFER_LEVEL       1
#define SEL_NOP                2
#define INVALID_SELECTION     -1

// Traffic distribution
#define TRAFFIC_RANDOM         0
#define TRAFFIC_TRANSPOSE1     1
#define TRAFFIC_TRANSPOSE2     2
#define TRAFFIC_HOTSPOT        3
#define TRAFFIC_TABLE_BASED    4
#define TRAFFIC_BIT_REVERSAL   5
#define TRAFFIC_SHUFFLE        6
#define TRAFFIC_BUTTERFLY      7
#define TRAFFIC_BENCHMARK	   8
#define INVALID_TRAFFIC       -1

// Verbosity levels
#define VERBOSE_OFF            0
#define VERBOSE_LOW            1
#define VERBOSE_MEDIUM         2
#define VERBOSE_HIGH           3

// Default configuration can be overridden with command-line arguments
#define DEFAULT_VERBOSE_MODE                     VERBOSE_OFF
#define DEFAULT_TRACE_MODE                             false
#define DEFAULT_TRACE_FILENAME                            ""
#define DEFAULT_MESH_DIM_X                                 8
#define DEFAULT_MESH_DIM_Y                                 8
#define DEFAULT_BUFFER_DEPTH                               8
#define DEFAULT_MAX_PACKET_SIZE                            5     // In layer-1 the packets can have as many as 5 flits per packet
#define DEFAULT_MIN_PACKET_SIZE                            2
#define DEFAULT_PACKET_SIZE_MC							10    // in layer-1 from MC
#define DEFAULT_PACKET_SIZE_MC_SLICE					40    // In layers-2 from MC
#define DEFAULT_ROUTING_ALGORITHM                 ROUTING_XY
#define DEFAULT_ROUTING_TABLE_FILENAME                    ""
#define DEFAULT_SELECTION_STRATEGY                SEL_RANDOM
#define DEFAULT_PACKET_INJECTION_RATE                   0.01
#define DEFAULT_PROBABILITY_OF_RETRANSMISSION           0.01
#define DEFAULT_TRAFFIC_DISTRIBUTION          TRAFFIC_BENCHMARK
#define DEFAULT_TRAFFIC_TABLE_FILENAME                    ""
#define DEFAULT_RESET_TIME                              1000
#define DEFAULT_SIMULATION_TIME                        10000
#define DEFAULT_STATS_WARM_UP_TIME        DEFAULT_RESET_TIME
#define DEFAULT_DETAILED                               false
#define DEFAULT_DYAD_THRESHOLD                           0.6
#define DEFAULT_MAX_VOLUME_TO_BE_DRAINED                   0
#define DEFAULT_ROUTER_PWR_FILENAME     "default_router.pwr"
#define DEFAULT_LOW_POWER_LINK_STRATEGY                false
#define DEFAULT_QOS                                      1.0
#define DEFAULT_SHOW_BUFFER_STATS                      false
#define GAM_COMPUTE_TIME 								1000
#define DEFAULT_LOGGING_FLAG							true
#define DEFAULT_ADDR_MIN								2147745792
#define DEFAULT_ADDR_MAX								2148832224
#define DEFAULT_CHECK_DEPTH									6
#define DEFAULT_BENCH_NAME									"trace_dct.db"
#define DEFAULT_FILE_PATH                                   "/home/yaswanth/Codes/traces_db/"
#define DEFAULT_APPROX_RATE									5
#define DEFAULT_BANK_QUEUES									4

// TODO by Fafa - this MUST be removed!!! Use only STL vectors instead!!!
#define MAX_STATIC_DIM 32

typedef unsigned int uint;

// NoximGlobalParams -- used to forward configuration to every sub-block
struct NoximGlobalParams {
    static int verbose_mode;
    static int trace_mode;
    static char trace_filename[128];
    static int mesh_dim_x;
    static int mesh_dim_y;
    static int buffer_depth;
    static int min_packet_size;
    static int max_packet_size;
    static int routing_algorithm;
    static char routing_table_filename[128];
    static int selection_strategy;
    static float packet_injection_rate;
    static float probability_of_retransmission;
    static int traffic_distribution;
    static char traffic_table_filename[128];
    static int simulation_time;
    static int stats_warm_up_time;
    static int rnd_generator_seed;
    static bool detailed;
    static vector <pair <int, double> > hotspots;
    static float dyad_threshold;
    static unsigned int max_volume_to_be_drained;
    static char router_power_filename[128];
    static bool low_power_link_strategy;
    static double qos;
    static bool show_buffer_stats;
    static bool show_log;
    // Dapper journal config
    static char bench_name[128];
    static char file_path[128];
    static double address_min;
    static double address_max;
    static int check_depth;
    static float approx_rate;
    static int bank_queues;
};

// NoximCoord -- XY coordinates type of the Tile inside the Mesh
class NoximCoord {
  public:
    int x;			// X coordinate
    int y;			// Y coordinate

    inline bool operator ==(const NoximCoord & coord) const {
	return (coord.x == x && coord.y == y);
}};

// NoximFlitType -- Flit type enumeration
struct NoximFlitType {
    enum value {FLIT_TYPE_HEAD = 0, FLIT_TYPE_BODY, FLIT_TYPE_TAIL};
};

// Memory controller tile id
enum mem_controllers {m1 = 1, m2 = 6, m3 = 8, m4 = 15, m5 = 48, m6 = 55, m7 = 57, m8 = 62};


// creating a map for core ids and mem controller ids to trace ids
struct core_id_map{
public:
	int mem_ctrls[MEM_CONTROLLERS] = {1, 6, 8, 15, 48, 55, 57, 62};
    int core_ids[NUM_CORES] =  {0, 2, 3, 4, 5, 7, 9, 10,
    				11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    				26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    				40, 41, 42, 43, 44, 45, 46, 47, 49, 50, 51, 52, 53, 54,
    				56, 58, 59, 60, 61, 63};
    std::map<int, int> core_map;
    std::map<int, int> mem_map;
    int core_map_size, mem_map_size;

    core_id_map(){
    	core_map_size = sizeof(core_ids)/sizeof(int);
    	mem_map_size = sizeof(mem_ctrls)/sizeof(int);

    	for (int i =0; i < NUM_CORES ; i++){
    		if(core_map_size  > i)
    		core_map.insert(std::pair<int, int>(core_ids[i], i));
    	}
    	for (int i =0; i < MEM_CONTROLLERS ; i++){
			if(mem_map_size > i)
			mem_map.insert(std::pair<int, int>(i, mem_ctrls[i]));
    	}
    }

    inline int get_trace_core_id(int local_id){
    	map<int, int>::iterator it ;
    	it = core_map.find(local_id);
    	if(it != core_map.end())	return it->second;
    	else return -1;
    }

    inline int get_chip_mcid(int local_id){
		map<int, int>::iterator it ;
		it = mem_map.find(local_id);
		if (it != mem_map.end()) return mem_map[local_id];
		else return -1;
    }
};

struct memc_data{
	double ts;
	int row, col, core;
	int bank;
	double addr;
	bool approx;
	memc_data* next;

	inline void print(){
		printf("ts: %f row: %d col: %d bank: %d dest: %d addr: %f curnt_ts: %f\n",
				ts, row, col, bank, core, addr, (sc_time_stamp().to_double()/1000));
	}
};


// NoximPayload -- Payload definition
struct NoximPayload {
    sc_uint<32> data;	// Bus for the data to be exchanged
    int row, col, bank;

    inline bool operator ==(const NoximPayload & payload) const {
	return (payload.data == data);
}
    NoximPayload():data(0), row(0), col(0), bank(0){

    }
	NoximPayload(int data):data(data){
		row = 0; col = 0; bank = 0;
	};
};

// NoximPacket -- Packet definition
struct NoximPacket {
    int src_id;
    int dst_id;
    double timestamp;		// SC timestamp at packet generation
    int size;
    int flit_left;		// Number of remaining flits inside the packet
    bool use_low_voltage_path;
    double ack_msg;
    bool approximable;
    int apx_dst_id[DEFAULT_CHECK_DEPTH];
    int approx_len;
    NoximPayload payload;

    // Constructors
    NoximPacket(): src_id(-1), dst_id(-1), approximable(0),
    		timestamp(0), size(0), flit_left(0), use_low_voltage_path(0),
    		ack_msg(0),  approx_len(0){
    		payload.row = 0; payload.bank = 0; payload.col = 0;}

    NoximPacket(const int s, const int d, const double ts, const int sz, const int rp_sz) {
	make(s, d, ts, sz);
    }

    void make(const int s, const int d, const double ts, const int sz) {
	src_id = s;
	dst_id = d;
	timestamp = ts;
	size = sz;
	flit_left = sz;
	use_low_voltage_path = false;

	ack_msg =0;
    }
};

// NoximRouteData -- data required to perform routing
struct NoximRouteData {
    int current_id;
    int src_id;
    int dst_id;
    int dir_in;			// direction from which the packet comes from
};

struct NoximChannelStatus {
    int free_slots;		// occupied buffer slots
    bool available;		// 
    inline bool operator ==(const NoximChannelStatus & bs) const {
	return (free_slots == bs.free_slots && available == bs.available);
    };
};

// NoximNoP_data -- NoP Data definition
struct NoximNoP_data {
    int sender_id;
    NoximChannelStatus channel_status_neighbor[DIRECTIONS];

    inline bool operator ==(const NoximNoP_data & nop_data) const {
	return (sender_id == nop_data.sender_id &&
		nop_data.channel_status_neighbor[0] ==
		channel_status_neighbor[0]
		&& nop_data.channel_status_neighbor[1] ==
		channel_status_neighbor[1]
		&& nop_data.channel_status_neighbor[2] ==
		channel_status_neighbor[2]
		&& nop_data.channel_status_neighbor[3] ==
		channel_status_neighbor[3]);
    };
};

// NoximFlit -- Flit definition
struct NoximFlit {
    int src_id;
    int dst_id;
    NoximFlitType::value flit_type;	// The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
    int sequence_no;		// The sequence number of the flit inside the packet
    NoximPayload payload;	// Optional payload
    double timestamp;		// Unix timestamp at packet generation
    int hop_no;			// Current number of hops from source to destination
    bool use_low_voltage_path;
    long data_size;
    double ack_msg;
    bool flit_sent;
    bool approximable;
    uint tag;
    int apx_dst_id[DEFAULT_CHECK_DEPTH];
    int approx_len;


    NoximFlit() : payload(0)
    {
    	flit_sent = false; src_id =-1; dst_id =-1; flit_type = NoximFlitType::FLIT_TYPE_BODY;
    	sequence_no = -1; timestamp =0; hop_no = 0;
    	use_low_voltage_path = false; data_size =0; ack_msg =0; flit_sent = false; tag =0;
    	approx_len = 0; approximable = false;
    }

    inline bool operator ==(const NoximFlit & flit) const {
	return (flit.src_id == src_id && flit.dst_id == dst_id
		&& flit.flit_type == flit_type
		&& flit.sequence_no == sequence_no
		&& flit.payload == payload && flit.timestamp == timestamp
		&& flit.hop_no == hop_no
		&& flit.use_low_voltage_path == use_low_voltage_path);
}};

// Output overloading

inline ostream & operator <<(ostream & os, const NoximFlit & flit)
{

    if (NoximGlobalParams::verbose_mode == VERBOSE_HIGH) {

	os << "### FLIT ###" << endl;
	os << "Source Tile[" << flit.src_id << "]" << endl;
	os << "Destination Tile[" << flit.dst_id << "]" << endl;
	switch (flit.flit_type) {
	case NoximFlitType::FLIT_TYPE_HEAD:
	    os << "Flit Type is HEAD" << endl;
	    break;
	case NoximFlitType::FLIT_TYPE_BODY:
	    os << "Flit Type is BODY" << endl;
	    break;
	case NoximFlitType::FLIT_TYPE_TAIL:
	    os << "Flit Type is TAIL" << endl;
	    break;
	}
	os << "Sequence no. " << flit.sequence_no << endl;
	os << "Payload printing not implemented (yet)." << endl;
	os << "Unix timestamp at packet generation " << flit.
	    timestamp << endl;
	os << "Total number of hops from source to destination is " <<
	    flit.hop_no << endl;
    } else {
	os << "[type: ";
	switch (flit.flit_type) {
	case NoximFlitType::FLIT_TYPE_HEAD:
	    os << "H";
	    break;
	case NoximFlitType::FLIT_TYPE_BODY:
	    os << "B";
	    break;
	case NoximFlitType::FLIT_TYPE_TAIL:
	    os << "T";
	    break;
	}

	os << ", seq: " << flit.sequence_no << ", " << flit.src_id << "-->" << flit.dst_id << "]";
    }

    return os;
}

inline ostream & operator <<(ostream & os,
			     const NoximChannelStatus & status)
{
    char msg;
    if (status.available)
	msg = 'A';
    else
	msg = 'N';
    os << msg << "(" << status.free_slots << ")";
    return os;
}

inline ostream & operator <<(ostream & os, const NoximNoP_data & NoP_data)
{
    os << "      NoP data from [" << NoP_data.sender_id << "] [ ";

    for (int j = 0; j < DIRECTIONS; j++)
	os << NoP_data.channel_status_neighbor[j] << " ";

    cout << "]" << endl;
    return os;
}

inline ostream & operator <<(ostream & os, const NoximCoord & coord)
{
    os << "(" << coord.x << "," << coord.y << ")";

    return os;
}

// Trace overloading

inline void sc_trace(sc_trace_file * &tf, const NoximFlit & flit, string & name)
{
    sc_trace(tf, flit.src_id, name + ".src_id");
    sc_trace(tf, flit.dst_id, name + ".dst_id");
    sc_trace(tf, flit.sequence_no, name + ".sequence_no");
    sc_trace(tf, flit.timestamp, name + ".timestamp");
    sc_trace(tf, flit.hop_no, name + ".hop_no");
}

inline void sc_trace(sc_trace_file * &tf, const NoximNoP_data & NoP_data, string & name)
{
    sc_trace(tf, NoP_data.sender_id, name + ".sender_id");
}

inline void sc_trace(sc_trace_file * &tf, const NoximChannelStatus & bs, string & name)
{
    sc_trace(tf, bs.free_slots, name + ".free_slots");
    sc_trace(tf, bs.available, name + ".available");
}

// Misc common functions

inline NoximCoord id2Coord(int id)
{
    NoximCoord coord;

    coord.x = id % NoximGlobalParams::mesh_dim_x;
    coord.y = id / NoximGlobalParams::mesh_dim_x;

    if(coord.y >= NoximGlobalParams::mesh_dim_y)
    	coord.y = coord.y % NoximGlobalParams::mesh_dim_y;

    //cout<<" coord.y: "<<coord.y<<" Noximglobalparams:y "<<NoximGlobalParams::mesh_dim_y;
    assert(coord.x < NoximGlobalParams::mesh_dim_x);
    assert(coord.y < NoximGlobalParams::mesh_dim_y);

    return coord;
}

inline int coord2Id(const NoximCoord & coord)
{
    int id = (coord.y * NoximGlobalParams::mesh_dim_x) + coord.x;

    assert(id < NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);

    return id;
}

inline void console_out(char * log)
{
	if(NoximGlobalParams::show_log)
	{
		cout<<log<<endl;
	}
}

inline bool is_mc(NoximCoord src_coord)
{
	if( coord2Id(src_coord) == m1 || coord2Id(src_coord) == m2 || coord2Id(src_coord) == m3
			|| coord2Id(src_coord) == m4 || coord2Id(src_coord) == m5 || coord2Id(src_coord) == m6
			|| coord2Id(src_coord) == m7 || coord2Id(src_coord) == m8)
				return true;
	else
		return false;
}

inline bool is_mc(int local_id)
{
	if( local_id == m1 || local_id == m2 || local_id == m3
			|| local_id == m4 || local_id == m5 || local_id == m6
			|| local_id == m7 || local_id == m8)
				return true;
	else
		return false;
}

inline void g_approximate(deque <NoximPacket>& interface_buf){

	vector<int> rem_indx;
	double err_thresh = 0;
	int check_depth = NoximGlobalParams::check_depth;
	float approx_ratio = NoximGlobalParams::approx_rate;
	NoximPacket pkt = interface_buf.front();

	//pkt.approx_len = 0;
		if(pkt.approximable)
		{

			//cout<<"interface buff size "<< interface_buf.size()<<endl;

			for (int i = 0; i< interface_buf.size()-1; i++)
			{
				if (i > check_depth) break;  // only do it to the extent of check depth

				err_thresh = rand()%10;
				NoximPacket nxt_pkt = interface_buf.at(i+1);
				if(nxt_pkt.approximable && err_thresh < approx_ratio)
				{
					pkt.apx_dst_id[i] = nxt_pkt.dst_id;
					rem_indx.push_back(i+1);
					pkt.approx_len++;
					//cout<<"PACKET COALESCED count "<<pkt.approx_len<<endl;

				}
			}
			int indx_adjuster = 0;
			for (int i =0; i < rem_indx.size(); i++)
			{
				interface_buf.erase(interface_buf.begin()+(rem_indx[i] - indx_adjuster));
				indx_adjuster++;
			}
		}

}
#endif
