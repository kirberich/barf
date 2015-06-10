#include <barf.h>
#include <Arduino.h>

Barf::Barf(Stream &serial, String ssid, String password, int baud_rate, bool allow_gpio) : ser(serial) {
	this->ssid = ssid;
	this->password = password;
	this->baud_rate = baud_rate;
	this->allow_gpio = allow_gpio;
}

void Barf::send_command(String command, String value) {
	ser.print(command + String(" ") + value + String("\n"));
}

void Barf::send_command(String command) {
	ser.print(command + String("\n"));
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
	String response = get_response_line(COMMAND_IS_CONNECTED);
	return response == "1";
}

String Barf::get_ip() {
	send_command(COMMAND_GET_IP);
	return get_response_line(COMMAND_GET_IP);
}

void Barf::set_led_mode(int mode) {
	send_command(COMMAND_LED_MODE, String(mode));
}

String Barf::debug_info() {
	return ser.readString();
}

String Barf::get_response_line(String expected_command, unsigned long timeout) {
	unsigned long begin = millis();

	while(!ser.available()) {
		if (millis() - begin > timeout) {
			return TIMEOUT;
		}
	}

	String line = ser.readStringUntil('\n');
	line.trim();
	if(!expected_command.length()) {
		return line;
	}

	if (line.substring(0, expected_command.length()) != expected_command) {
		return UNEXPECTED_COMMAND;
	}

	return line.substring(expected_command.length() + 1);
}

String Barf::get_response_line(String expected_command) {
	return get_response_line(expected_command, 10000);
}

String Barf::get_response_line(unsigned long timeout) {
	return get_response_line("", timeout);
}

String Barf::get_response_line() {
	return get_response_line("", 10000);
}

String Barf::get_or_post(String command, String url) {
	send_command(command, url);

	// We can't guarantee that the first line that comes back will be the response
	// so we wait for RESPONSE_START and discard everything up to that point
	bool response_has_started = false;

	String response = "";
	bool headers_finished = false;
	while (true) {
		String line = get_response_line();

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
				response += line;
			}
			if(!line.length()) {
				headers_finished = true;
			}
		}
	}

	return response;
}

String Barf::get(String url) {
	return get_or_post(COMMAND_GET, url);
}

String Barf::post(String url) {
	return get_or_post(COMMAND_POST, url);
}

void Barf::run() {}
