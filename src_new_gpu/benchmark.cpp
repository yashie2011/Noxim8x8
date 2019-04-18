#include "benchmark.h"

void benchmark::packet_injection_nodeindex()
{
	if(reset.read())
  {
    
  }
  else
  {
	  comm_req.clear();
	  //cout<<"The benchmark cycle sensitive module in action "<<endl;
	  int lineNumber_req=v_req.size();
	  if(lineNumber_req-1 > trace_index)    // This block needs a lot of edits
	  {
	
		double _current_cycle= sc_time_stamp().to_double()/1000;
		double cc= _current_cycle - (DEFAULT_RESET_TIME + NoximGlobalParams::stats_warm_up_time);


		std::string stringitem[NUM_WORDS_IN_A_TRACE_LINE];   // chnage No. of words in a line

		int i=0;
		while(i < ( MEM_CONTROLLERS ))    // This is changes when chip size increased to 8X8
		{
			std:: string str = v_req[trace_index];
			//cout<<"assinging the trace to a str obj"<<endl;
			std::istringstream traceline(str);
			//cout<<"assinging the str obj to a strstream obj"<<endl;
			int word_index = 0;
			while(std::getline(traceline, str, ' ') && word_index < NUM_WORDS_IN_A_TRACE_LINE)
			{
				stringitem[word_index] = str;
				word_index++;
			}
			double temp =  double(atoi(stringitem[TIME_STAMP_POS].c_str()));
			//cout<<"Trace collecter cycle for node" << atoi(stringitem[SRC_ID_POS].c_str()) <<"is: "<< temp<<endl<<endl;
			//cout<<" word count is "<<word_no;
			//cout<<" Printing the last word "<< stringitem[9]<<endl;
			if(temp == cc)
			{
				//cout<<"node: "<< nodeid;
				//cout<< " matching clock cycle "<<cc <<", pushing packet into request queue "<<endl;
				const char* dest_id = stringitem[DEST_ID_POS].c_str();
				//cout<<"attempting to print computation time "<<endl;
				//cout << "Printing computation time "<< stringitem[9].c_str()<<endl;
				const char* comp_time = stringitem[COMPUTE_TIME_POS].c_str();
				trace_index++;
				i++;

				comm req_node ;
				int mem_con = 0;
				mem_con = atoi(dest_id);
				// Added a stupid hack to change the dest_id of gpgpu MCs as per Noxim
				switch(mem_con){
				case 0:
					req_node.dest_id = m1;
					break;
				case 1:
					req_node.dest_id = m2;
					break;
				case 2:
					req_node.dest_id = m3;
					break;
				case 3:
					req_node.dest_id = m4;
					break;
				case 4:
					req_node.dest_id = m5;
					break;
				case 5:
					req_node.dest_id = m6;
					break;
				case 6:
					req_node.dest_id = m7;
					break;
				case 7:
					req_node.dest_id = m8;
					break;

				default:
					req_node.dest_id = m1;
					break;
				}

				req_node.data_size = REPLY_DATA_SIZE;
				req_node.status = true;
				req_node.comp_time = atoi(comp_time);
				comm_req.push_back(req_node);

			}
			else
			{
				i= MEM_CONTROLLERS + 1;
			}
	}

	}
}

//for(int i=0; i < node_id.size(); i++)
	//{
		//cout<< sc_time_stamp().to_double()/1000<<" : node id"<< node_id[i]<<endl;
	//}
}



//This function provides response to the processing element, whether to inject traffic or not 
bool benchmark::packet_injection_response_req(int nodeid_request, std::vector<comm> &ret_arr )
{
	// Just iterate over the list and send
	if(comm_req.size() >0){
		int len = comm_req.size();
		for (int it = 0; it <len; it++){
			ret_arr.push_back(comm_req[it]);
		}
		return true;
	}
	else{
		return false;
	}

}



void benchmark::clear_vector()
{
	//v.clear();
	
}

 



 
	
	
		
	
