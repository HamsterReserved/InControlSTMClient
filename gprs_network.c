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

#define BUF_STRNCMP(a) (strncmp(buf, (a), strlen((a))))

// used in process_http function
char is_http_header_received = 0;

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
void set_last_request(int request) { last_request = request; }

// OOP for external use
int get_last_request() { return last_request; }

void set_last_err_request(int request) { last_err_request = request; }

int get_last_err_request() { return last_err_request; }

void set_device_name(const char* buf) { strcpy(device_name, buf); }

void get_device_name(char* dest_buf) { strcpy(dest_buf, device_name); }

// For switching NORMAL/NEW CLIENT state
void switch_state(int state){
    // Make sure we only accept new Android clients when user agrees.
    char id_buf[DEVICE_ID_LENGTH] = "";
    char url_buf[URL_BUFFER_LENGTH] = "";
    read_device_id(id_buf);

    BUILD_URL_BODY(url_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_ID, id_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_TYPE, STRINGIFY(DEVICE_TYPE));
    APPEND_PARAMETER(url_buf, PARAM_KEY_REQUEST_TYPE, STRINGIFY(REQUEST_TYPE_SWITCH_STATE));
    APPEND_PARAMETER(url_buf, PARAM_KEY_STATE, STRINGIFY(state));

    request_url(url_buf);
    set_last_request(REQUEST_TYPE_SWITCH_STATE);
}

void request_self_name() {
    // Poor boy, you don't even know yourself's name.
    char id_buf[DEVICE_ID_LENGTH] = "";
    char url_buf[URL_BUFFER_LENGTH] = "";
    read_device_id(id_buf);

    BUILD_URL_BODY(url_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_ID, id_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_TYPE, STRINGIFY(DEVICE_TYPE));
    APPEND_PARAMETER(url_buf, PARAM_KEY_REQUEST_TYPE, STRINGIFY(REQUEST_TYPE_SERVER_NAME));

    request_url(url_buf);
    set_last_request(REQUEST_TYPE_SERVER_NAME);
}

// This will request name and trigger simultaneously
void request_sensor_list() {
    char id_buf[DEVICE_ID_LENGTH] = "";
    char url_buf[URL_BUFFER_LENGTH] = "";
    read_device_id(id_buf);

    BUILD_URL_BODY(url_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_ID, id_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_TYPE, STRINGIFY(DEVICE_TYPE));
    APPEND_PARAMETER(url_buf, PARAM_KEY_REQUEST_TYPE, STRINGIFY(REQUEST_TYPE_SERVER_SENSOR_LIST));
    APPEND_PARAMETER(url_buf, PARAM_KEY_STATE, STRINGIFY(state));

    request_url(url_buf);
    set_last_request(REQUEST_TYPE_SERVER_SENSOR_LIST);
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

void process_report(const char* buf) {
    if (BUF_STRNCMP("OK") == 0) {
        log("process_report: OK");
        set_last_err_request(REQUEST_TYPE_NONE);
        // Wait for this STATE_NEW_CLNT in main function and 
        // prompt user to pair on Android
    } else {
        set_last_err_request(REQUEST_TYPE_REPORT);
        // Is this safe?
        // Dual-check last_err_command and last_err_request to
        // determine whether it DOES fail to report
    }
}

void process_server_name(const char* buf) {
    set_device_name(buf);
    set_last_err_request(REQUEST_TYPE_NONE);
}

void process_sensor_list(const char* buf) {
    // name,trigger|name,trigger...
    // TODO
    set_last_err_request(REQUEST_TYPE_NONE);
}

void process_switch_state(const char* buf) {
    if (BUF_STRNCMP("OK") == 0) {
        log("process_switch_state: OK");
        if (state == STATE_NORMAL) state == STATE_NEW_CLIENT;
        else state = STATE_NORMAL;
        // Wait for this STATE_NEW_CLNT in main function and 
        // prompt user to pair on Android
    }
    // Below is necessary to get it out of shadow of failure
    set_last_err_request(REQUEST_TYPE_NONE);
}


void process_http(const char* buf) {
    // Note that we have this function called every \r\n (process_result in modem_common)
    // So we just have to determine whether HTTP header has been received?
    // (by setting global var is_http_header_received)
    // After receiving header, the next \r\n will be the content
    if (BUF_STRNCMP("HTTP/1.1 200") == 0 ||
        BUF_STRNCMP("HTTP/1.0 200") == 0 ||
        BUF_STRNCMP("HTTP/2.0 200") == 0) {
        // HTTP 200 OK
        is_http_header_received = 1; // Ready to receive next part of \r\n
    } else if (BUF_STRNCMP("HTTP/1.1 501") == 0 ||
        BUF_STRNCMP("HTTP/1.0 501") == 0 ||
        BUF_STRNCMP("HTTP/2.0 501") == 0) {
        // HTTP 501 Not implemented
        // Parameter error
        set_last_error_command(COMMAND_HTTPACTION); // Sure we got to execute this command
        // Or we won't receive 501
    } else {
        if (is_http_header_received) {
            is_http_header_received = 0; // Clear it at once to prepare for next refresh
            switch (get_last_request()) {
                case REQUEST_TYPE_REPORT:
                    break;
                case REQUEST_TYPE_SERVER_NAME:
                    break;
                case REQUEST_TYPE_SWITCH_STATE:
                    break;
                case REQUEST_TYPE_SERVER_SENSOR_LIST:
                    break;
                case REQUEST_TYPE_USER_REGISTRATION:
                    // Do we ever use this here?
                    break;
            }
        }
    }
}