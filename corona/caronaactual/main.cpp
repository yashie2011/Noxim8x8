#include <systemc.h>
#include <cassert>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "buffer.h"
#include "status.h"
#include "power.h"
#include "processingelement.h"
#include "gatewayinterface.h"
#include "gitile.h"
#include "arbiter.h"
#include "noc.h"
#include "globalstatus.h"


unsigned int drained_volume;
unsigned int lambda[32] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                              16, 17, 18, 19, 20 , 21, 22, 23,24, 25, 26 , 27, 28, 29, 30, 31};

// Main program
int sc_main(int arg_num, char* arg_vet[]) {

    sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
    // TEMP
    drained_volume = 0;
    bool specifiedPir = false;
    bool specifiedPor = false;
    // Signals
    sc_clock clock("clock", 1.6, SC_NS);
    //sc_clock photonic_clock("photonic_clock", 0.4, SC_NS);

    sc_signal<bool> reset;

    // NoC instance
    TNoC* n = new TNoC("NoC");

    n->clock(clock);
    //n->photonic_clock(photonic_clock);
    n->reset(reset);
    
    // Reset the chip and run the simulation
    reset.write(1);
    cout << "Reset...";
    srand(TGlobalParams::rnd_generator_seed); // time(NULL));
    sc_start(DEFAULT_RESET_TIME, SC_NS);
    reset.write(0);
    cout << " done! Now running for " << TGlobalParams::simulation_time << " cycles..." << endl;
    sc_start(TGlobalParams::simulation_time, SC_NS);
	//
	// System C trace
	//
	sc_trace_file *pfScTrace = sc_create_vcd_trace_file("waveform");
  pfScTrace->set_time_unit(100, SC_PS);
  
  //sc_trace(pfScTrace, sysClk, sc_gen_unique_name("sysClk")); //system_clock
  //sc_trace(pfScTrace, clock, sc_gen_unique_name("system_clock"));
   
  //for(int traceId = 0 ; traceId < 4 ; traceId ++)
  //{
    //sc_trace(pfScTrace, sgnDestAddr[traceId], sc_gen_unique_name("DA"));
    //sc_trace(pfScTrace, sgnBuffAddr[traceId], sc_gen_unique_name("buffAddr"));
    //sc_trace(pfScTrace, sgnWriteEnable[traceId], sc_gen_unique_name("WE"));

    //sc_trace(pfScTrace, sgnDimmEnable[traceId], sc_gen_unique_name("DE"));
    //sc_trace(pfScTrace, sgnBiuBusy[traceId], sc_gen_unique_name("RB"));
    //sc_trace(pfScTrace, sgnAddrOut[traceId], sc_gen_unique_name("AO"));
    //sc_trace(pfScTrace, sgnWriteOut[traceId], sc_gen_unique_name("WO"));
    //sc_trace(pfScTrace, sgnCompAck[traceId], sc_gen_unique_name("CA"));
  //}
    // Show statistics
    TGlobalStats gs(n);
    gs.showStats(std::cout, TGlobalParams::detailed);

    return 0;

}

