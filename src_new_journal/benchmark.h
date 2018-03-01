#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>        // std::abs
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <systemc.h>

#include "constants.h"
#include "NoximMain.h"

using namespace std;
typedef struct communication{
    		int dest_id;
    		int data_size;
    		bool status;
    		int comp_time;
    		int warp_id;

    	communication(){dest_id = 0; data_size = 0; status = false; comp_time = 0; warp_id = 0;};
    	}comm;


SC_MODULE(benchmark)
{
	sc_in_clk            clock;        // The input clock for the bm
    sc_in<bool>          reset;        // The reset signal for the bm


    double trace_len;

    inline  double get_trace_len(){return trace_len;};

	void packet_injection_nodeindex();
	bool packet_injection_response_req(int nodeid_request, std::vector<comm> &ret_arr);
	comm packet_injection_response_rep(int nodeid_request);
	void clear_vector();


  SC_HAS_PROCESS(benchmark);
  benchmark(sc_module_name name, char* filename, int node_id)
  {
	  nodeid = node_id;
		trace_index = 0;
		//cout<<"nodeid is: "<<nodeid<<" filename is "<< filename<<endl;
		string benchmark_type = filename; // "TracefileP2.txt";
		myfile.open(benchmark_type.c_str());
		if (myfile.is_open()) 
		{
        while (getline(myfile,line)) 
        {
			v_req.push_back(line);
		}
		}
		myfile.close();
		//cout << "Number of values read from " << benchmark_type <<" file are " << v_req.size() << endl;
		//cout << "First Line " << v_req[0] << endl;

		trace_len = v_req.size();
    SC_METHOD(packet_injection_nodeindex);
    sensitive << reset;
    sensitive << clock.pos();


  }

private:

	int trace_index;
	double current_cycle;
	string benchmark_type;
	ifstream myfile;
	string line;
	vector<string> v_req;
	int nodeid;
	vector<comm> comm_req;

};

#endif

	
