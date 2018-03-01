#include "gatewayinterface.h"

/** This thread transfers flits to the lower electrical layer.
 *  Since the photonic communication comes to an end here , the 
 *  inter pri bit is reset here before sending to electrical router 
**/

void TGatewayInterface::txProcess() // Clock Edge
{
	if (reset.read())
    {
      req_tx.write(0);
	  
	}
  else
    {
		  TFlit received_flit_temp = mwmr_data_rx.read();
		  if (received_flit_temp.timestamp !=0)
		  stats.receivedFlit(sc_time_stamp().to_double()/1000, received_flit_temp);
		  flit_tx.write(mwmr_data_rx.read());		// passing the flit from cross bar to electircal domain
  }


}

/** This thread recieves  flits from the lower electrical layer and stores it 
 * in the local buffer. Once a flit is pushed in the buffer ,process triggers
 * the photonic transfer process by notifying the reservation process 
**/
void TGatewayInterface::rxProcess() // sensitive -- photonic_to_electrical_tx
{
  if(reset.read())
    { // Clear outputs and indexes of receiving protocol
      ack_rx.write(0);
	  current_level_rx = 0;
	  routed_flits = 0;
      local_drained = 0;
    }
  else
     {

	  if ( (req_rx.read()==1-current_level_rx) && !buffer_local.IsFull() )
	    {
	      TFlit received_flit = flit_rx.read();

	      if(TGlobalParams::verbose_mode > VERBOSE_OFF)
		{
		  cout << sc_time_stamp().to_double()/1000 << ": GWI["<< local_id <<"], Input, Received flit: " << received_flit << endl;
		}

	      // Store the incoming flit in the circular buffer
	      buffer_local.Push(received_flit);            

	      // Negate the old value for Alternating Bit Protocol (ABP)
	      current_level_rx = 1-current_level_rx;
	    }
	  ack_rx.write(current_level_rx);
	
    }

    
}
/**--------------------------------------------------------------------------
 * This module writes on crossbar in MWSR fashion and trainsmits data to its destination 
 * based on the decision taken by arbiter 
 * 
---------------------------------------------------------------------------*/
void TGatewayInterface::mwsr_tx_data() // sensitive - Clock
{
  if(reset.read())
  {
	  ack_txc_arbiter.write(1);
	  arbiter_current_level_tx =0;
  }
  
  else if(!reset.read()  && !buffer_local.IsEmpty() )//&& ack_tx_arbiter.read() == 1- arbiter_current_level_tx
  {
       TFlit flit = buffer_local.Front();
       
       // computing the destination id of transmitting flit
       TCoord dst;
	   dst.x = id2Coord(flit.dst_id).x;
	   dst.y = id2Coord(flit.dst_id).y;
	   mwmr_data_tx[dst.x][dst.y].write(flit);
	   if(TGlobalParams::verbose_mode > VERBOSE_OFF)
	   {
         cout<< sc_time_stamp().to_double()/1000  << "Source_Id --> " <<  flit.src_id <<" Destination_Id --> " <<  flit.dst_id << endl; 	   
	   }
	   if(flit.flit_type == FLIT_TYPE_TAIL) 
	   { 
		   arbiter_current_level_tx = 1- arbiter_current_level_tx;
		   if(TGlobalParams::verbose_mode > VERBOSE_OFF)
	   {
         cout<< sc_time_stamp().to_double()/1000  << "Source_Id --> " <<  flit.src_id <<" Destination_Id --> " <<  flit.dst_id << endl; 	   
	   }
		   safe_to_reserve = 1;
		   safe_to_send    = 0;	  
	   }
	   buffer_local.Pop();
	   
	   //routed_flits++;
	   ack_txc_arbiter.write(arbiter_current_level_tx);
	   	  
  }	
}

/**--------------------------------------------------------------------------
 * This module requests arbiter to grant access to arbiter by sending its head flit
 * 
---------------------------------------------------------------------------*/

//This module generates request to arbiter about arrival of flit
  
void TGatewayInterface::arbiter_req() // sensitive - Clock
{
  //if(reset.read())
  //{
	  
  //}
  
  //else if(!reset.read() && ack_tx_arbiter.read() != 1 && !buffer_local.IsEmpty() )
  //{
       //TFlit flit = buffer_local.Front();
       //// sending head flit to arbiter 
	   //if(flit.flit_type == FLIT_TYPE_HEAD) 
	   //{ 
		  //req_tx_arbiter.write(flit);
		   //if(TGlobalParams::verbose_mode > VERBOSE_OFF)
	   //{
         //cout<< sc_time_stamp().to_double()/1000  << "Source_Id --> " <<  flit.src_id <<" Head_Destination_Id --> " <<  flit.dst_id << endl; 	   
	   //}
		   
	   //}
	   	  
  //}
  if(reset.read())
  {
	  req_tx_arbiter_hf.write(0);
	  arbiter_current_level_tx_req =0;
	  previous_transfer = -10;
	  
  }
  else
  {
  if(!reset.read() && ack_tx_arbiter_hf.read() == arbiter_current_level_tx_req && !buffer_local.IsEmpty())
  {
	  if(previous_transfer != buffer_local.Front().timestamp)
	  {
       TFlit flit = buffer_local.Front();
       // sending head flit to arbiter 
		  req_tx_arbiter_flit.write(flit);
		  previous_transfer = flit.timestamp;
		   if(TGlobalParams::verbose_mode > VERBOSE_OFF)
	   {
         cout<< sc_time_stamp().to_double()/1000  << "Previous transfer:" << previous_transfer << "From Request Source_Id --> " <<  flit.src_id <<" Head_Destination_Id --> " <<  flit.dst_id <<" ab value " << arbiter_current_level_tx_req << endl; 	   
	   }
	   
	   arbiter_current_level_tx_req = 1-arbiter_current_level_tx_req;
	   req_tx_arbiter_hf.write(arbiter_current_level_tx_req);
   } 
		   
	   
	   	  
  }
}	
}

void TGatewayInterface::bufferMonitor()
{
  if (reset.read())
  {
    free_slots.write(buffer_local.GetMaxBufferSize());
  }
  else
  {
     // update current input buffers level to Hub
    free_slots.write(buffer_local.getCurrentFreeSlots());

    }
  }

//---------------------------------------------------------------------------
unsigned long TGatewayInterface::getRoutedFlits()
{ 
  return routed_flits; 
}
