#include "Tracker.h"

#include <algorithm>

using namespace std;

namespace {
	void VRPN_CALLBACK handle_tracker_stub(void *userdata, const vrpn_TRACKERCB t)
	{
		Tracker *tracker = static_cast<Tracker*>(userdata);
		tracker->handle_tracker(nullptr, t);
	}
}

Tracker::Tracker(const string &device)
{
	_tracker = new vrpn_Tracker_Remote(device.c_str());
	_tracker->register_change_handler(this, handle_tracker_stub);
}

Tracker::~Tracker()
{
	delete _tracker;
}

void Tracker::getPos(double &x, double &y, double &z) const
{
	x = _pos[0];
	y = _pos[1];
	z = _pos[2];
}

void Tracker::getQuat(double &x, double &y, double &z, double &w) const
{
	x = _quat[0];
	y = _quat[1];
	z = _quat[2];
	w = _quat[3];
}

void Tracker::update()
{
	_tracker->mainloop();
}

void Tracker::handle_tracker(void *userdata, const vrpn_TRACKERCB t)
{
	copy_n(t.pos, 3, _pos);
	copy_n(t.quat, 4, _quat);
}
