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


memory_controller::memory_controller() {

	tRAS = 100;   //ns
	tCAS = 8;     // ns
	tRCD = 10;    // ns
	max_queue_depth = 16;
	last_bank_q = 0;
	//bank_queues = new queue<NoximPacket> [NoximGlobalParams::bank_queues];
	for (int i =0; i< NoximGlobalParams::bank_queues ; i++){
		bank_queues.push_back(queue<NoximPacket>());
	}

}

memory_controller::~memory_controller(){

	bank_queues.clear();
}



bool memory_controller::push_packet(NoximFlit hd_flit){

	// select the suitable the queues
	int q_indx = hd_flit.payload.bank % NoximGlobalParams::bank_queues;
	NoximPacket tmp_p;
	tmp_p.src_id = hd_flit.dst_id;
	tmp_p.dst_id = hd_flit.src_id;
	tmp_p.ack_msg = hd_flit.timestamp;
	tmp_p.size = REPLY_DATA_SIZE/FLIT_SIZE + 2;
	tmp_p.approximable = hd_flit.approximable;
	tmp_p.flit_left = tmp_p.size;
	tmp_p.payload = hd_flit.payload;

	// if the queue is not empty, use the last packet's return timestamp, for current packets timestamp
	if (! bank_queues[q_indx].empty() ){

		if(hd_flit.payload.row == bank_queues[q_indx].back().payload.row){
			tmp_p.timestamp = (sc_time_stamp().to_double()/1000) + tCAS + tRCD;
		}
		else{
			tmp_p.timestamp = (sc_time_stamp().to_double()/1000) + tCAS + tRCD + tRAS;
		}

	}
	else{
		tmp_p.timestamp = (sc_time_stamp().to_double()/1000) + tCAS + tRCD + tRAS;
	}

	if(bank_queues[q_indx].size() < 20 *NoximGlobalParams::buffer_depth){
		//cout<<"pushing packet into reply queue "<< tmp_p.src_id<<" "<<tmp_p.dst_id<<endl;
		bank_queues[q_indx].push(tmp_p);
		return true;
	}
	else
		return false;
	//
}

bool memory_controller::get_packets(deque<NoximPacket>& interface_buf){

	bool ret_flag = false;
	// go through all the bank queues
	for(int q =0; q < NoximGlobalParams::bank_queues; q++){
		int indx = (q + last_bank_q) % NoximGlobalParams::bank_queues;
		if(bank_queues[indx].empty()) continue;
		else if (bank_queues[indx ].front().timestamp <= sc_time_stamp().to_double()/1000) {
			if(1)//(interface_buf.size() < 20 * NoximGlobalParams::buffer_depth)
			{
				//cout<<"reply packet collected at mc"<< bank_queues[q].front().src_id<<
						//bank_queues[q].front().dst_id<<endl;
				interface_buf.push_back(bank_queues[indx ].front());
				bank_queues[indx ].pop();
				ret_flag |= true;
				break;
			}
		}
	}
	last_bank_q = (last_bank_q + 1 )% NoximGlobalParams::bank_queues;
	return ret_flag;
}

int NoximProcessingElement::randInt(int min, int max)
{
    return min +
	(int) ((double) (max - min + 1) * rand() / (RAND_MAX + 1.0));
}


void NoximProcessingElement::rxProcess()
{

    if (reset.read()) {

    		ack_rx.write(0);
    			current_level_rx = 0;
    			rx_flits=0;
    			num_reqs = 0;
    			max_buffer_size=0;


    }
    else {
    	// Read from each slice -- arbitration no priority.. Just reading in order

    	if (req_rx.read() == 1 - current_level_rx) {
    			NoximFlit flit_tmp;
    			flit_tmp = flit_rx.read();
    			NoximCoord dest_id = id2Coord(flit_tmp.dst_id);
    			NoximCoord src_id = id2Coord(flit_tmp.src_id);
    			if(is_mc(dest_id)){

    				bool pkt_pushed = true;
					// Then check if the current PE is a memory controller
					// If head flit, push the packet into mem_c and then send
    				// read ack to the sender router
					if(flit_tmp.flit_type == NoximFlitType::FLIT_TYPE_HEAD){
						pkt_pushed = mem_c->push_packet(flit_tmp);

					//cout << sc_simulation_time() << ": ProcessingElement[" <<
					    				    //local_id << "] RECEIVING " << flit_tmp << endl;
						if(pkt_pushed){
							num_reqs++;
							received_packets++;
							// Negate the old value for Alternating Bit Protocol (ABP)
							current_level_rx = 1 - current_level_rx;
							rx_flits++;
							pe_pwr.Buffering();
						}
					}
					// else send read ack to the sender router anyway
					else{
						current_level_rx = 1 - current_level_rx;
						rx_flits++;
						pe_pwr.Buffering();
					}

    			  }
    			  else{
    			    if( flit_tmp.flit_type == NoximFlitType::FLIT_TYPE_HEAD ){
    			    		// Start a timer that should be increment at every cycle
    			    		start_clock = true;
    			    		received_packets++;

    			    		cout << sc_simulation_time() << ": ProcessingElement[" <<
    			    		    				    local_id << "] RECEIVING " << flit_tmp << endl;
    			    	//cout<<" with time stamp: "<< flit_tmp.timestamp<< " received at: "<< (sc_time_stamp().to_double()/1000)<<endl;
    			    	  //last_packet = 0;
    			    	}
        			    current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)
        			    rx_flits++;
    			    }

    			    if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
    				cout << sc_simulation_time() << ": ProcessingElement[" <<
    				    local_id << "] RECEIVING " << flit_tmp << endl;
    			    }

    			}
    			ack_rx.write(current_level_rx);



    }
}



void NoximProcessingElement::txProcess()
{
    if (reset.read()) {
    	req_tx.write(0);
    		current_level_tx = 0;
    		tx_flits=0;


	send = true;
	computation_clock = 0;
	start_clock = false;
	last_packet = 0;
	transmittedAtPreviousCycle = false;

    }

    else {


    	//canShot();
    	// if MC just call push_packet
    	if(canShot()){//;is_mc(id2Coord(local_id))){
    		push_packet();
    		pe_pwr.Buffer_read();
    	}
    	else  // Otherwise, check if the last packet has received reply and then call push packet
    	{
    		//push_packet();

    	}

    if(1){


		if (ack_tx.read() == current_level_tx) {
			if (!packet_queue.empty()) {
			NoximFlit flit = nextFlit();	// Generate a new flit
			//cout<<"Sending flit to slice 1 at pe of node "<<local_id<<endl;
			if(flit.flit_type == NoximFlitType::FLIT_TYPE_HEAD) sent_packets++;
			if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
				cout << sc_time_stamp().to_double() /
				1000 << ": ProcessingElement[" << local_id <<
				"] SENDING " << flit << endl;
			}
			flit_tx->write(flit);	// Send the generated flit
			current_level_tx = 1 - current_level_tx;	// Negate the old value for Alternating Bit Protocol (ABP)
			req_tx.write(current_level_tx);
			tx_flits++;

			}
		}

    }
    }
}

void NoximProcessingElement::push_packet(){
	NoximPacket packet;
	NoximCoord src_coord = {0};
	NoximCoord dest_coord = {0};

	if (true) {

		/// Decide which layer the packet should be sent..
		src_coord = id2Coord(local_id);
		if (!interface_buf.empty() )  {  // This should cover the entire module
		packet = interface_buf.front();
		if(is_mc(packet.src_id)){

			//approximate(packet, interface_buf);
		}
		dest_coord = id2Coord(packet.dst_id);

		if (packet_queue.size() < 10 * NoximGlobalParams::buffer_depth ){//&& packet.packet_returned)   {   // Added since canshot is irrelevant now!
			//cout<<" Packet with src "<< packet.src_id<< " dest_"<< packet.dst_id<<" queued in slice "<<slice<<endl;
			packet.timestamp =  sc_time_stamp().to_double()/1000 ;
			//if(slice == 1)

			if(!is_mc(src_coord))
			{
				last_packet = packet.timestamp;
			}
			else {
			//cout<< "packet queue lenght so far "<<packet_queue[slice].size() <<endl;
			}
			packet_queue.push(packet);
			if(max_buffer_size < interface_buf.size())
				max_buffer_size = interface_buf.size();
			interface_buf.pop_front();
		}

	}  // this happens only when the interace_buff is not empty
	transmittedAtPreviousCycle = true;
	}
	else
	    transmittedAtPreviousCycle = false;


}

NoximFlit NoximProcessingElement::nextFlit()
{
    NoximFlit flit;
    NoximPacket packet = packet_queue.front();
    if(!packet_queue.empty()){
    flit.src_id = packet.src_id;
    flit.dst_id = packet.dst_id;
    flit.timestamp = packet.timestamp;
    flit.sequence_no = packet.size - packet.flit_left;
    flit.hop_no = 0;

    flit.use_low_voltage_path = packet.use_low_voltage_path;
    flit.data_size = FLIT_SIZE;
    flit.flit_sent = true;
    flit.tag = randInt(1, 500);
	if(packet.approximable)
	{
		flit.approximable = true;

	}


    //to calculate app exe time
    flit.ack_msg = packet.ack_msg;

    if (packet.size == packet.flit_left)
    {
    	flit.flit_type = NoximFlitType::FLIT_TYPE_HEAD;
    	flit.payload = packet.payload;

    }
    else if (packet.flit_left == 1)
	flit.flit_type = NoximFlitType::FLIT_TYPE_TAIL;
    else
	flit.flit_type = NoximFlitType::FLIT_TYPE_BODY;

    packet_queue.front().flit_left--;
    if (packet_queue.front().flit_left == 0)
	packet_queue.pop();
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
		break;
	    }
	    setUseLowVoltagePath(packet);
	}

	// Remove it from here for better purposes


    }

    else {			// Table based communication traffic
	if (never_transmit)
	    return false;

	double now = sc_time_stamp().to_double()/1000 ;
	bool use_pir = (transmittedAtPreviousCycle == false);
	vector < pair < int, double > > dst_prob;
	vector <bool> use_low_voltage_path;
	double threshold =
	    traffic_table->getCumulativePirPor(local_id, (int) now, use_pir, dst_prob, use_low_voltage_path);

	double prob = (double) rand() / RAND_MAX;
	shot = (prob < threshold);
	if (shot) {
	    for (unsigned int i = 0; i < dst_prob.size(); i++) {
		if (prob < dst_prob[i].second) {
		    packet.make(local_id, dst_prob[i].first, now,
				getRandomSize());
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

    p.timestamp = sc_time_stamp().to_double()/1000 ;
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

    p.timestamp = sc_time_stamp().to_double()/1000 ;
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

    p.timestamp = sc_time_stamp().to_double()/1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

// Benchmark traffic generator
NoximPacket NoximProcessingElement::trafficBenchmark(){

	NoximPacket p;
	NoximCoord src_coord, dst;
	src_coord = id2Coord(local_id);

	// If the source tile is a memory controller tile, then choose reply trace check

	// checking for the memory controller this needs to be modified when changing the mesh size
	bool mem_controller = is_mc(src_coord);
	if(mem_controller){

		mem_c->get_packets(interface_buf);
	}
	else{
		trafficDB();
	}
	return p;
}



NoximPacket NoximProcessingElement::trafficDB(){
	NoximPacket p;
	p.src_id = local_id;
	NoximCoord src_coord, dst;
	src_coord = id2Coord(local_id);
	double time_stamp = sc_time_stamp().to_double()/1000;
	stringstream table_strm;
	table_strm.str("");
	int trace_core_id = cmap->get_trace_core_id(local_id);
	assert (trace_core_id >= 0);

	table_strm<<"node"<<trace_core_id;
	string table_str = table_strm.str();
	const char* table_name = table_str.c_str();
	memc_data *temp_ptr = get_mem_comm_from_trace( time_stamp, table_name);
	memc_data *del_ptr = NULL;
	//if (local_id == 1) exit(0);
	if(!is_mc(src_coord)){
		// get mem_trace from DB pointer
		while (temp_ptr != NULL
				&& (interface_buf.size() <  20 *NoximGlobalParams::buffer_depth)){
			//cout<<" local id "<<local_id<<endl;
			//temp_ptr->print();
			p.dst_id = cmap->get_chip_mcid(temp_ptr->core);
			p.src_id = local_id;
			p.timestamp = time_stamp;
			p.size = 3;
			p.flit_left = p.size;
			p.payload.bank = temp_ptr->bank;
			p.payload.col = temp_ptr->col;
			p.payload.row = temp_ptr->row;

			if (temp_ptr->addr >= NoximGlobalParams::address_min
					&& temp_ptr->addr <= NoximGlobalParams::address_max)
				p.approximable = true;
			else p.approximable = false;

			del_ptr = temp_ptr;
			temp_ptr = temp_ptr->next;
			delete del_ptr;

			//cout<< "packet returned by bench.cpp in "<< local_id<<" at "<<p.timestamp<<endl;
			interface_buf.push_back(p);// Adding into the infinite buffer
			// create a packet from comm data
		}

		return p;
	}
	return p;
}

memc_data* NoximProcessingElement::get_mem_comm_from_trace(double ts, const char *table_name){

	assert(table_name != NULL);
	memc_data *head_ptr, *mem_trace ;
	std::stringstream sql_str;
	std::stringstream result_val;
	result_val.str("");
	sql_str.str("");
	//cout <<"table name is "<<table_name<<endl;
	sql_str<<"SELECT * FROM "<<table_name<<" WHERE ts = "<<(int)ts<<";" ;
	const string tmp_str = sql_str.str();
	const char* sql_c_str = tmp_str.c_str();
	//
	IResult *res=trace_db_p->ExcuteSelect(sql_c_str);
	head_ptr = NULL; mem_trace = NULL;

		if(!res)
		  cout<<"\n Error:"<<trace_db_p->GetLastError().c_str()<<endl;
		else
		{

			//Get Column Count
			int i = res->GetColumnCount();
			assert(i>=7);
			memc_data *prev_ptr = NULL;
			while (res->Next()){
				//cout<<sql_c_str<<endl;
				mem_trace = new memc_data;
				result_val.clear();
				result_val << res->ColomnData(6);
				result_val >> mem_trace->addr;
				mem_trace->approx = false;
				result_val.clear();
				result_val<<res->ColomnData(0);
				result_val>>mem_trace->ts;
				result_val.clear();
				result_val<<res->ColomnData(3);
				result_val >> mem_trace->bank ;
				result_val.clear();
				result_val<<res->ColomnData(1);
				result_val >> mem_trace->row;
				result_val.clear();
				result_val<<res->ColomnData(2);
				result_val >> mem_trace->col;
				result_val.clear();
				result_val<<res->ColomnData(5);
				result_val >> mem_trace->core;

				mem_trace->next = NULL;
				if(head_ptr == NULL){
					head_ptr = mem_trace;
					prev_ptr = mem_trace;
				}
				else{
					prev_ptr->next = mem_trace;
					prev_ptr = mem_trace;
				}
			}

			res->Release();

		}
		return head_ptr;
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

    p.timestamp = sc_time_stamp().to_double()/1000 ;
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

    p.timestamp = sc_time_stamp().to_double()/1000 ;
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

    p.timestamp = sc_time_stamp().to_double()/1000 ;
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


int NoximProcessingElement::get_reply_queue_size(){ // Needs change

	return packet_queue.size();

}
bool NoximProcessingElement::reply_queue_full(){
	//cout<<"reply queue size: "<<reply_queue.size()<<endl;
	if(reply_queue.size() < MC_BUFF_CAP)
		return false;
	else
		return true;
}

void NoximProcessingElement::approximate(NoximPacket& pkt, deque <NoximPacket>& interface_buf){

	double err_thresh = 0;
	float approx_ratio = NoximGlobalParams::approx_rate;


		if (is_mc(local_id))
		{
			if(pkt.approximable)
			{

				err_thresh = rand()%100;
				//cout<<"interface buff size "<< interface_buf.size()<<endl;
				if(err_thresh < approx_ratio){
					pkt.size = 3;
					pkt.flit_left = 3;
				}

			}
		}
}

void NoximProcessingElement::end_of_simulation(){
	//cout<<"sent packets at "<<local_id<<" : "<<sent_packets<<endl;
	//cout<<"received packets at "<<local_id<<" : "<<received_packets<<endl;
}
