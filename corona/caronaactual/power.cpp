#include <cassert>
#include "GlobalTypeDefs.h"
#include "power.h"


using namespace std;

TPower::TPower()
{
  pwr = 0.0;
  pwr_standby  = PWR_STANDBY;
  pwr_forward  = PWR_FORWARD_FLIT;
  pwr_incoming = PWR_INCOMING;
}

// ---------------------------------------------------------------------------

void TPower::Routing()
{
  pwr += pwr_routing;
}

// ---------------------------------------------------------------------------

void TPower::Selection()
{
  pwr += pwr_selection;
}

// ---------------------------------------------------------------------------

void TPower::Standby()
{
  pwr += pwr_standby;
}

// ---------------------------------------------------------------------------

void TPower::Forward()
{
  pwr += pwr_forward;
}

// ---------------------------------------------------------------------------

void TPower::Incoming()
{
  pwr += pwr_incoming;
}

// ---------------------------------------------------------------------------



