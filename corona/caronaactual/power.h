#ifndef __POWER_H__
#define __POWER_H__

class TPower
{

 public:

  TPower();

  void Routing();
  void Selection();
  void Standby();
  void Forward();
  void Incoming();

  double getPower() { return pwr; }

  double getPwrRouting() { return pwr_routing; }
  double getPwrSelection() { return pwr_selection; }
  double getPwrForward() { return pwr_forward; }
  double getPwrStandBy() { return pwr_standby; }
  double getPwrIncoming() { return pwr_incoming; }

 private:

  double pwr_routing;
  double pwr_selection;
  double pwr_forward;
  double pwr_standby;
  double pwr_incoming;

  double pwr;
};

// ---------------------------------------------------------------------------

#endif
