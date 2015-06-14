// This is a simple test sketch to try out the ESP8266 running the barf firmware
// while connected to an arduino. It passes everything from the serial console through to the wifi module and vice versa.

// The sketch assumes the ESP8266 connected to Serial1. I haven't tested it with SoftwareSerial, that'll probably require annoying code changes.
// That means this'll only work on an arduino with more than one hardware serial port (like a Mega or Due) or a Teensy.

#include <barf.h>

#define CONSOLE_SERIAL Serial
#define BARF_SERIAL Serial1

Barf barf(BARF_SERIAL, "ssid", "password", 9600, true);

void setup() {
	CONSOLE_SERIAL.begin(9600);
	BARF_SERIAL.begin(9600);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	barf.init();
	barf.connect();

	while(!barf.is_connected()) {
		CONSOLE_SERIAL.println("Waiting for wifi...");
		delay(100);
	}
	CONSOLE_SERIAL.println("connected!");
	CONSOLE_SERIAL.println(barf.get_ip() );
}

void loop() {
	if (BARF_SERIAL.available()) {
		char inByte = BARF_SERIAL.read();
		CONSOLE_SERIAL.print(inByte);
	}

	if (CONSOLE_SERIAL.available()) {
		char inByte = CONSOLE_SERIAL.read();
		BARF_SERIAL.print(inByte);
	}
}
