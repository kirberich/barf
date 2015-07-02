#pragma once
#ifndef nullptr
#define nullptr NULL
#endif

#include <Stream.h>
#include "jsonic/containers.h"
#include "constants.h"

void * operator new (size_t size);
// placement new
void * operator new (size_t size, void * ptr);
// delete
void operator delete (void * ptr);

typedef jsonic::containers::String jString;

struct RequestVar {
	jString name;
	jString value;
};

struct Request {
	jString method;
	jsonic::containers::Vector<jString> fragments;
	jsonic::containers::Vector<RequestVar> get_vars;

	Request() {
		fragments.reserve(100);
		get_vars.reserve(100);
	}

	bool is_null() {
		return method.length() == 0;
	}
};

class Barf {
public:
	Barf(Stream &serial, jString ssid, jString password, int baud_rate, bool allow_gpio);

	void init();
	void connect();
	void disconnect();
	bool is_connected();
	void set_led_mode(int mode);
	jString debug_info();
	void send_command(jString command, jString value);
	void send_command(jString command);
	void send_data(jString data);
	void get_command_value(jString &command, jString &value);
	jString get_ip();
	jString read_line(jString expected_command, unsigned long timeout);
	jString read_line(jString expected_command);
	jString read_line(unsigned long timeout);
	jString read_line();

	jString get_or_post(jString command, jString url);
	jString get(jString url);
	jString post(jString url);


	Request run();

private:
	jString ssid;
	jString password;
	int baud_rate;
	int led_mode;
	bool allow_gpio;
	Stream &ser;
};
