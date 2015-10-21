#ifndef TRACKER_H_
#define TRACKER_H_

#include "vrpn/vrpn_Tracker.h"
#include <string>

class Tracker {
public:
	Tracker(const std::string &device);
	~Tracker();

	void getPos(double &x, double &y, double &z) const;
	void getQuat(double &x, double &y, double &z, double &w) const;
	void update();

	void handle_tracker(void *userdata, const vrpn_TRACKERCB t);

private:
	double _pos[3];
	double _quat[4];

	vrpn_Tracker_Remote *_tracker;
};

#endif
