#ifndef __GLOBAL_TYPE_DEFS_H__
#define __GLOBAL_TYPE_DEFS_H__
#include <cassert>
#include <systemc.h>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

using namespace std;

// ---------------------------------------------------------------------------
/*
The average energy dissipated by a flit for a hop switch was estimated
as being 0.151nJ, 0.178nJ, 0.182nJ and 0.189nJ for XY, Odd-Even, DyAD,
and NoP-OE respectively

We assumed the tile size to be 2mm x 2mm and that the tiles were
arranged in a regular fashion on the floorplan. The load wire
capacitance was set to 0.50fF per micron, so considering an average of
25% switching activity the amount of energy consumed by a flit for a
hop interconnect is 0.384nJ.
*/

#define PWR_ROUTING_XY             0.151e-9
#define PWR_SEL_RANDOM             0.002e-9
#define PWR_SEL_BUFFER_LEVEL       0.006e-9
#define PWR_SEL_NOP                0.012e-9

#define PWR_FORWARD_FLIT           0.384e-9
#define PWR_INCOMING               0.002e-9
#define PWR_STANDBY                0.0001e-9/2.0

#define PWR_ROUTING_XYZ            0.151e-9

//#define DEFAULT_RESET_TIME 1000

// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Define the directions as numbers
#define DIRECTIONS             4
#define GIR_DIRECTIONS         5
#define SAFE_TO_SEND           1
#define UNSAFE_TO_SEND         0
#define WDM_BUFFER_NUM         6
#define WDM_GROUP_ZERO         8
#define WDM_GROUP_ONE          16
#define WDM_GROUP_TWO          24
#define WDM_GROUP_THREE        32
#define MAX_GI                 4
#define PRI                    4
#define MWMR_SIZE              4
#define SWMR_SIZE              3


// WDM DIRECTIONS


// Routing algorithms
#define ROUTING_XY             0

// Selection strategies
#define SEL_RANDOM             0
#define SEL_BUFFER_LEVEL       1
#define SEL_NOP                2

// Verbosity levels
#define VERBOSE_OFF            0
#define VERBOSE_LOW            1
#define VERBOSE_MEDIUM         2
#define VERBOSE_HIGH           3

// Traffic distribution
#define TRAFFIC_RANDOM         0
#define TRAFFIC_TRANSPOSE1     1
#define TRAFFIC_TRANSPOSE2     2
#define TRAFFIC_HOTSPOT        3
#define TRAFFIC_TABLE_BASED    4
#define TRAFFIC_BIT_REVERSAL   5
#define TRAFFIC_SHUFFLE        6
#define TRAFFIC_BUTTERFLY      7

// Topology
#define MESH_2D                0




//---------------------------------------------------------------------------

// Default configuration can be overridden with command-line arguments
#define DEFAULT_VERBOSE_MODE               VERBOSE_OFF
#define DEFAULT_TRACE_MODE                       false
#define DEFAULT_TRACE_FILENAME                      ""
#define DEFAULT_MESH_DIM_X                           8
#define DEFAULT_MESH_DIM_Y                           8
#define DEFAULT_BUFFER_DEPTH                         5
#define PHOTONIC_BUFFER_DEPTH                       10
#define DEFAULT_MAX_PACKET_SIZE                     10
#define DEFAULT_MIN_PACKET_SIZE                     2
#define DEFAULT_ROUTING_ALGORITHM           ROUTING_XY
#define DEFAULT_ROUTING_TABLE_FILENAME              ""
#define DEFAULT_SELECTION_STRATEGY          SEL_RANDOM
#define DEFAULT_PACKET_INJECTION_RATE             0.07
#define DEFAULT_PROBABILITY_OF_RETRANSMISSION      .01
#define DEFAULT_TRAFFIC_DISTRIBUTION    TRAFFIC_SHUFFLE
#define DEFAULT_TRAFFIC_TABLE_FILENAME              ""
#define DEFAULT_RESET_TIME                        1000
#define DEFAULT_SIMULATION_TIME                  10000
#define DEFAULT_STATS_WARM_UP_TIME  DEFAULT_RESET_TIME
#define DEFAULT_DETAILED                         false
#define DEFAULT_DYAD_THRESHOLD                     0.6
#define DEFAULT_MAX_VOLUME_TO_BE_DRAINED             0
#define DEFAULT_PHOTONIC_NOC                         1

#define MAX_STATIC_DIM 20
#define COLUMN_AOC 22

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TGlobalParams -- used to forward configuration to every sub-block
struct TGlobalParams
{
  static int verbose_mode;
  static int trace_mode;
  static char trace_filename[128];
  static int topology;
  static int mesh_dim_x;
  static int mesh_dim_y;
  static int buffer_depth;
  static int photonic_buffer_depth;
  static int min_packet_size;
  static int max_packet_size;
  static int routing_algorithm;
  static char routing_table_filename[128];
  static int selection_strategy;
  static float packet_injection_rate;
  static float probability_of_retransmission;
  static int traffic_distribution;
  static char traffic_table_filename[128];
  static int simulation_time;
  static int stats_warm_up_time;
  static int rnd_generator_seed;
  static bool detailed;
  static int photonic_noc;
  static vector<pair<int,double> > hotspots;
  static float dyad_threshold;
  static unsigned int max_volume_to_be_drained;
};

//---------------------------------------------------------------------------
// TFlitType -- Flit type enumeration
enum TFlitType
{
  FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL
};
//---------------------------------------------------------------------------
// TPayload -- Payload definition
struct TPayload
{
  sc_uint<32>        data;         // Bus for the data to be exchanged

  inline bool operator == (const TPayload& payload) const
  {
    return (payload.data==data);
  }
};

//---------------------------------------------------------------------------
// TFlit -- Flit definition
struct TFlit
{
  int                src_id;
  int                dst_id;
  int                nearest_gateway_id; // The id of the nearest gateway router
  bool               inter_pri;          // For the inter PRI communication
  TFlitType          flit_type;          // The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
  int                sequence_no;        // The sequence number of the flit inside the packet
  TPayload           payload;            // Optional payload
  double             timestamp;          // Unix timestamp at packet generation
  int                hop_no;             // Current number of hops from source to destination
  int                wdm;                // Indicates the WDM group,  the flit belongs to 
  

  inline bool operator == (const TFlit& flit) const
  {
    return (flit.src_id==src_id && flit.dst_id==dst_id && flit.nearest_gateway_id==nearest_gateway_id
    && flit.inter_pri==inter_pri && flit.flit_type==flit_type && flit.sequence_no==sequence_no
    && flit.payload==payload && flit.timestamp==timestamp && flit.hop_no==hop_no && flit.wdm == wdm);
  }

};

//---------------------------------------------------------------------------
struct TChannelStatus
{
    int free_slots;  // occupied buffer slots
    bool available; // 
    inline bool operator == (const TChannelStatus& bs) const
    {
	return (free_slots == bs.free_slots && available == bs.available);
    };
};
//---------------------------------------------------------------------------
// TNoP_data -- NoP Data definition

struct TNoP_data
{
    int sender_id;
    TChannelStatus channel_status_neighbor[DIRECTIONS]; 

    inline bool operator == (const TNoP_data& nop_data) const
    {
	return ( sender_id==nop_data.sender_id  &&
		nop_data.channel_status_neighbor[0]==channel_status_neighbor[0] &&
		nop_data.channel_status_neighbor[1]==channel_status_neighbor[1] &&
		nop_data.channel_status_neighbor[2]==channel_status_neighbor[2] &&
		nop_data.channel_status_neighbor[3]==channel_status_neighbor[3]);
    };
};

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TCoord -- XY coordinates type of the Tile inside the Mesh
class TCoord
{
 public:
  int                x;            // X coordinate
  int                y;            // Y coordinate

  inline bool operator == (const TCoord& coord) const
  {
    return (coord.x==x && coord.y==y);
  }
};


// misc common functions **************************************

//---------------------------------------------------------------------------
inline TCoord id2Coord(int id) 
{
  TCoord coord;

  coord.x = id % TGlobalParams::mesh_dim_x;
  coord.y = id / TGlobalParams::mesh_dim_x;

  assert(coord.x < TGlobalParams::mesh_dim_x);
  assert(coord.y < TGlobalParams::mesh_dim_y);

  return coord;
}


//---------------------------------------------------------------------------
inline int coord2Id(const TCoord& coord) 
{
  int id = (coord.y * TGlobalParams::mesh_dim_x) + coord.x;

  assert(id < TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y);

  return id;
}

//---------------------------------------------------------------------------
inline TCoord newid2Coord(int newid) 
{
  TCoord coord;
  int tempx, tempy;

  tempy = newid % TGlobalParams::mesh_dim_x;
  tempx = newid / TGlobalParams::mesh_dim_x;
  if(tempx % 2 ==0)
  {
	  coord.x = tempx;
	  coord.y = tempy;
  }
  else
  {
	  coord.x = tempx;
	  coord.y = (TGlobalParams::mesh_dim_x - 1) - tempy;
  }

  assert(coord.x < TGlobalParams::mesh_dim_x);
  assert(coord.y < TGlobalParams::mesh_dim_y);

  return coord;
}

//---------------------------------------------------------------------------
inline int coord2newId(const TCoord& coord) 
{
  int newid;
  if (coord.x % 2 ==0)
  {
	  newid = (coord.x * TGlobalParams::mesh_dim_x) + coord.y;
  }
  else 
  {
	  newid = ((coord.x+1) * TGlobalParams::mesh_dim_x) - (coord.y+1);
  }

  assert(newid < TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y);

  return newid;
}

//----------------------------------------------------------------
inline ostream& operator << (ostream& os, const TFlit& flit)
{

  if (TGlobalParams::verbose_mode > VERBOSE_HIGH)
  {

      os << "### FLIT ###" << endl;
      os << "Source Tile[" << flit.src_id << "]" << endl;
      os << "Destination Tile[" << flit.dst_id << "]" << endl;
      switch(flit.flit_type)
      {
	case FLIT_TYPE_HEAD: os << "Flit Type is HEAD" << endl; break;
	case FLIT_TYPE_BODY: os << "Flit Type is BODY" << endl; break;
	case FLIT_TYPE_TAIL: os << "Flit Type is TAIL" << endl; break;
      }
      os << "Sequence no. " << flit.sequence_no << endl;
      os << "Payload printing not implemented (yet)." << endl;
      os << "Unix timestamp at packet generation " << flit.timestamp << endl;
      os << "Total number of hops from source to destination is " << flit.hop_no << endl;
  }
  else
    {
      os << "[type: ";
      switch(flit.flit_type)
      {
	case FLIT_TYPE_HEAD: os << "H"; break;
	case FLIT_TYPE_BODY: os << "B"; break;
	case FLIT_TYPE_TAIL: os << "T"; break;
      }
      
      os << ", seq: " << flit.sequence_no << ", " << flit.src_id << "-->" << flit.dst_id << "]"; 
    }

  return os;
}
//---------------------------------------------------------------------------

inline ostream& operator << (ostream& os, const TChannelStatus& status)
{
  char msg;
  if (status.available) msg = 'A'; 
  else
      msg = 'N';
  os << msg << "(" << status.free_slots << ")"; 
  return os;
}
//---------------------------------------------------------------------------

inline ostream& operator << (ostream& os, const TNoP_data& NoP_data)
{
  os << "      NoP data from [" << NoP_data.sender_id << "] [ ";

  for (int j=0; j<DIRECTIONS; j++)
      os << NoP_data.channel_status_neighbor[j] << " ";

  cout << "]" << endl;
  return os;
}


//---------------------------------------------------------------------------

inline ostream& operator << (ostream& os, const TCoord& coord)
{
  os << "(" << coord.x << "," << coord.y << ")";

  return os;
}


// trace redefinitions *******************************************
//
//---------------------------------------------------------------------------
inline void sc_trace(sc_trace_file*& tf, const TFlit& flit, string& name)
{
  sc_trace(tf, flit.src_id, name+".src_id");
  sc_trace(tf, flit.dst_id, name+".dst_id");
  sc_trace(tf, flit.sequence_no, name+".sequence_no");
  sc_trace(tf, flit.timestamp, name+".timestamp");
  sc_trace(tf, flit.hop_no, name+".hop_no");
}
//---------------------------------------------------------------------------

inline void sc_trace(sc_trace_file*& tf, const TNoP_data& NoP_data, string& name)
{
  sc_trace(tf, NoP_data.sender_id, name+".sender_id");
}

//---------------------------------------------------------------------------
inline void sc_trace(sc_trace_file*& tf, const TChannelStatus& bs, string& name)
{
  sc_trace(tf, bs.free_slots, name+".free_slots");
  sc_trace(tf, bs.available, name+".available");
}

// TPacket -- Packet definition
struct TPacket
{
  int                src_id;
  int                dst_id;
  double             timestamp;    // SC timestamp at packet generation
  int                size;
  int                flit_left;    // Number of remaining flits inside the packet

  TPacket() {;}
  TPacket(const int s, const int d, const double ts, const int sz) {
    make(s, d, ts, sz);
  }

  void make(const int s, const int d, const double ts, const int sz) {
    src_id = s; dst_id = d; timestamp = ts; size = sz; flit_left = sz;
  }
};

//---------------------------------------------------------------------------
// TRouteData -- data required to perform routing
struct TRouteData
{
    int  current_id;
    int  src_id;
    int  dst_id;
    int  dir_in;             // direction from which the packet comes from
    int  nearest_gateway_id; // id of the nearest gateway interface
    bool inter_pri;          // if set, packet is sent via photonic layer

};


//---------------------------------------------------------------------------
struct CommHistory
{
  int            src_id;
  vector<double> delays;
  unsigned int   total_received_flits;
  double         last_received_flit_time;

};

//---------------------------------------------------------------------------

typedef pair<int,int> TLinkId; // source, destination node

// Routing table
typedef set<TLinkId> TAdmissibleOutputs;

// Map a destination to a set of admissible outputs
typedef map<int,TAdmissibleOutputs> TRoutingTableLink; 

// Map an input link to its routing table
typedef map<TLinkId,TRoutingTableLink> TRoutingTableNode;

// Map a node of the network to its routing table
typedef map<int,TRoutingTableNode> TRoutingTableNoC;

//---------------------------------------------------------------------------

// Converts an input direction to a link 
TLinkId direction2ILinkId(const int node_id, const int dir);

// Converts an input direction to a link
int oLinkId2Direction(const TLinkId& out_link);

// Converts a set of output links to a set of directions
vector<int> admissibleOutputsSet2Vector(const TAdmissibleOutputs& ao);

#endif
