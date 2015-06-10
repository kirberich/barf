// This is a simple test sketch to try out the ESP8266 running the barf firmware
// while connected to an arduino. It passes everything from the serial console through to the wifi module and vice versa.

// The sketch assumes the ESP8266 connected to Serial1. I haven't tested it with SoftwareSerial, that'll probably require annoying code changes.
// That means this'll only work on an arduino with more than one hardware serial port (like a Mega or Due) or a Teensy.

#include <barf.h>

Barf barf(Serial1, String("Hagrid's older underpants"), String("o3jc7axp2"), 9600, true);

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	barf.init();
	barf.connect();

	while(!barf.is_connected()) {
		Serial.println("Waiting for wifi...");
		delay(100);
	}
}

void loop() {
	if (Serial1.available()) {
		char inByte = Serial1.read();
		Serial.print(inByte);
	}

	if (Serial.available()) {
		char inByte = Serial.read();
		Serial1.print(inByte);
	}
}
