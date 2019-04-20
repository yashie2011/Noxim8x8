/*
 * constants.h
 *
 *  Created on: Jan 27, 2018
 *      Author: yaswanth
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_


#define NUM_CORES 56
#define MEM_CONTROLLERS 8
#define REPLY_DATA_SIZE 1024
#define FLIT_SIZE 64
#define MC_BUFF_CAP 256

// positions of each value in trace line
#define NUM_WORDS_IN_A_TRACE_LINE 12
#define TIME_STAMP_POS 9
#define SRC_ID_POS 3
#define DEST_ID_POS 11
#define COMPUTE_TIME_POS 12
#define WID_POS 5

#endif /* CONSTANTS_H_ */
