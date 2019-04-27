/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the router
 */

#include "fc_router.h"


void fc_router::rxProcess()
{
	 if (reset.read()) {
	// Clear outputs and indexes of receiving protocol
	reservation_table.clear();
	routed_flits = 0;
	local_drained = 0;
	in_flits = 0;
	prev_flit = 0;
	prev_flit_ax = 0;
    } else {
    	if(!is_mc(local_id)){
    		for(int i=0; i<2; i++){
    		if(enable_rx[i].read()){
    			NoximFlit flit = fast_flit_rx[i].read();
    			if(flit.dst_id == local_id &&
    					flit.hop_no > prev_flit){
    				incoming_buffer.Push(flit);
    				stats.receivedFlit(sc_time_stamp().to_double()/1000, flit);
    				stats.power.Buffering();
    				prev_flit = flit.hop_no;
    			}

    			// Stupid code for calculating the flit arrival rate
    			NoximCoord src_coord = id2Coord(flit.src_id);
    			NoximCoord dest_coord = id2Coord(flit.dst_id);
    			NoximCoord local_coord = id2Coord(local_id);
    			if(local_coord.y == src_coord.y){
    				if((local_coord.x < src_coord.x && local_coord.x >= dest_coord.x)
    						||(local_coord.x >= src_coord.x && local_coord.x <= dest_coord.x)){
    					in_flits++;
    					stats.power.Latch();
    					stats.power.Link(false);
    					stats.power.Crossbar();
    				}
    			}
    			else if(local_coord.x == dest_coord.x){
    				if((local_coord.y < src_coord.y && local_coord.y >= dest_coord.y)
    						||(local_coord.y > src_coord.y && local_coord.y <= dest_coord.y)){
    					in_flits++;
    					stats.power.Latch();
    					stats.power.Link(false);
    					stats.power.Crossbar();
    				}
    			}
    		}
    	}
    	}

    }
    stats.power.Leakage();
}

void fc_router::txProcess()
{
  if (reset.read())
    {
      // Clear outputs and indexes of transmitting protocol
	  for(int i=0; i<2; i++)
	  enable_tx[i].write(0);
      pipeline_latency = 0;
    }
  else
    {
	 // cout<<" fc_tx_process at node "<<local_id<<" check"<<endl;
	  if(is_mc(local_id) && send_enable){
		  bool en_flag = enable_rx[fast_line].read();
		  bool en_flag_2 = enable_rx[1 -fast_line].read();

		  if(!outgoing_buffer.IsEmpty()){
			  NoximFlit flit = outgoing_buffer.Front();
			  NoximCoord dst_coord = id2Coord(flit.dst_id);
			  if( !fc_buffer_full[dst_coord.x][dst_coord.y]){
				  bool ret = true;
				  for (int i = 0; i< flit.approx_len; i++)
				  {
					  NoximCoord dst_coord_i = id2Coord(flit.apx_dst_id[i]);
				  		if( !fc_buffer_full[dst_coord_i.x][dst_coord_i.y])
				  		{
				  			ret &= false;
				  		}
				  	}
				  if(ret){
						  enable_tx[fast_line].write(en_flag|true);
						  enable_tx[1-fast_line].write(en_flag_2|false);
						  flit.hop_no = sc_time_stamp().to_double()/1000;
						  fast_flit_tx[fast_line].write(flit);
						  outgoing_buffer.Pop();
						  stats.power.Buffer_read();
				  }
			  }
	  	  }
		  else{
			  enable_tx[fast_line].write(en_flag|false);
			  enable_tx[1-fast_line].write(en_flag_2|false);

		  }
	  }

    }				// else
  stats.power.Leakage();
}

//---------------------------------------------------------------------------
void fc_router::pe_rxprocess(){
	if(reset.read()){
		ack_rx.write(0);
		current_level_rx = 0;
	}
	else{

		if((req_rx.read() == 1 - current_level_rx) && !(outgoing_buffer.IsFull())){
			NoximFlit received_flit = flit_rx.read();
			//cout<<"The router at node "<<local_id<<" has received a flit from its PE"<<endl;
			received_flit.timestamp = sc_time_stamp().to_double()/1000;
			outgoing_buffer.Push(received_flit);
			in_flits++;
			current_level_rx = 1 - current_level_rx;
			ack_rx.write(current_level_rx);
		}
	}
}

void fc_router::pe_txprocess(){
	if(reset.read()){
		req_tx.write(0);
		current_level_tx = 0;
	}
	else{
		if(!(incoming_buffer.IsEmpty())){
			NoximFlit received_flit = incoming_buffer.Front();
			if(received_flit.dst_id == local_id){

				if (current_level_tx == ack_tx.read() ){

					current_level_tx = 1- current_level_tx;
					req_tx.write(current_level_tx);
					flit_tx.write(received_flit);

					//cout<<"The router at node "<<local_id<<" has matched its dest_id with the flit, so absorbing it"<<endl;

					  incoming_buffer.Pop();
				}
			}
			else{
				incoming_buffer.Pop();
			}
		}
	}

}

void fc_router::configure(const int _id,
			    const double _warm_up_time,
			    const unsigned int _max_buffer_size,
			    NoximGlobalRoutingTable & grt, int slice)
{
    local_id = _id;
    slice_id = slice;
    stats.configure(_id, _warm_up_time);

    if (grt.isValid())
	routing_table.configure(grt, _id);

	outgoing_buffer.SetMaxBufferSize(_max_buffer_size);
	incoming_buffer.SetMaxBufferSize(_max_buffer_size);


}

unsigned long fc_router::getRoutedFlits()
{
    return routed_flits;
}

unsigned int fc_router::getFlitsCount()
{
    unsigned count = 0;

    count += outgoing_buffer.Size();
	count += incoming_buffer.Size();

    return count;
}

double fc_router::getPower()
{
    return stats.power.getPower();
}


void fc_router::ShowBuffersStats(std::ostream & out)
{

    outgoing_buffer.ShowStats(out);
    incoming_buffer.ShowStats(out);
}



