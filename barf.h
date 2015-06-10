#include <Stream.h>
#include "constants.h"

class Barf {
public:
	Barf(Stream &serial, String ssid, String password, int baud_rate, bool allow_gpio);

	void init();
	void connect();
	void disconnect();
	bool is_connected();
	void set_led_mode(int mode);
	String debug_info();
	void send_command(String command, String value);
	void send_command(String command);
	String get_ip();
	String get_response_line(String expected_command, unsigned long timeout);
	String get_response_line(String expected_command);
	String get_response_line(unsigned long timeout);
	String get_response_line();

	String get_or_post(String command, String url);
	String get(String url);
	String post(String url);

	void run();

private:
	String ssid;
	String password;
	int baud_rate;
	int led_mode;
	bool allow_gpio;
	Stream &ser;
};
