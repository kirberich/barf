// This is a simple test sketch to try out the ESP8266 running the barf firmware
// while connected to an arduino. It passes everything from the serial console through to the wifi module and vice versa.

String ssid("ssisd here");
String password("password here");

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	Serial1.print("ssid ");
	Serial1.println(ssid);
	Serial1.print("password ");
	Serial1.println(password);
	Serial1.println("connect");
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
