# Barmy Arduino REST Framework

A simple ESP8266-arduino based firmware with accompanying arduino-compatible library to allow easy use of an ESP8266 wifi chip. There are other libraries out there that allow similar functionality without having to reflash the ESP8266, but I like being able to easily add functionality to the wifi module directly.

It isn't really a framework, it doesn't enforce REST-style resources or support any verbs except GET and POST and all of the code is either not arduino-specific or runs on a dedicated wifi-chip, not an arduino. Hence the barmy.

This has been hacked together in a day; do not expect pretty. **I repeat, this is some rough code.**

## The most important bits
The ESP8266 is **NOT** 5V compatible. Everything here applies to 3V logic. Use a 3V FTDI cable, and a 3V arduino. Otherwise, use level shifters and loads and loads of caution.

At least on my laptop, powering the chip via USB seems to be fine, but in general you'll want a proper power supply that can supply something like 200-300mA.

You break it, I don't buy it.

## Features
Allows bi-directional wifi communication over serial with an ESP8266 board. It's meant to be easy to use and isn't particularly fast, smart, feature completed or tested. What it currently does do:

 * Runtime configuration of ssid and wifi password
 * Runs an http server (on whatever ip address your router assigns the chip, on port 80)
 * Http server listens to incoming requests and passes the method, path fragments and query string arguments through serial
 * Waits briefly for a response over serial which is sent back to the client
 * Allows setting of response status codes, only a couple (200, 401, 403, 404, 418 and 500) provide proper text status codes though.
 * Return a 404 if no response is supplied over serial
 * POST or GET to remote http server and return response over serial

## Missing features
There is a whole boatload of missing features, but these are the ones I'll probably add next

 * Automatic wifi reconnect
 * Runtime modification of serial baud-rate (it's set to 9600 currently), or at least a more sensible default
 * Built-in url scheme for controlling the 2 GPIO ports on the ESP8266

## Commands
 * `ssid <ssid>` - configure wifi ssid
 * `password <password>` - configure wifi password
 * `connect` - connect to wifi, using previously configured ssid and password
 * `debug` - print some basic status information
 * `led_mode <mode>` - Set status LED mode to 0 (show activity), 1 (show connection status), 2 (on) or 3 (off)
 * `timeout` - Configure server timeout, after which clients connecting to the server are disconnected
 * `get <host>[:<port>][/path/to/resource]` - Make an HTTP get request. Contents are returned over serial
 * `post <host>[:<port>][/path/to/resource`] - Same as get, but using POST.

## Response formats
Example output for a client requesting a resource at /test/1?what=up:

 * `method GET`
 * `num_fragments 2`
 * `path_fragment test`
 * `path_fragment 1`
 * `get_var what`
 * `get_value up`

A response can be supplied after a request comes in as just plain html or with a status at the beginning: `status:418 here's some content.`

Example output for a request to a website (`get somesite.com:8080/some/resource/or/other`)

 * `response_start`
 * `<html>`
 * `some data`
 * `</html>`
 * `response_end`

## Firmware setup

 * Get the necessary board data to make the ESP8266 work with the arduino IDE: https://github.com/esp8266/Arduino
 * Wire up the ESP for firmware uploading via ftdi adapter: VCC **and CH_PD** to 3V, GND and GPIO0 to GND, Reset to Reset/DTR, TX to RX, RX to TX.
 * Plug out and back in again the reset cable to reset in bootloader mode
 * Upload the esp8266_firmware sketch
 * After sucessfully uploading, remove GPIO0 from GND and Reset from Reset/DTR.
 * Everything should now be running, test via serial console.
