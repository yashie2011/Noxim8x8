/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "NoximProcessingElement.h"

extern ofstream slice_0_trace;
extern ofstream slice_1_trace;
extern ofstream slice_2_trace;
extern ofstream slice_3_trace;
extern ofstream slice_4_trace;

extern ofstream pre;
extern ofstream pre_reply;


int NoximProcessingElement::randInt(int min, int max)
{
    return min +
	(int) ((double) (max - min + 1) * rand() / (RAND_MAX + 1.0));
}


void NoximProcessingElement::sim_stop_poll()
{
	if (reset.read()){
		sim_stop = 0;
	}
	else
	{
		//cout << "Entered sim_Stop"<<endl;
		if(!is_mc(id2Coord(local_id)))
		{
			//cout<<" The nodeid "<< local_id <<" sent "<< sent_requests <<" reqs and the trace len is "<<b_mark.get_trace_len() <<endl;
			if(sent_requests >= (b_mark.get_trace_len()-2000)){

				cout << std::fixed;
				cout << std::setprecision(8);
				sim_stop = 1;
				if(sent_requests == (b_mark.get_trace_len()-2000)){
					cout<<"the processing element "<<local_id<<"stops at "<<  sc_time_stamp().to_double()<<endl;
					sent_requests++;
				}
			}
			else
				sim_stop = 0;
		}
		else {
			//cout<<" The nodeid "<< local_id <<" is a memory controller "<< endl;
			sim_stop =1;
		}
	}
}
void NoximProcessingElement::rxProcess()
{

    if (reset.read()) {
    //looping over the slices
    	for(int k=0; k<SLICES; k++){
    		ack_rx[k].write(0);
    			current_level_rx[k] = 0;
    			rx_flits[k]=0;
    	}

    } else {
    	// Read from each slice -- arbitration no priority.. Just reading in order
    	if(!pre.is_open())
    		 pre.open("pre.log", ios::out | ios::app);

    	for(int k=0; k< SLICES; k++){
    		if (req_rx[k].read() == 1 - current_level_rx[k]) {
    			    NoximFlit flit_tmp = flit_rx[k].read();
    			    NoximCoord dest_id = id2Coord(flit_tmp.dst_id);
    			    NoximCoord src_id = id2Coord(flit_tmp.src_id);
    			    // Then check if the current PE is a memory controller
    			    //pre << "at node "<< local_id<<" ack_msg: "<<flit_tmp.ack_msg<<" last_packet: "<<last_packet<<endl;
    			    if(is_mc(dest_id)){

        			    // If yes, push a structure of reply_info into the queue
    			    	if(flit_tmp.flit_type == FLIT_TYPE_HEAD){
    			    	reply_data temp_reply;
    			    	temp_reply.data_size= flit_tmp.data_size;
    			    	temp_reply.dest_id = flit_tmp.src_id;
    			    	temp_reply.return_time = (sc_time_stamp().to_double() / 1000) + randInt(120, 150);
    			    	temp_reply.ack_msg = flit_tmp.timestamp;
    			    	//cout<<" packet received at MC: src_id == "<< flit_tmp.src_id<<" dest_id == "<<local_id<<endl;
    			    	//pre <<" with time stamp: "<< flit_tmp.timestamp<< " received at: "<< (sc_time_stamp().to_double() / 1000);
    			    	//pre << " scheduled to be sent at: "<<(temp_reply.return_time)<<endl;
    			    	reply_queue.push_back(temp_reply);
    			    	//pre <<"reply queue length: "<< reply_queue.size()<<endl;
    					//pre_reply<<"printing queue contents ";
    					//for(std::vector<reply_data>::const_iterator i = reply_queue.begin() ; i != reply_queue.end(); i++ ){
    					//	pre_reply<< i->return_time <<" ";
    					//}
    					//pre_reply<<endl;
    			    	}

        			    // else, do nothing

    			    }
    			    else
    			    {
    			    	if( flit_tmp.ack_msg == last_packet && flit_tmp.flit_type == FLIT_TYPE_HEAD )
    			    	{
    			    		// Start a timer that should be increment at every cycle
    			    		start_clock = true;
    	   			    	//cout<<" packet received at nonMC: src_id == "<< flit_tmp.src_id<<" dest_id == "<<local_id<<endl;
    	    			   // pre<<" with time stamp: "<< flit_tmp.timestamp<< " received at: "<< (sc_time_stamp().to_double() / 1000)<<endl;
    			    	  //last_packet = 0;
    			    	}
    			    }

    			    if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
    				cout << sc_simulation_time() << ": ProcessingElement[" <<
    				    local_id << "] RECEIVING " << flit_tmp << endl;
    			    }
    			    current_level_rx[k] = 1 - current_level_rx[k];	// Negate the old value for Alternating Bit Protocol (ABP)
    			    rx_flits[k]++;
    			}
    			ack_rx[k].write(current_level_rx[k]);

    	}

    }
}

// Get the slice of the destination
int NoximProcessingElement::get_slice(NoximCoord src_coord, NoximCoord dest_coord){
		// Base case : if the request data, slice 0, reply data slice 2

	// checking for the memory controller this needs to be modified when changing the mesh size
	bool mem_controller = false;
	if((src_coord.x == 0 && src_coord.y == 2)||(src_coord.x == 2 && src_coord.y == 0)
			||(src_coord.x == 1 && src_coord.y == 3)||(src_coord.x == 3 && src_coord.y == 1))
		mem_controller = true;
	//===================================================================
	if (mem_controller){
		return 1;
	}
	else return 0;    // Lowest slice

}

void NoximProcessingElement::txProcess()
{
    if (reset.read()) {
    for(int k=0; k< SLICES; k++){
    	req_tx[k].write(0);
    		current_level_tx[k] = 0;
    		tx_flits[k]=0;

    }
	send = true;
	computation_time = 0;
	computation_clock = 0;
	start_clock = false;
	last_packet = 0;
	transmittedAtPreviousCycle = false;
    } else {

    	if(start_clock)
    		computation_clock++;
    	if (start_clock && computation_clock >= computation_time)
    		send = true;


    	canShot();
    // if MC just call push_packet
    	if(is_mc(id2Coord(local_id))){
    		push_packet();
    	}
    	else  // Otherwise, check if the last packet has received reply and then call push packet
    	{
    		//pre<< "at node "<< local_id<<" nonMC the send flag is "<< send<<endl;
    		if(send)
    		{
    			push_packet();
    		}
    	}


	/*NoximFlit temp_flit = nextFlit_view();
	src_coord = id2Coord(local_id);
	dest_coord = id2Coord(temp_flit.dst_id);
	slice = get_slice(src_coord, dest_coord);*/

	for(int slice =0; slice<SLICES; slice++){

	if (ack_tx[slice].read() == current_level_tx[slice]) {
	    if (!packet_queue[slice].empty()) {
		NoximFlit flit = nextFlit(slice);	// Generate a new flit
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
		    cout << sc_time_stamp().to_double() / 
			1000 << ": ProcessingElement[" << local_id <<
			"] SENDING " << flit << endl;
		}

		if(flit.flit_type == FLIT_TYPE_HEAD){
			sent_requests++;
					/*switch (slice){
					case 0:
						slice_0_trace<<"node "<<this->local_id;
						slice_0_trace<<" at slice 0: sending flits with "<< flit.src_id<< " "<<flit.dst_id<<" type "<<flit.flit_type;
						slice_0_trace<<" at "<< sc_time_stamp().to_double() / 1000<< endl;
						break;

					case 1:
						slice_1_trace<<"node "<<this->local_id;
						slice_1_trace<<" slice 1 sending flits with "<< flit.src_id<< " "<<flit.dst_id<<" type "<<flit.flit_type;
						slice_1_trace<<" at "<< sc_time_stamp().to_double() / 1000<< endl;
						break;
					case 2:
						slice_2_trace<<"node "<<this->local_id;
						slice_2_trace<<" slice 2 sending flits with "<< flit.src_id<< " "<<flit.dst_id<<" type "<<flit.flit_type;
						slice_2_trace<<" at "<< sc_time_stamp().to_double() / 1000<< endl;
						break;
					case 3:
						slice_3_trace<<"node "<<this->local_id;
						slice_3_trace<<" slice 3 sending flits with "<< flit.src_id<< " "<<flit.dst_id<<" type "<<flit.flit_type;
						slice_3_trace<<" at "<< sc_time_stamp().to_double() / 1000<< endl;
						break;
					case 4:
						slice_4_trace<<"node "<<this->local_id;
						slice_4_trace<<" slice 4 sending flits with "<< flit.src_id<< " "<<flit.dst_id<<" type "<<flit.flit_type;
						slice_4_trace<<" at "<< sc_time_stamp().to_double() / 1000<< endl;
						break;
					}*/

				}
		flit_tx[slice]->write(flit);	// Send the generated flit
		current_level_tx[slice] = 1 - current_level_tx[slice];	// Negate the old value for Alternating Bit Protocol (ABP)
		req_tx[slice].write(current_level_tx[slice]);
		tx_flits[slice]++;
	    }
	}
	}  // loop over slices
    }
}

void NoximProcessingElement::push_packet(){
	NoximPacket packet;
	int slice = 0;
	NoximCoord src_coord = {0};
	NoximCoord dest_coord = {0};

	if (true) {

		/// Decide which layer the packet should be sent..
		src_coord = id2Coord(local_id);
		if (!interface_buf.empty())  {  // This should cover the entire module
		packet = interface_buf.front();
		dest_coord = id2Coord(packet.dst_id);
		slice = get_slice(src_coord, dest_coord);

		if( packet_queue[slice].size() < 150 ){//&& packet.packet_returned)   {   // Added since canshot is irrelevant now!

			packet.timestamp =  sc_time_stamp().to_double() /1000;
			//cout<<" Packet with src "<< packet.src_id<< " dest_"<< packet.dst_id<<" queued, "<<endl;
			if(!is_mc(src_coord))
			{
				last_packet = packet.timestamp;
				start_clock = false;
				send = false;
				computation_time = packet.computation_time;
			}
			else {

		//	pre_reply<< "buffer lenght so far "<<packet_queue[slice].size() <<endl;
			}
			packet_queue[slice].push(packet);
			interface_buf.pop();
		}

	}  // this happens only when the interace_buff is not empty
	    transmittedAtPreviousCycle = true;
	} else
	    transmittedAtPreviousCycle = false;


}

NoximFlit NoximProcessingElement::nextFlit(int slice)
{
    NoximFlit flit;
    NoximPacket packet = packet_queue[slice].front();
    if(!packet_queue[slice].empty()){
    flit.src_id = packet.src_id;
    flit.dst_id = packet.dst_id;
    flit.timestamp = packet.timestamp;
    flit.sequence_no = packet.size - packet.flit_left;
    flit.hop_no = 0;
    //  flit.payload     = DEFAULT_PAYLOAD;
    flit.use_low_voltage_path = packet.use_low_voltage_path;
    flit.data_size = packet.reply_data_size;

    //to calculate app exe time
    flit.ack_msg = packet.ack_msg;

    if (packet.size == packet.flit_left)
	flit.flit_type = FLIT_TYPE_HEAD;
    else if (packet.flit_left == 1)
	flit.flit_type = FLIT_TYPE_TAIL;
    else
	flit.flit_type = FLIT_TYPE_BODY;

    packet_queue[slice].front().flit_left--;
    if (packet_queue[slice].front().flit_left == 0)
	packet_queue[slice].pop();
    }
    return flit;
}


bool NoximProcessingElement::canShot()
{
    bool shot;
    double threshold;
    NoximPacket packet;

    if (NoximGlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED) {
	if (!transmittedAtPreviousCycle)
	    threshold = NoximGlobalParams::packet_injection_rate;
	else
	    threshold = NoximGlobalParams::probability_of_retransmission;

	//shot = (((double) rand()) / RAND_MAX < threshold);  We cant use this in benchmark traffic
	shot = true;
	if (1){  // (shot) {    Changed to look for a packet at each cycle
	    switch (NoximGlobalParams::traffic_distribution) {
	    case TRAFFIC_RANDOM:
		packet = trafficRandom();
		break;

	    case TRAFFIC_TRANSPOSE1:
		packet = trafficTranspose1();
		break;

	    case TRAFFIC_TRANSPOSE2:
		packet = trafficTranspose2();
		break;

	    case TRAFFIC_BIT_REVERSAL:
		packet = trafficBitReversal();
		break;

	    case TRAFFIC_SHUFFLE:
		packet = trafficShuffle();
		break;

	    case TRAFFIC_BUTTERFLY:
		packet = trafficButterfly();
		break;

	    case TRAFFIC_BENCHMARK:
	    	packet = trafficBenchmark();
	    	break;

	    default:
		assert(false);
	    }
	    setUseLowVoltagePath(packet);
	}

	// Remove it from here for better purposes


    } else {			// Table based communication traffic
	if (never_transmit)
	    return false;

	double now = sc_time_stamp().to_double() / 1000;
	bool use_pir = (transmittedAtPreviousCycle == false);
	vector < pair < int, double > > dst_prob;
	vector <bool> use_low_voltage_path;
	double threshold =
	    traffic_table->getCumulativePirPor(local_id, (int) now,
					       use_pir, dst_prob, use_low_voltage_path);

	double prob = (double) rand() / RAND_MAX;
	shot = (prob < threshold);
	if (shot) {
	    for (unsigned int i = 0; i < dst_prob.size(); i++) {
		if (prob < dst_prob[i].second) {
		    packet.make(local_id, dst_prob[i].first, now,
				getRandomSize(), 0);
		    packet.use_low_voltage_path = use_low_voltage_path[i];
		    break;
		    sc_time_stamp;
		}
	    }
	}
    }

    return shot;
}

void NoximProcessingElement::setUseLowVoltagePath(NoximPacket& packet)
{
  if (NoximGlobalParams::qos < 1.0)
    {
      double rnd = (double)rand() / (double)RAND_MAX;

      packet.use_low_voltage_path = (rnd >= NoximGlobalParams::qos);
      /*
      if (rnd >= NoximGlobalParams::qos)
	packet.use_low_voltage_path = true;
      else
	packet.use_low_voltage_path = false;
      */
    }
  else
    packet.use_low_voltage_path = false;
}

NoximPacket NoximProcessingElement::trafficRandom()
{
    NoximPacket p;
    p.src_id = local_id;
    double rnd = rand() / (double) RAND_MAX;
    double range_start = 0.0;

    //cout << "\n " << sc_time_stamp().to_double()/1000 << " PE " << local_id << " rnd = " << rnd << endl;

    int max_id =
	(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y) - 1;

    // Random destination distribution
    do {
	p.dst_id = randInt(0, max_id);

	// check for hotspot destination
	for (uint i = 0; i < NoximGlobalParams::hotspots.size(); i++) {
	    //cout << sc_time_stamp().to_double()/1000 << " PE " << local_id << " Checking node " << NoximGlobalParams::hotspots[i].first << " with P = " << NoximGlobalParams::hotspots[i].second << endl;

	    if (rnd >= range_start
		&& rnd <
		range_start + NoximGlobalParams::hotspots[i].second) {
		if (local_id != NoximGlobalParams::hotspots[i].first) {
		    //cout << sc_time_stamp().to_double()/1000 << " PE " << local_id <<" That is ! " << endl;
		    p.dst_id = NoximGlobalParams::hotspots[i].first;
		}
		break;
	    } else
		range_start += NoximGlobalParams::hotspots[i].second;	// try next
	}
    } while (p.dst_id == p.src_id);

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();
    return p;
}

NoximPacket NoximProcessingElement::trafficTranspose1()
{
    NoximPacket p;
    p.src_id = local_id;
    NoximCoord src, dst;

    // Transpose 1 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = NoximGlobalParams::mesh_dim_x - 1 - src.y;
    dst.y = NoximGlobalParams::mesh_dim_y - 1 - src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficTranspose2()
{
    NoximPacket p;
    p.src_id = local_id;
    NoximCoord src, dst;

    // Transpose 2 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = src.y;
    dst.y = src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

// Benchmark traffic generator
NoximPacket NoximProcessingElement::trafficBenchmark(){

	NoximPacket p;
	p.src_id = local_id;
	NoximCoord src_coord, dst;
	src_coord = id2Coord(local_id);
	p.packet_returned = false;

	// If the source tile is a memory controller tile, then choose reply trace check

	// checking for the memory controller this needs to be modified when changing the mesh size
		bool mem_controller = is_mc(src_coord);
	//===================================================================
		if(mem_controller){
			// check the queue for a packet
			double current_time =  sc_time_stamp().to_double() / 1000;
			// if its current time is matching with the return_time

			if(!reply_queue.empty()) {
				struct compare my_compare;
				std::sort(reply_queue.begin(), reply_queue.end(), my_compare);
				/*pre_reply<<"nodeid "<<local_id<<"printing queue contents ";
				for(std::vector<reply_data>::const_iterator i = reply_queue.begin() ; i != reply_queue.end(); i++ ){
					pre_reply<< i->return_time <<" ";
				}*/
				//pre_reply<<endl;
				if( !reply_queue.empty()  && current_time <=  reply_queue.front().return_time){
					reply_data temp_reply = reply_queue.front();
					//pre_reply<<" Cycle matched at MC "<< current_time;
					//pre_reply <<"reply queue length: "<< reply_queue.size();
					p.packet_returned = true;
					p.dst_id = temp_reply.dest_id;
					p.flit_left = (temp_reply.data_size/16)+2;
					p.timestamp = sc_time_stamp().to_double() / 1000;
					p.size = p.flit_left;
					p.ack_msg = temp_reply.ack_msg;
					//cout<<"MC sending the packet with src "<< src_coord<<" and dest "<< id2Coord(p.dst_id) << " size "<< p.size<<" flag "<<p.packet_returned<<endl;
					reply_queue.erase(reply_queue.begin());
					if(p.packet_returned)
					{
						//cout<< "packet returned by bench.cpp in "<< local_id<<" at "<<p.timestamp<<endl;
						interface_buf.push(p);
					}// Adding into the infinite buffer
			}
			}

			 //send a packet to the corresponding destination
			 //else dont respond
		}

	// else if the source tile is not a memory controller tile, then choose a request trace check
		//else{
		if(!mem_controller){
			bool flag;
			flag= b_mark.packet_injection_response_req(local_id, ret_comm);    //Core node Ids are handled in the benchmark function
			//cout<< "Benchmark rep pack gen at: "<<sc_time_stamp().to_double()/1000;
			//cout<< "Shader Core  src id "<<local_id<<" dest id "<<temp_comm.dest_id;
			//cout<< "Packet present status: "<< temp_comm.status<<endl;


			if(flag){
				// Iterate over the vector and push the packets into the int_buffer
				for(std::vector<comm>::iterator it = ret_comm.begin(); it != ret_comm.end(); ++it) {
				    /* std::cout << *it; ... */
					p.dst_id = it->dest_id;
					p.flit_left =  3;    // 2+(temp_comm.data_size/8);    // Request network sends 3 flits only
					p.timestamp = sc_time_stamp().to_double() / 1000;
					p.size = p.flit_left;
					p.packet_returned = flag;
					p.reply_data_size = it->data_size;
					p.computation_time =it->comp_time;
					if(p.packet_returned)
					{
						//cout<< "packet returned by bench.cpp in "<< local_id<<" at "<<p.timestamp<<endl;
						interface_buf.push(p);
					}// Adding into the infinite buffer
				}
				ret_comm.clear();  // Clearing the vector to be sent in the next cycle
			}
		}
   //=======================================================================
		return p;

}

void NoximProcessingElement::setBit(int &x, int w, int v)
{
    int mask = 1 << w;

    if (v == 1)
	x = x | mask;
    else if (v == 0)
	x = x & ~mask;
    else
	assert(false);
}

int NoximProcessingElement::getBit(int x, int w)
{
    return (x >> w) & 1;
}

inline double NoximProcessingElement::log2ceil(double x)
{
    return ceil(log(x) / log(2.0));
}

NoximPacket NoximProcessingElement::trafficBitReversal()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits; i++)
	setBit(dnode, i, getBit(local_id, nbits - i - 1));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficShuffle()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits - 1; i++)
	setBit(dnode, i + 1, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficButterfly()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 1; i < nbits - 1; i++)
	setBit(dnode, i, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));
    setBit(dnode, nbits - 1, getBit(local_id, 0));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

void NoximProcessingElement::fixRanges(const NoximCoord src,
				       NoximCoord & dst)
{
    // Fix ranges
    if (dst.x < 0)
	dst.x = 0;
    if (dst.y < 0)
	dst.y = 0;
    if (dst.x >= NoximGlobalParams::mesh_dim_x)
	dst.x = NoximGlobalParams::mesh_dim_x - 1;
    if (dst.y >= NoximGlobalParams::mesh_dim_y)
	dst.y = NoximGlobalParams::mesh_dim_y - 1;
}

int NoximProcessingElement::getRandomSize()
{
    return randInt(NoximGlobalParams::min_packet_size,
		   NoximGlobalParams::max_packet_size);
}

bool NoximProcessingElement::is_mc(NoximCoord src_coord)
{
	if((src_coord.x == 0 && src_coord.y == 2)||(src_coord.x == 2 && src_coord.y == 0)
					||(src_coord.x == 1 && src_coord.y == 3)||(src_coord.x == 3 && src_coord.y == 1))
				return true;
	else
		return false;
}

