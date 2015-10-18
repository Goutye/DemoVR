#ifndef EBONY_GAMEPAD_H_
#define EBONY_GAMEPAD_H_

#include <vector>
#include "vrpn/vrpn_XInputGamepad.h"
#include "vrpn/vrpn_Analog.h"
#include "vrpn/vrpn_Button.h"

class Gamepad {
public:
	static void VRPN_CALLBACK handle_analog_stub(void* userData, const vrpn_ANALOGCB a);
	static void VRPN_CALLBACK handle_button_stub(void* userData, const vrpn_BUTTONCB a);
	static void VRPN_CALLBACK handle_button_states_stub(void* userData, const vrpn_BUTTONSTATESCB a);
	Gamepad(char *name);
	~Gamepad();
	void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a);
	void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b);
	void VRPN_CALLBACK handle_button_states(void *userdata, const vrpn_BUTTONSTATESCB b);
	void update();
	void getAnalogL(float &x, float &y);
	void getAnalogR(float &x, float &y);

private:
	char *_name;

	vrpn_Button_Remote *buttons;
	vrpn_Analog_Remote *analogs;

	float analogL[2];
	float analogR[2];
};

#endif