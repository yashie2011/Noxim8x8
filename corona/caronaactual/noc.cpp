#include "noc.h"

//---------------------------------------------------------------------------


void TNoC::buildCrossbar()
{
	// Arbiter pin assignments
    garbiter   = new TGlobalarbiter("Globalarbiter");
    // Map clock and reset
	  garbiter->clock(clock);
	  garbiter->reset(reset);
  // Create the grid as a matrix of tiles
  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
    {
      for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
	{
	  // Create the single Tile with a proper name
	  char tile_name[20];
	  sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	  git[i][j] = new TGITile(tile_name);

	  // Tell to the PE its coordinates
	  git[i][j]->pe->local_id = j * TGlobalParams::mesh_dim_x + i;
	  //cout << "pe local id : " <<i<<j<< git[i][j]->pe->local_id << endl;
          //git[i][j]->pe->never_transmit = (gttable.occurrencesAsSource(git[i][j]->pe->local_id) == 0);

	  // Map clock and reset
	  git[i][j]->clock(clock);
	  git[i][j]->reset(reset);
	  
	  // Map Arbiter signals
	  git[i][j]->req_tx_arbiter(req_tx_arbiter_signal[i][j]);
	  git[i][j]->ack_tx_arbiter(ack_tx_arbiter_signal[i][j]);
	  git[i][j]->ack_txc_arbiter(ack_txc_arbiter_signal[i][j]);
	  	
	  // Map Rx signals
	  git[i][j]->flit_rx(cross_bar[i][j]);
	  //cout << "Tile" << i << j << "input port connected to signal of cross bar"<< i << j<< endl;
	  // Map Tx signals
	   for(int k=0; k<(TGlobalParams::mesh_dim_x); k++) 
		{ 
			for(int l=0; l<(TGlobalParams::mesh_dim_y); l++) 
		{ 
			if(k == i && l == j)
			{ // Tell to the GWI its coordinates
				git[i][j]->flit_tx[k][l](cross_bar_additional);
				//cout << "Tile" << i << j << "out put port"<<k<<l<<"connected to dummy signal of cross bar"<< endl;
				git[i][j]->gwi->local_xid = i;
				git[i][j]->gwi->local_yid = j;
				 git[i][j]->gwi->local_id = j * TGlobalParams::mesh_dim_x + i;
			} else
			{
			git[i][j]->flit_tx[k][l](cross_bar[k][l]);
			//cout << "Tile" << i << j << "out put port"<<k<<l<<"connected to signal of cross bar"<<k<<l<< endl;
		}
		}
		}

	  // connect arbiter with tiles
	  
	   garbiter->req_tx_arbiter[i][j](req_tx_arbiter_signal[i][j]);
	   garbiter->ack_tx_arbiter[i][j](ack_tx_arbiter_signal[i][j]);
	   garbiter->ack_txc_arbiter[i][j](ack_txc_arbiter_signal[i][j]);

	}
    }

}

TGITile* TNoC::searchNode(const int id) const
{
  for (int i=0; i<TGlobalParams::mesh_dim_x; i++)
    for (int j=0; j<TGlobalParams::mesh_dim_y; j++)
      if (git[i][j]->gwi->local_id == id)
	return git[i][j];

  return false;
}

//---------------------------------------------------------------------------

