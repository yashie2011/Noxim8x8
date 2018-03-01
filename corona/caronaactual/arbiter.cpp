#include "GlobalTypeDefs.h"
#include "processingelement.h"
#include "arbiter.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector

//---------------------------------------------------------------------------
//sc_in<TFlit>      req_tx_arbiter[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
  //sc_out<bool>      ack_tx_arbiter[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y]; // ack from arbiter to send data
  //sc_in<bool>     	ack_txc_arbiter[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y];
void TGlobalarbiter::proc_arbiter_req_rx()
{
	 if(reset.read())
  {
	  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
		  {
			  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
			  {
				  gwi_current_level_rx_req[i][j] = 0;
				  ack_tx_arbiter_hf[i][j].write(0);
			  }
		  }
  }
  
  else
  {
	  if(!reset.read()) 
	  {
		  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
		  {
			  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
			  {
				  if( req_tx_arbiter_hf[i][j].read()==1-gwi_current_level_rx_req[i][j]) 
				  {
				  
				  TFlit received_flit = req_tx_arbiter[i][j].read(); // Receving Head Flit from a tile 
				    if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				    {
						cout<< sc_time_stamp().to_double()/1000  << "Source_Id --> " <<  received_flit.src_id <<" Head_Destination_Id --> " <<  received_flit.dst_id << endl;
						}
						
				  if(received_flit.flit_type == FLIT_TYPE_HEAD)
				 {
				  TCoord dst, src;
				  dst.x = id2Coord(received_flit.dst_id).x;
				  dst.y = id2Coord(received_flit.dst_id).y;
				  src.x = id2Coord(received_flit.src_id).x;
				  src.y = id2Coord(received_flit.src_id).y;
				  int newid = coord2Id(src);
				  	  if(TGlobalParams::verbose_mode > VERBOSE_OFF)
				    {
						cout<< sc_time_stamp().to_double()/1000  << "Source_new_Id --> " <<  newid <<" Head_Destination_Id --> " <<  received_flit.dst_id << endl;
						}
				 
					  if(std::find(reserve_req_queue[dst.x][dst.y].begin(), reserve_req_queue[dst.x][dst.y].end(), newid) == reserve_req_queue[dst.x][dst.y].end())
					  {
						  temp_reserve_req_queue[dst.x][dst.y].push_back(newid);
						 
						  if(TGlobalParams::verbose_mode > VERBOSE_OFF)
					  {
						  cout << sc_time_stamp().to_double()/1000 << "queue for X.cord" << dst.x << "Y.cord" << dst.y << "queue value"<<temp_reserve_req_queue[dst.x][dst.y].size() << endl;
						  }
						  
					  }
				  }
					  gwi_current_level_rx_req[i][j] = 1-gwi_current_level_rx_req[i][j]; 
						}
						ack_tx_arbiter_hf[i][j].write(gwi_current_level_rx_req[i][j]);
				  
			  }
		  }
		  
		  	  
	  }
  }	
}
void TGlobalarbiter::proc_arbiter_maint_queue()
{
	 if(reset.read())
  {
	   for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
	  {
		  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
		  {
			  reserve_req_queue[i][j].clear();
		  }
	  }
  }
  
  else
  {
	  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
		  {
			  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
			  {
				   std::sort(temp_reserve_req_queue[i][j].begin(), temp_reserve_req_queue[i][j].end());
				  reserve_req_queue[i][j].insert(reserve_req_queue[i][j].end(), temp_reserve_req_queue[i][j].begin(), temp_reserve_req_queue[i][j].end());
				  temp_reserve_req_queue[i][j].clear();
				  if(TGlobalParams::verbose_mode > VERBOSE_OFF)
					  {
						  cout << sc_time_stamp().to_double()/1000 << "queue for X.cord" << i << "Y.cord" << j << "queue value"<<reserve_req_queue[i][j].size() << endl;
						  }
			  }
		  }
	  }
  }
void TGlobalarbiter::proc_arbiter_ack_tx()
{
	 if(reset.read())
  {
	   for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
	  {
		  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
		  {
			  ack_tx_arbiter[i][j].write(0);
			  //arbiter_current_level_tx =0;
		  }
	  }
  }
  
  else
  {
	  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
		  {
			  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
			  {
				  if (!reserve_req_queue[i][j].empty())
				  {
					  int ack_src_newid = reserve_req_queue[i][j].front();
				  TCoord ack_src;                         // cordinates of the source to be acknowledged
				  ack_src.x = id2Coord(ack_src_newid).x;
				  ack_src.y = id2Coord(ack_src_newid).y;
				  if (ack_txc_arbiter[ack_src.x][ack_src.y] != 0)
				  {
					  ack_tx_arbiter[ack_src.x][ack_src.y].write(1);
				  }
				  reserve_req_queue[i][j].erase (reserve_req_queue[i][j].begin()+0);
			  }
			  }
		  }
	  }
  }
				  
				  
