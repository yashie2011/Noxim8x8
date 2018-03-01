#ifndef __HUB_H__
#define __HUB_H__

//---------------------------------------------------------------------------

#include <cassert>
#include <queue>
#include "GlobalTypeDefs.h"

using namespace std;

//---------------------------------------------------------------------------

SC_MODULE(THub)
{

  // I/O Ports

  sc_in_clk            clock;        // The input clock for the HUB
  sc_in<bool>          reset;        // The reset signal for the HUB

  sc_in<TFlit>         flit_rx;      // The input channel from PE
  sc_in<bool>          req_rx;       // The request associated with the input channel to PE
  sc_out<bool>         ack_rx;       // The outgoing ack signal associated with the input channel to PE

  sc_out<TFlit>        flit_tx;      // The output channel to PE
  sc_out<bool>         req_tx;       // The request associated with the output channel to PE
  sc_in<bool>          ack_tx;       // The outgoing ack signal associated with the output channel to PE
  
  sc_in<TFlit>         gflit_rx;      // The input channel from GWI
  sc_in<bool>          greq_rx;       // The request associated with the input channel to GWI
  sc_out<bool>         gack_rx;       // The outgoing ack signal associated with the input channel to GWI

  sc_out<TFlit>        gflit_tx;      // The output channel to GWI
  sc_out<bool>         greq_tx;       // The request associated with the output channel to GWI
  sc_in<bool>          gack_tx;       // The outgoing ack signal associated with the output channel to GWI

  sc_out<int>         free_slots;
  sc_in<int>          free_slots_neighbor;
  
  sc_out<int>         gfree_slots;
  sc_in<int>          gfree_slots_neighbor;
  
  void proc_hub();
   SC_CTOR(THub)
  {
    SC_METHOD(proc_hub);
    sensitive << reset;
    sensitive << clock.pos();
   }

 };

#endif
