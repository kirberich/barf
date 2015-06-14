// Simple example showing how to run barf as a server and responding to incoming requests.

// The sketch assumes the ESP8266 connected to Serial1. I haven't tested it with SoftwareSerial, that'll probably require annoying code changes.
// Tested on a teensy, on an arduino you should only need to change the definition of BARF_SERIAL to "Serial"
#include <barf.h>

#define CONSOLE_SERIAL Serial
#define BARF_SERIAL Serial1

Barf barf(BARF_SERIAL, "ssid", "password", 9600, true);
Request request;

String to_arduino_string(jString s) {
	return String(s.c_str());
}

void setup() {
	pinMode(13, OUTPUT);
	CONSOLE_SERIAL.begin(9600);
	BARF_SERIAL.begin(9600);
	delay(1000);

	barf.init();
	barf.connect();

	while(!barf.is_connected()) {
		CONSOLE_SERIAL.println("Waiting for wifi...");
		delay(100);
	}
	CONSOLE_SERIAL.print("connected!\nip: ");
	CONSOLE_SERIAL.println(to_arduino_string(barf.get_ip()));

	digitalWrite(13, HIGH);
}

void loop() {
	request = barf.run();
	if (!request.is_null()) {
		CONSOLE_SERIAL.println(String("method: ") + to_arduino_string(request.method));

		CONSOLE_SERIAL.print("fragments ");
		CONSOLE_SERIAL.println(request.fragments.size());
		for(int i=0; i<request.fragments.size(); i++) {
			CONSOLE_SERIAL.println(to_arduino_string(request.fragments[i]));
		}

		for(int i=0; i<request.get_vars.size(); i++) {
			CONSOLE_SERIAL.println(to_arduino_string(request.get_vars[i].name) + String(" = ") + to_arduino_string(request.get_vars[i].value));
		}
	}
}
