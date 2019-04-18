/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the Network-on-Chip
 */

#include "NoximNoC.h"

void NoximNoC::buildMesh()
{
    // Check for routing table availability
    if (NoximGlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
	assert(grtable.load(NoximGlobalParams::routing_table_filename));

    // Check for traffic table availability
    if (NoximGlobalParams::traffic_distribution == TRAFFIC_TABLE_BASED)
	assert(gttable.load(NoximGlobalParams::traffic_table_filename));

    // create the Trace DB handler
    SQLiteDB* trace_db_p = new SQLiteDB();
    //Connect TO database
    if(!trace_db_p->OpenConnection("trace_dct.db", "/home/yaswanth/NVIDIA_GPU_Computing_SDK/C/bin/linux/release/dct_logs/"))
    {
    	cout<<"\nConnecting To DB Failed :"<<trace_db_p->GetLastError().c_str();
    	exit(0);
    }
    // Create the mesh as a matrix of tiles

    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
	for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
		// Creating a benchmark module for each tile

	    int id = (j* NoximGlobalParams::mesh_dim_x)+ i;
	    // Adapting to the ID numbers of the gpgpusim    -- needs change when expanding
		if(id >= m1 && id < m2 )
			id--;
		if(id >= m2 && id < m3 )
			id = id-2;
		if(id >= m3 && id < m4)
			id = id-3;
		if(id >= m4 && id < m5)
			id = id-4;
		if(id >= m5 && id < m6)
			id = id-5;
		if(id >= m6 && id < m7)
			id = id-6;
		if(id >= m7 && id < m8)
			id = id-7;
		if(id >= m8)
			id = id-8;

	    // Create the single Tile with a proper name
	    char tile_name[20];
	    sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	    t[i][j] = new NoximTile(tile_name, trace_db_p);



	    // Tell to the PE its coordinates
	    t[i][j]->pe->local_id = j * NoximGlobalParams::mesh_dim_x + i;
	    t[i][j]->pe->traffic_table = &gttable;	// Needed to choose destination
	    t[i][j]->pe->never_transmit = (gttable.occurrencesAsSource(t[i][j]->pe->local_id) == 0);

	    // Map clock and reset
	    t[i][j]->clock(clock);
	    t[i][j]->reset(reset);
	    // Looping over the slices
	        for (int k = 0; k< SLICES; k++) {
        // Tell to the router its coordinates
	    t[i][j]->r[k]->configure(j * NoximGlobalParams::mesh_dim_x + i,
	       NoximGlobalParams::stats_warm_up_time,
	       NoximGlobalParams::buffer_depth,
	       grtable, k);
	    // Map Rx signals
	    t[i][j]->req_rx[k][DIRECTION_NORTH] (req_to_south[k][i][j]);
	    t[i][j]->flit_rx[k][DIRECTION_NORTH] (flit_to_south[k][i][j]);
	    t[i][j]->ack_rx[k][DIRECTION_NORTH] (ack_to_north[k][i][j]);

	    t[i][j]->req_rx[k][DIRECTION_EAST] (req_to_west[k][i + 1][j]);
	    t[i][j]->flit_rx[k][DIRECTION_EAST] (flit_to_west[k][i + 1][j]);
	    t[i][j]->ack_rx[k][DIRECTION_EAST] (ack_to_east[k][i + 1][j]);

	    t[i][j]->req_rx[k][DIRECTION_SOUTH] (req_to_north[k][i][j + 1]);
	    t[i][j]->flit_rx[k][DIRECTION_SOUTH] (flit_to_north[k][i][j + 1]);
	    t[i][j]->ack_rx[k][DIRECTION_SOUTH] (ack_to_south[k][i][j + 1]);

	    t[i][j]->req_rx[k][DIRECTION_WEST] (req_to_east[k][i][j]);
	    t[i][j]->flit_rx[k][DIRECTION_WEST] (flit_to_east[k][i][j]);
	    t[i][j]->ack_rx[k][DIRECTION_WEST] (ack_to_west[k][i][j]);

	    // Map Tx signals
	    t[i][j]->req_tx[k][DIRECTION_NORTH] (req_to_north[k][i][j]);
	    t[i][j]->flit_tx[k][DIRECTION_NORTH] (flit_to_north[k][i][j]);
	    t[i][j]->ack_tx[k][DIRECTION_NORTH] (ack_to_south[k][i][j]);

	    t[i][j]->req_tx[k][DIRECTION_EAST] (req_to_east[k][i + 1][j]);
	    t[i][j]->flit_tx[k][DIRECTION_EAST] (flit_to_east[k][i + 1][j]);
	    t[i][j]->ack_tx[k][DIRECTION_EAST] (ack_to_west[k][i + 1][j]);

	    t[i][j]->req_tx[k][DIRECTION_SOUTH] (req_to_south[k][i][j + 1]);
	    t[i][j]->flit_tx[k][DIRECTION_SOUTH] (flit_to_south[k][i][j + 1]);
	    t[i][j]->ack_tx[k][DIRECTION_SOUTH] (ack_to_north[k][i][j + 1]);

	    t[i][j]->req_tx[k][DIRECTION_WEST] (req_to_west[k][i][j]);
	    t[i][j]->flit_tx[k][DIRECTION_WEST] (flit_to_west[k][i][j]);
	    t[i][j]->ack_tx[k][DIRECTION_WEST] (ack_to_east[k][i][j]);
	        } // Looping over the slices

	    // Map buffer level signals (analogy with req_tx/rx port mapping)
	    /*t[i][j]->free_slots[DIRECTION_NORTH] (free_slots_to_north[i][j]);
	    t[i][j]->free_slots[DIRECTION_EAST] (free_slots_to_east[i + 1][j]);
	    t[i][j]->free_slots[DIRECTION_SOUTH] (free_slots_to_south[i][j + 1]);
	    t[i][j]->free_slots[DIRECTION_WEST] (free_slots_to_west[i][j]);

	    t[i][j]->free_slots_neighbor[DIRECTION_NORTH] (free_slots_to_south[i][j]);
	    t[i][j]->free_slots_neighbor[DIRECTION_EAST] (free_slots_to_west[i + 1][j]);
	    t[i][j]->free_slots_neighbor[DIRECTION_SOUTH] (free_slots_to_north[i][j + 1]);
	    t[i][j]->free_slots_neighbor[DIRECTION_WEST] (free_slots_to_east[i][j]);

	    // NoP 
	    t[i][j]->NoP_data_out[DIRECTION_NORTH] (NoP_data_to_north[i][j]);
	    t[i][j]->NoP_data_out[DIRECTION_EAST] (NoP_data_to_east[i + 1][j]);
	    t[i][j]->NoP_data_out[DIRECTION_SOUTH] (NoP_data_to_south[i][j + 1]);
	    t[i][j]->NoP_data_out[DIRECTION_WEST] (NoP_data_to_west[i][j]);

	    t[i][j]->NoP_data_in[DIRECTION_NORTH] (NoP_data_to_south[i][j]);
	    t[i][j]->NoP_data_in[DIRECTION_EAST] (NoP_data_to_west[i + 1][j]);
	    t[i][j]->NoP_data_in[DIRECTION_SOUTH] (NoP_data_to_north[i][j + 1]);
	    t[i][j]->NoP_data_in[DIRECTION_WEST] (NoP_data_to_east[i][j]); */
	}
    }

    // dummy NoximNoP_data structure
    NoximNoP_data tmp_NoP;

    tmp_NoP.sender_id = NOT_VALID;

    for (int i = 0; i < DIRECTIONS; i++) {
	tmp_NoP.channel_status_neighbor[i].free_slots = NOT_VALID;
	tmp_NoP.channel_status_neighbor[i].available = false;
    }

    // Looping over the slices
    for (int k =0; k< SLICES; k++) {
    // Clear signals for borderline nodes
    for (int i = 0; i <= NoximGlobalParams::mesh_dim_x; i++) {
	req_to_south[k][i][0] = 0;
	ack_to_north[k][i][0] = 0;
	req_to_north[k][i][NoximGlobalParams::mesh_dim_y] = 0;
	ack_to_south[k][i][NoximGlobalParams::mesh_dim_y] = 0;
	/*
	free_slots_to_south[i][0].write(NOT_VALID);
	free_slots_to_north[i][NoximGlobalParams::mesh_dim_y].write(NOT_VALID);

	NoP_data_to_south[i][0].write(tmp_NoP);
	NoP_data_to_north[i][NoximGlobalParams::mesh_dim_y].write(tmp_NoP); */

    }

    for (int j = 0; j <= NoximGlobalParams::mesh_dim_y; j++) {
	req_to_east[k][0][j] = 0;
	ack_to_west[k][0][j] = 0;
	req_to_west[k][NoximGlobalParams::mesh_dim_x][j] = 0;
	ack_to_east[k][NoximGlobalParams::mesh_dim_x][j] = 0;

	/*free_slots_to_east[0][j].write(NOT_VALID);
	free_slots_to_west[NoximGlobalParams::mesh_dim_x][j].write(NOT_VALID);

	NoP_data_to_east[0][j].write(tmp_NoP);
	NoP_data_to_west[NoximGlobalParams::mesh_dim_x][j].write(tmp_NoP); */

    }

    // invalidate reservation table entries for non-exhistent channels
    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
	t[i][0]->r[k]->reservation_table.invalidate(DIRECTION_NORTH);
	t[i][NoximGlobalParams::mesh_dim_y - 1]->r[k]->reservation_table.invalidate(DIRECTION_SOUTH);
    }
    for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
	t[0][j]->r[k]->reservation_table.invalidate(DIRECTION_WEST);
	t[NoximGlobalParams::mesh_dim_x - 1][j]->r[k]->reservation_table.invalidate(DIRECTION_EAST);
    }
} // Looping over the slices
}

NoximTile *NoximNoC::searchNode(const int id) const
{
    for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++)
	for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++)
		for (int k=0; k < SLICES; k++)
	    if (t[i][j]->r[k]->local_id == id)
		return t[i][j];

    return NULL;
}

void NoximNoC::sim_stop_poller()
{
	//int status = 0;
	if (reset.read()){

		}

	else{
		total_sent_pkts = 0;
		total_recv_pkts  = 0;
		if((sc_time_stamp().to_double()/1000) > 1000){
	// Should poll for all the nodes and check if all of them are true
		for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
			for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
				// Creating a benchmark module for each tile

				total_sent_pkts +=  t[i][j]->pe->get_sent_packets();
				total_recv_pkts += t[i][j]->pe->get_recv_packets();
				//cout<<"=============="<<endl;
				//cout << "at time "<< sc_time_stamp().to_double()/1000 <<" stop signal is "<<status<<endl;
			}
		}
		}
		if(total_sent_pkts >= 1000000){
			cout<< " total packets "<< total_sent_pkts+total_recv_pkts <<endl;
			sc_stop();
		}
	}
}
