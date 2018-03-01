#ifndef __GATEWAYINTERFACE_H__
#define __GATEWAYINTERFACE_H__

//---------------------------------------------------------------------------

#define GI_BUFFER_DEPTH 2
#define GLOBAL_TX_INDEX 1
#define LOCAL_TX_INDEX  0
#define RESET_REQ_QUEUE 200
#define ONE_CYCLE 0.5

#include <systemc.h>
#include <vector>
#include <algorithm>
#include "buffer.h"
#include "status.h"
//#include "reservationtable.h"
#include "GlobalTypeDefs.h"
extern unsigned int lambda[];

SC_MODULE(TGatewayInterface)
{
	int 			local_xid; 			// X-Coordinates of current location
	int 			local_yid;			// Y-Cordinates of current location
	int             local_id;            // Unique ID
  // I/O Ports

  sc_in_clk          clock;      // The input clock for the router
  sc_in<bool>        reset;               // The reset signal for the router

  /** Ports for the bottom electrical layer - START **/
  sc_in<TFlit>       flit_rx;
  sc_in<bool>          req_rx;       
  sc_out<bool>         ack_rx; 
  
  sc_out<TFlit>      flit_tx;
  sc_out<bool>       req_tx; 
  sc_in<bool>        ack_tx; 
  
  sc_out<int>        free_slots;
  sc_in<int>         free_slots_neighbor;
  
 
  /** Ports for the bottom electrical layer - END **/
  
  /** Ports for optical communication  - START **/
  
  sc_out<TFlit>                  mwmr_data_tx[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  sc_in<TFlit>                   mwmr_data_rx;
  //sc_vector<sc_in<TFlit> >        swmr_data_rx;
    /** Ports for optical communication  - END **/
  
  /** Ports for communication with Arbiter  - START **/
  
  sc_out<TFlit>		     req_tx_arbiter_flit;   // req for arbiter 
  sc_out<bool>           req_tx_arbiter_hf;  //req for arbiter to receive head flit
  sc_in<bool>            ack_tx_arbiter_hf;  // ack of arbiter for receipt of head flit      
  
  sc_in<bool>            req_rx_arbiter_ch;//ack_tx_arbiter;    // ack from arbiter to send data
  sc_in<int>             mwmr_channel_no; // MWMR channel number from arbiter to send data
  sc_out<bool>            ack_rx_arbiter_ch;  // ack of arbiter for receipt of channel number
    
  sc_out<bool>			 req_txc_arbiter;
  sc_out<bool>			 ack_txc_arbiter;    // ack to arbiter after sending data
  sc_in<bool>			 ack_txc_gwi;    // ack from arbiter after receving data
  
  bool                   arbiter_current_level_rx_ch;    // Arbiter Current level for Alternating Bit Protocol (ABP)
  bool                   arbiter_current_level_tx_req;    // Arbiter Current level for Alternating Bit Protocol (ABP) for request
  /** Ports for communication with Arbiter  - END **/
  
  /** Ports for optical communication  - END **/
  
  /** Ports for communication with Arbiter  - START **/
  
  sc_out<TFlit>		     req_tx_arbiter;   // req for arbiter 
  sc_in<bool>            ack_tx_arbiter;    // ack from arbiter to send data
  sc_out<bool>			 ack_txc_arbiter;    // ack to arbiter after sending data
  bool                   arbiter_current_level_tx;    // Arbiter Current level for Alternating Bit Protocol (ABP)
  /** Ports for communication with Arbiter  - END **/
    
  TBuffer            buffer_local;         // Buffer for input channel from the electrical side
  bool               current_level_rx;    // Current level for Alternating Bit Protocol (ABP)
  bool               current_level_tx;    // Current level for Alternating Bit Protocol (ABP)
  
  TStats             stats;                           // Statistics
  
  int                start_from_port;                 // Port from which to start the reservation cycle
  unsigned long      routed_flits;

  // Threads and Functions
  void               rxProcess();           // The receiving process
  void               txProcess();           // The Tx Process
  void               mwsr_tx_data();         // Send the data to crossbar
  void				 arbiter_req();          // request arbiter for cross bar channel 
  void               bufferMonitor();
   unsigned long     getRoutedFlits(); // Returns the number of routed flits 
  // Constructor

  SC_CTOR(TGatewayInterface):
 // ack_tx_arbiter(sc_gen_unique_name("arbiter_in"))
  ack_txc_arbiter(sc_gen_unique_name("arbiter_out"))
  {
	  
	  
	  SC_METHOD(rxProcess);
      sensitive << reset;
      sensitive << clock.pos();

      SC_METHOD(txProcess);
      sensitive << reset;
      sensitive << clock.pos();
      
      SC_METHOD(mwsr_tx_data);
      sensitive << reset;
      sensitive << clock.pos();
      
      SC_METHOD(arbiter_req);
      sensitive << reset;
      sensitive << clock.pos();
      
      SC_METHOD(bufferMonitor);
	  sensitive << reset;
      sensitive << clock.pos();
    	    	            
  }

  private:
 
  
  int                           nearest_gateway_id ; // nearest_gateway_id
  std::map<int, int>            gateway_map;
  std::map<int, int>::iterator  pri_gateway_it;  
   
  // sends the reservation request if the 
  //local buffer is found to have at least a flit
  sc_event_queue      reservation_req;     
  
  // sends reservation ack event to notify the 
  //sender if local buffer has space to be allotted
  sc_event           reservation_ack;    
  
  sc_event           data_ack;
  sc_event           photonic_to_electrical_tx;
  
  vector<int>        reserve_req_queue;
  vector<int>        data_ack_queue;
  sc_uint<2>         req_source_id;
  int                prev_winner;
  bool               safe_to_reserve; // safe to send reservation to arbiter
  bool               safe_to_send;    // safe to send data to crossbar
  double 			 previous_transfer;
  public:
  unsigned int local_drained;
};  

#endif

