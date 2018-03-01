#include "benchmark.h"

void benchmark::packet_injection_nodeindex()
{
	if(reset.read())
  {
    
  }
  else
  {
	  node_id_req.clear(); // clearing previous cycle node entries
	  comm_req.clear();
	  //cout<<"The benchmark cycle sensitive module in action "<<endl;
	  int lineNumber_req=v_req.size();
	  if(lineNumber_req-1 > count_req)    // This block needs a lot of edits
	  {
	
		double _current_cycle= sc_time_stamp().to_double()/1000;
		double cc= _current_cycle-1000;
		//cout<< "current time: "<< cc << endl;
		std::string stringitem[10];   // chnage No. of words in a line
		int i=0;

		while(i<12)    // This is changes when chip size increased to 8X8
		{
		std:: string str = v_req[count_req];
		std::istringstream myline(str);
		int word_no = 0;
		while(std::getline(myline, str, ' '))
		{
			stringitem[word_no] = str;
			word_no++;
		}
		double temp =  double(atof(stringitem[8].c_str()));
		//cout<<"Request trace collecter cycle is: "<< str;
		//cout<<" word count is "<<word_no;
		//cout<<" Printing the last word "<< stringitem[9]<<endl;
		if(temp == cc)
		{
			//cout<<" node: "<< nodeid;
			//cout<< " matching clock cycle "<<cc <<" in request trace, pushing flit into request queue "<<endl;
			const char* temp2 = stringitem[1].c_str();
			const char* dest_id = stringitem[0].c_str();
			string data_size_temp = stringitem[5].substr(5);
			const char* sdata= data_size_temp.c_str();   // doubtful of a crash
			int temp_nid = atoi(temp2);
			//cout<<"attempting to print computation time "<<endl;
			//cout << "Printing computation time "<< stringitem[9].c_str()<<endl;
			const char* comp_time = stringitem[9].c_str();
			node_id_req.push_back(temp_nid);      // Pushing ids of nodes that inject packets in the current cycle
			count_req++;
			i++;

			comm req_node ;
			int mem_con = 0;
			mem_con = atoi(dest_id);
			// Added a stupid hack to change the dest_id of gpgpu MCs as per Noxim
			switch(mem_con){
			case 0:
				req_node.dest_id = 2;
				break;
			case 1:
				req_node.dest_id = 7;
				break;
			case 2:
				req_node.dest_id = 8;
				break;
			case 3:
				req_node.dest_id = 13;
				break;
			default:
				req_node.dest_id = 2;
				break;
			}

			req_node.data_size = atoi(sdata);
			req_node.status = true;
			req_node.comp_time = atoi(comp_time);
			comm_req.push_back(req_node);

		}
		else
		{
			i=17;
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
	//cout<<" node: "<< nodeid;
	//cout<<" request received from node req "<< nodeid_request;
	// Adapting to the ID numbers of the gpgpusim    -- needs change when expanding
	if(nodeid_request > 2 && nodeid_request < 7 )
		nodeid_request--;
	if(nodeid_request > 8 && nodeid_request < 13 )
		nodeid_request = nodeid_request-3;
	if(nodeid_request > 13 )
		nodeid_request = nodeid_request -4;

	//cout<<" node: "<< nodeid;
	//cout<<" processed node req to "<< nodeid_request <<endl;
	//cout<<"printing contents of the queue ";
	//for( std::vector<int>::const_iterator i = node_id_req.begin(); i != node_id_req.end(); ++i)
	  //  cout << *i << ' ';
	//cout<<endl;

	// Just iterate over the list and send
	if(node_id_req.size() >0){
		int len = node_id_req.size();
		for (int it = 0; it <len; it++){
			ret_arr.push_back(comm_req[it]);
		}
		return true;
	}
	/*const vector<int>::const_iterator pos = std::find( node_id_req.begin()
	                                                     , node_id_req.end()
	                                                     , nodeid_request );*/
	/*if(  node_id_req.end() ) {
		//cout<<" Processing element request at simulator cycle: "<<(sc_time_stamp().to_double() / 1000)  <<" Req network success ";
	    const vector<int>::size_type idx = pos-node_id_req.begin();
	    comm return_val = comm_req[idx];
	    //cout<<"Returning values "<<return_val.dest_id<<" "<<return_val.data_size<<" "<<return_val.status<<endl;
	    return return_val;
	} */
	else{
		return false;
	}

}



void benchmark::clear_vector()
{
	//v.clear();
	
}

 



 
	
	
		
	
