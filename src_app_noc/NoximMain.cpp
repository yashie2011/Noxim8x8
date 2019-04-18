/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "NoximMain.h"
#include "NoximNoC.h"
#include "NoximGlobalStats.h"
#include "NoximCmdLineParser.h"
using namespace std;

// need to be globally visible to allow "-volume" simulation stop
unsigned int drained_volume;

ofstream slice_0_trace;
ofstream slice_1_trace;
ofstream slice_2_trace;
ofstream slice_3_trace;
ofstream slice_4_trace;
ofstream slice_5_trace;
ofstream slice_6_trace;
ofstream slice_7_trace;
ofstream slice_8_trace;
ofstream pre;
ofstream pre_reply;


// Initialize global configuration parameters (can be overridden with command-line arguments)
int NoximGlobalParams::verbose_mode = DEFAULT_VERBOSE_MODE;
int NoximGlobalParams::trace_mode = DEFAULT_TRACE_MODE;
char NoximGlobalParams::trace_filename[128] = DEFAULT_TRACE_FILENAME;
int NoximGlobalParams::mesh_dim_x = DEFAULT_MESH_DIM_X;
int NoximGlobalParams::mesh_dim_y = DEFAULT_MESH_DIM_Y;
int NoximGlobalParams::buffer_depth = DEFAULT_BUFFER_DEPTH;
int NoximGlobalParams::min_packet_size = DEFAULT_MIN_PACKET_SIZE;
int NoximGlobalParams::max_packet_size = DEFAULT_MAX_PACKET_SIZE;
int NoximGlobalParams::routing_algorithm = DEFAULT_ROUTING_ALGORITHM;
char NoximGlobalParams::routing_table_filename[128] = DEFAULT_ROUTING_TABLE_FILENAME;
int NoximGlobalParams::selection_strategy = DEFAULT_SELECTION_STRATEGY;
float NoximGlobalParams::packet_injection_rate = DEFAULT_PACKET_INJECTION_RATE;
float NoximGlobalParams::probability_of_retransmission = DEFAULT_PROBABILITY_OF_RETRANSMISSION;
int NoximGlobalParams::traffic_distribution = DEFAULT_TRAFFIC_DISTRIBUTION;
char NoximGlobalParams::traffic_table_filename[128] = DEFAULT_TRAFFIC_TABLE_FILENAME;
int NoximGlobalParams::simulation_time = DEFAULT_SIMULATION_TIME;
int NoximGlobalParams::stats_warm_up_time = DEFAULT_STATS_WARM_UP_TIME;
int NoximGlobalParams::rnd_generator_seed = time(NULL);
bool NoximGlobalParams::detailed = DEFAULT_DETAILED;
float NoximGlobalParams::dyad_threshold = DEFAULT_DYAD_THRESHOLD;
unsigned int NoximGlobalParams::max_volume_to_be_drained = DEFAULT_MAX_VOLUME_TO_BE_DRAINED;
vector <pair <int, double> > NoximGlobalParams::hotspots;
char NoximGlobalParams::router_power_filename[128] = DEFAULT_ROUTER_PWR_FILENAME;
bool NoximGlobalParams::low_power_link_strategy = DEFAULT_LOW_POWER_LINK_STRATEGY;
double NoximGlobalParams::qos = DEFAULT_QOS;
bool NoximGlobalParams::show_buffer_stats = DEFAULT_SHOW_BUFFER_STATS;
bool NoximGlobalParams::show_log = DEFAULT_LOGGING_FLAG;
double NoximGlobalParams::address_max = DEFAULT_ADDR_MAX;
double NoximGlobalParams::address_min = DEFAULT_ADDR_MIN;
char NoximGlobalParams::bench_name[128] = DEFAULT_BENCH_NAME;
int NoximGlobalParams::check_depth = DEFAULT_CHECK_DEPTH;
float NoximGlobalParams::approx_rate = DEFAULT_APPROX_RATE;
int NoximGlobalParams::bank_queues = DEFAULT_BANK_QUEUES;


//---------------------------------------------------------------------------

int sc_main(int arg_num, char *arg_vet[])
{
    // TEMP
    drained_volume = 0;

    // Handle command-line arguments
    cout << endl << "\t\tNoxim - the NoC Simulator" << endl;
    cout << "\t\t(C) University of Catania" << endl << endl;

    parseCmdLine(arg_num, arg_vet);

    // Signals
    sc_clock clock("clock", 1, SC_NS);
    sc_signal <bool> reset;

    // Benchmark Instance
     //benchmark* b_mark;
     //b_mark = new benchmark("benchmark");
     //b_mark->clock(clock);
     //b_mark->reset(reset);
     //cout<< "benchmark started"<<endl;
    // NoC instance
    NoximNoC *n = new NoximNoC("NoC");//, *b_mark);
    n->clock(clock);
    n->reset(reset);

    // Trace signals
    sc_trace_file *tf = NULL;
    if (NoximGlobalParams::trace_mode) {
	tf = sc_create_vcd_trace_file(NoximGlobalParams::trace_filename);
	sc_trace(tf, reset, "reset");
	sc_trace(tf, clock, "clock");

	for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
	    for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
		char label[30];

		sprintf(label, "req_to_east(%02d)(%02d)", i, j);
		sc_trace(tf, n->req_to_east[i][j], label);
		sprintf(label, "req_to_west(%02d)(%02d)", i, j);
		sc_trace(tf, n->req_to_west[i][j], label);
		sprintf(label, "req_to_south(%02d)(%02d)", i, j);
		sc_trace(tf, n->req_to_south[i][j], label);
		sprintf(label, "req_to_north(%02d)(%02d)", i, j);
		sc_trace(tf, n->req_to_north[i][j], label);

		sprintf(label, "ack_to_east(%02d)(%02d)", i, j);
		sc_trace(tf, n->ack_to_east[i][j], label);
		sprintf(label, "ack_to_west(%02d)(%02d)", i, j);
		sc_trace(tf, n->ack_to_west[i][j], label);
		sprintf(label, "ack_to_south(%02d)(%02d)", i, j);
		sc_trace(tf, n->ack_to_south[i][j], label);
		sprintf(label, "ack_to_north(%02d)(%02d)", i, j);
		sc_trace(tf, n->ack_to_north[i][j], label);
	    }
	}
    }
    // Reset the chip and run the simulation
    reset.write(1);
    cout << "Reset...";
    srand(NoximGlobalParams::rnd_generator_seed);	// time(NULL));
    sc_start(DEFAULT_RESET_TIME, SC_NS);
    reset.write(0);
    cout << " done! Now running for " << NoximGlobalParams::
	simulation_time << " cycles..." << endl;
    slice_0_trace.open("slice_0.log", ios::out | ios::app);
    slice_1_trace.open("slice_1.log", ios::out | ios::app);
    slice_2_trace.open("slice_2.log", ios::out | ios::app);
    slice_3_trace.open("slice_3.log", ios::out | ios::app);
    slice_4_trace.open("slice_4.log", ios::out | ios::app);
    slice_5_trace.open("slice_5.log", ios::out | ios::app);
    slice_6_trace.open("slice_6.log", ios::out | ios::app);
    slice_7_trace.open("slice_7.log", ios::out | ios::app);
    slice_8_trace.open("slice_8.log", ios::out | ios::app);
    pre.open("pre.log", ios::out | ios::app);
    pre_reply.open("pre_reply.log", ios::out | ios::app);


    sc_start(NoximGlobalParams::simulation_time, SC_NS);
    //sc_start();
    if(not sc_end_of_simulation_invoked()) {
        cout << "Simulation stopped without explicit sc_stop()" << endl;
        sc_stop();
      }
    // Close the simulation
    if (NoximGlobalParams::trace_mode)
	sc_close_vcd_trace_file(tf);
    cout << "Noxim simulation completed." << endl;
    cout << " ( " << sc_time_stamp().to_double() /
	1000 << " cycles executed)" << endl;


    // Show statistics
    NoximGlobalStats gs(n);
    gs.showStats(std::cout, NoximGlobalParams::detailed);

     slice_0_trace.close();
     slice_1_trace.close();
     slice_2_trace.close();
     slice_3_trace.close();
     slice_4_trace.close();
     slice_5_trace.close();
     slice_6_trace.close();
     slice_7_trace.close();
     slice_8_trace.close();

     pre.close();
        pre_reply.close();
    if ((NoximGlobalParams::max_volume_to_be_drained > 0) &&
	(sc_time_stamp().to_double() / 1000 >=
	 NoximGlobalParams::simulation_time)) {
	cout <<
	    "\nWARNING! the number of flits specified with -volume option"
	    << endl;
	cout << "has not been reached. ( " << drained_volume <<
	    " instead of " << NoximGlobalParams::
	    max_volume_to_be_drained << " )" << endl;
	cout <<
	    "You might want to try an higher value of simulation cycles" <<
	    endl;
	cout << "using -sim option." << endl;
#ifdef TESTING
	cout << "\n Sum of local drained flits: " << gs.
	    drained_total << endl;
	cout << "\n Effective drained volume: " << drained_volume;
#endif
    }

    return 0;
}
