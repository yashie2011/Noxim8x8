#include "token_issuer.h"


token_issuer::token_issuer(){

}

bool token_issuer::set_token(int mem_c, NoximCoord src_coord, int core_id){

	bool ret_flag = false;

	if (is_mc(src_coord))
		return true;

	//get slice of the enetwork
	int slice_id = 0;
	if(src_coord.x == 0 ){slice_id = 0;}  // column 0 choose slice 0
	if(src_coord.x == 1 ){slice_id = 1;}  // column 1 choose slice 1
	if(src_coord.x == 2 ){slice_id = 2;}  // column 2 choose slice 2
	if(src_coord.x == 3 ){slice_id = 3;}  // column 3 choose slice 3
	if(src_coord.x == 4 ){slice_id = 4;}  // column 4 choose slice 4
	if(src_coord.x == 5 ){slice_id = 5;}  // column 5 choose slice 5
	if(src_coord.x == 6 ){slice_id = 6;}  // column 6 choose slice 6
	if(src_coord.x == 7 ){slice_id = 7;}  // column 7 choose slice 7

	assert(mem_c < MEM_CONTROLLERS && slice_id < SLICES);

	mtx.lock();

	if(token_issuer::tokens[slice_id][mem_c] == -1){
			ret_flag = true;
			token_issuer::tokens[slice_id][mem_c] = core_id;
	}

	mtx.unlock();

	return ret_flag;



}

bool token_issuer::reset_token(int mem_c, NoximCoord src_coord){

	//get slice of the enetwork
	int slice_id = 0;
	if(src_coord.x == 0 ){slice_id = 0;}  // column 0 choose slice 0
	if(src_coord.x == 1 ){slice_id = 1;}  // column 1 choose slice 1
	if(src_coord.x == 2 ){slice_id = 2;}  // column 2 choose slice 2
	if(src_coord.x == 3 ){slice_id = 3;}  // column 3 choose slice 3
	if(src_coord.x == 4 ){slice_id = 4;}  // column 4 choose slice 4
	if(src_coord.x == 5 ){slice_id = 5;}  // column 5 choose slice 5
	if(src_coord.x == 6 ){slice_id = 6;}  // column 6 choose slice 6
	if(src_coord.x == 7 ){slice_id = 7;}  // column 7 choose slice 7

	assert(mem_c < MEM_CONTROLLERS && slice_id < SLICES);

	mtx.lock();

	token_issuer::tokens[slice_id][mem_c] = -1;

	mtx.unlock();

	return true;

}
