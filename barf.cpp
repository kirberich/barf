#include <barf.h>
#include <Arduino.h>

// new
void * operator new (size_t size) { return malloc (size); }
// placement new
void * operator new (size_t size, void * ptr) { return ptr; }
// delete
void operator delete (void * ptr) { free (ptr); }

#ifndef _getpid
extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
}
#endif

Barf::Barf(Stream &serial, jString ssid, jString password, int baud_rate, bool allow_gpio) : ser(serial) {
	this->ssid = ssid;
	this->password = password;
	this->baud_rate = baud_rate;
	this->allow_gpio = allow_gpio;
}

void Barf::send_command(jString command, jString value) {
	ser.print(command.c_str());
	ser.print(" ");
	ser.print(value.c_str());
	ser.print("\n");
}

void Barf::send_command(jString command) {
	ser.print(command.c_str());
	ser.print("\n");
}

void Barf::send_data(jString data) {
	ser.print(data.c_str());
	ser.print("\n");
}

void Barf::init() {
	// send_command(COMMAND_BAUD_RATE, baud_rate);
	// ser.begin(baud_rate);

	send_command(COMMAND_SSID, ssid);
	send_command(COMMAND_PASSWORD, password);
	send_command(allow_gpio ? COMMAND_ALLOW_GPIO : COMMAND_DISALLOW_GPIO);
}

void Barf::connect() {
	send_command(COMMAND_CONNECT);
}

void Barf::disconnect() {
	send_command(COMMAND_DISCONNECT);
}

bool Barf::is_connected() {
	send_command(COMMAND_IS_CONNECTED);
	jString response = read_line(COMMAND_IS_CONNECTED);
	return response == "1";
}

jString Barf::get_ip() {
	send_command(COMMAND_GET_IP);
	return read_line(COMMAND_GET_IP);
}

void Barf::set_led_mode(int mode) {
	char cmode[2];
	itoa(mode, cmode, 10);
	send_command(COMMAND_LED_MODE, jString(cmode));
}

jString Barf::debug_info() {
	return jString(ser.readString().c_str());
}

jString Barf::read_line(jString expected_command, unsigned long timeout) {
	unsigned long begin = millis();

	while(!ser.available()) {
		if (millis() - begin > timeout) {
			return TIMEOUT;
		}
	}

	jString line;

	char c;
	while (true) {
		if (ser.available()) {
			c = ser.read();

			if (c == '\n') {
				break;
			}

			line.push_back(c);
		}

		if (millis() - begin > timeout) {
			return TIMEOUT;
		}
	}
	if(!expected_command.length()) {
		return line;
	}

	if (line.substr(0, expected_command.length()) != expected_command) {
		return UNEXPECTED_COMMAND;
	}

	return line.substr(expected_command.length()+1);
}

jString Barf::read_line(jString expected_command) {
	return read_line(expected_command, 10000);
}

jString Barf::read_line(unsigned long timeout) {
	return read_line("", timeout);
}

jString Barf::read_line() {
	return read_line("", 10000);
}

jString Barf::get_or_post(jString command, jString url) {
	send_command(command, url);

	// We can't guarantee that the first line that comes back will be the response
	// so we wait for RESPONSE_START and discard everything up to that point
	bool response_has_started = false;

	jString response = "";
	bool headers_finished = false;
	while (true) {
		jString line = read_line();

		if (line == COMMAND_RESPONSE_START) {
			response_has_started = true;
		} else if (line == TIMEOUT) {
			return line;
		} else if (!response_has_started) {
			continue;
		} else if (line == COMMAND_RESPONSE_END) {
			break;
		} else {
			if (headers_finished) {
				response = response + line;
			}
			if(!line.length()) {
				headers_finished = true;
			}
		}
	}

	return response;
}

jString Barf::get(jString url) {
	return get_or_post(COMMAND_GET, url);
}

jString Barf::post(jString url) {
	return get_or_post(COMMAND_POST, url);
}

void Barf::get_command_value(jString &command, jString &value) {
	jString line = read_line();
	int space_index = line.find(" ");

	if (space_index != -1) {
		command = line.substr(0, space_index);
		value = line.substr(space_index + 1);
	} else {
		command = line;
		value = "";
	}
}

Request Barf::run() {
	// Handle requests incoming over wifi
	Request request;
	jString command;
	jString value;

	if (ser.available()) {
		get_command_value(command, value);

		if (command == COMMAND_METHOD) {
			// Begins a new request
			request.method = value;

			while (command != COMMAND_REQUEST_RESPONSE) {
				get_command_value(command, value);

				if (command == COMMAND_PATH_FRAGMENT) {
					request.fragments.push_back(value);
				} else if (command == COMMAND_GET_VAR) {
					RequestVar var({value, read_line(COMMAND_GET_VALUE)});
					request.get_vars.push_back(var);
				} else if (command == TIMEOUT) {
					// Give up waiting for the respond command if reading the line times out
					break;
				}
			}
		}
	}

	return request;
}
