// Constants used both by the firmware and the barf library

#pragma once

const bool METHOD_GET = 0;
const bool METHOD_POST = 1;

const int LED_ACTIVITY = 0;
const int LED_CONNECTION = 1;
const int LED_OFF = 2;
const int LED_ON = 3;

const String COMMAND_DEBUG = "debug";
const String COMMAND_METHOD = "method";
const String COMMAND_NUM_FRAMENTS = "num_fragments";
const String COMMAND_PATH_FRAGMENT = "path_frament";
const String COMMAND_GET_VAR = "get_var";
const String COMMAND_GET_VALUE = "get_value";
const String COMMAND_REQUEST_RESPONSE = "respond";
const String COMMAND_RESPONSE_START = "response_start";
const String COMMAND_RESPONSE_END = "response_end";

const String COMMAND_SSID = "ssid";
const String COMMAND_PASSWORD = "password";
const String COMMAND_CONNECT = "connect";
const String COMMAND_DISCONNECT = "disconnect";
const String COMMAND_TIMEOUT = "timeout";
const String COMMAND_LED_MODE = "led_mode";
const String COMMAND_GET = "get";
const String COMMAND_POST = "post";
