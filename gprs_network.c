/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/
#include "stub_stm32.h"
#include "gprs_network.h"
#include "gprs_network_private.h"
#include "modem_common.h"
#include "sensor.h"

#include <string.h>

// strcpy API URL and strcat a "?"
#define BUILD_URL_BODY(buf) strcpy((buf), INCONTROL_API_URL); \
    strcat((buf), "?");

// strcat/append "x=y&"
#define APPEND_PARAMETER(buf, key, name) strcat((buf), (key)); \
    strcat((buf), "="); \
    strcat((buf), (name)); \
    strcat((buf), "&");

// Self-explanatory. Same as __stringify
// If x is expression,it will be evaluated
// THATS REALLY SMART!
#define _INT_STRINGIFY(x) #x
#define STRINGIFY(x) _INT_STRINGIFY(x)

void report_data(SENSOR_INFO* snr) {
    char id_buf[DEVICE_ID_LENGTH] = "";
    char url_buf[URL_BUFFER_LENGTH] = "";
    read_device_id(id_buf);

    BUILD_URL_BODY(url_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_ID, id_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_TYPE, STRINGIFY(DEVICE_TYPE));
    APPEND_PARAMETER(url_buf, PARAM_KEY_REQUEST_TYPE, STRINGIFY(REQUEST_TYPE_REPORT));
    APPEND_PARAMETER(url_buf, PARAM_KEY_SENSOR_ID, STRINGIFY(snr->sensor_name));
    APPEND_PARAMETER(url_buf, PARAM_KEY_SENSOR_DATE, STRINGIFY(get_current_time()));
    APPEND_PARAMETER(url_buf, PARAM_KEY_SENSOR_VALUE, STRINGIFY(snr->sensor_value));
    APPEND_PARAMETER(url_buf, PARAM_KEY_SENSOR_TYPE, STRINGIFY(snr->sensor_type));
    if (snr->sensor_name[0] != 0) {
        APPEND_PARAMETER(url_buf, PARAM_KEY_SENSOR_NAME, snr->sensor_name);
    }

    request_url(url_buf);
    set_last_request(REQUEST_TYPE_REPORT);
}

// State machine
void set_last_request(int request) {
    last_request = request;
}

void clear_last_request() {
    last_request = 0;
}

// OOP for external use
int get_last_request() {
    return last_request;
}

// For switching NORMAL/NEW CLIENT state
void switch_state(int state){
    // Make sure we only accept new Android clients when user agrees.
}

void request_self_name() {
    // Poor boy, you don't even know yourself's name.

}

void request_sensor_trigger(int sensor_id) {

}

// Append parameters yourself
void request_url(char* url) {
    // Should handle results in INT service
    char buf[URL_BUFFER_LENGTH + 17] = "AT+HTTPPARA=url,"; // AT+HTTPPARA=url,...\r
    
    strcat(buf, url);
    strcat(buf, "\r");
    send_command(buf);

    // send_cmd will wait for response so don't delay here
    strcpy(buf, "AT+HTTPSETUP\r");
    send_command_with_id(buf, COMMAND_HTTPSETUP);

    strcpy(buf, "AT+HTTPACTION=0\r");
    send_command_with_id(buf, COMMAND_HTTPACTION);
}

void process_http(const char* buf) {
    // seek from bottom and find \r\n
}