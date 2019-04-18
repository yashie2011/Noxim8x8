/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global statistics
 */

#ifndef __NOXIMGLOBALSTATS_H__
#define __NOXIMGLOBALSTATS_H__

#include <iostream>
#include <vector>
#include <iomanip>
#include "NoximNoC.h"
#include "NoximTile.h"
using namespace std;

extern ofstream slice_0_trace;
extern ofstream slice_1_trace;
extern ofstream slice_2_trace;
extern ofstream slice_3_trace;
extern ofstream slice_4_trace;

class NoximGlobalStats {

  public:

    NoximGlobalStats(const NoximNoC * _noc);

    // Returns the aggragated average delay (cycles)
    double getAverageDelay(int slice);

    // Returns the aggragated average delay (cycles) for communication src_id->dst_id
    double getAverageDelay(const int src_id, const int dst_id);

    // Returns the max delay
    double getMaxDelay();

    // Returns the max delay (cycles) experimented by destination
    // node_id. Returns -1 if node_id is not destination of any
    // communication
    double getMaxDelay(const int node_id);

    // Returns the max delay (cycles) for communication src_id->dst_id
    double getMaxDelay(const int src_id, const int dst_id);

    // Returns tha matrix of max delay for any node of the network
     vector < vector < double > > getMaxDelayMtx();

    // Returns the aggragated average throughput (flits/cycles)
    double getAverageThroughput();

    // Returns the aggragated average throughput (flits/cycles) for
    // communication src_id->dst_id
    double getAverageThroughput(const int src_id, const int dst_id);

    // Returns the total number of received packets
    unsigned int getReceivedPackets();

    // Returns the total number of received flits
    unsigned int getReceivedFlits();

    // Returns the maximum value of the accepted traffic
    double getThroughput();

    // Returns the number of routed flits for each router
     vector < vector < unsigned long > > getRoutedFlitsMtx();

    // Returns the total power
    double getPower();


    // get the max buffer size occupied at mcs
    int get_max_buffer_size();

    // Shows global statistics
    void showStats(std::ostream & out = std::cout, bool detailed = false);

    void showBufferStats(std::ostream & out);


    int getReceivedflits_slice(int k);
    int gettransmittedflits_slice(int k);

#ifdef TESTING
    unsigned int drained_total;
#endif

  private:
    const NoximNoC *noc;
};

#endif
