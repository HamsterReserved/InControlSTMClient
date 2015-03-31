/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.

 Notes about get_last_request and get_last_err_request:

 Every time a request is sent, last_request is set.
 It will NOT be cleared until next request is sent.

 Every time an invaild server response is received,
 last_err_request is set. It WILL be cleared when
 expected response is received.

 This is different from modem_common's last_err_cmd.
 Error mark is cleared on next COMMAND SENT there,
 but next expected RESULT RECEIVED here.

 Example:
          last_req=REPORT, last_err_req=REPORT
    means we did a report recently and it failed.

          last_req=SWITCH_STATE, lasr_err_req=REPORT
    means we did a switch but it hasn't returned. And
    another report has failed earlier.

          last_req=REPORT, last_err_req=NONE
    means we did a report recently and it succeeded.

 ABOUT SENSOR NAME: Default sensor name MUST NOT contain spaces
    or other URL unfriendly things!
*/
#include "stub_stm32.h"
#include "gprs_network.h"
#include "gprs_network_private.h"
#include "modem_common.h"
#include "sensor.h"
#include "trigger.h"

#include <string.h>

// strcpy API URL and strcat a "?"
#define BUILD_URL_BODY(buf) strcpy((buf), INCONTROL_API_URL); \
    strcat((buf), "?");

// strcat/append "x=y&"
#define APPEND_PARAMETER(buf, key, name) strcat((buf), (key)); \
    strcat((buf), "="); \
    strcat((buf), (name)); \
    strcat((buf), "&");

#define BUF_STRNCMP(a) (strncmp(buf, (a), strlen((a))))

// used in process_http function
char is_http_header_received = 0;

// State machine
void set_last_request(int request) { last_request = request; }

// OOP for external use
int get_last_request() { return last_request; }

void set_last_err_request(int request) { last_err_request = request; }

int get_last_err_request() { return last_err_request; }

void set_device_name(const char* buf) { strcpy(device_name, buf); }

void get_device_name(char* dest_buf) { strcpy(dest_buf, device_name); }

void append_parameter_int(char* buf, char* key, unsigned int value) {
    char digits[12] = "\0\0\0\0\0\0\0\0\0\0\0\0"; // 2147483647 with 0
    unsigned int now = value;
    int digit = 0;
    int i = 11;
    int j = 0;
    char tmp; // swap

    // say value = 1024
    while (digits[i--] = now % 10 + '0', now /= 10) {}
    // digits is "\0\0....1024"

    while (i++ < 11) {
        tmp = digits[i];
        digits[i] = digits[j];
        digits[j] = tmp;
        j++;
    }

    APPEND_PARAMETER(buf, key, digits);
}

void report_data(SENSOR_INFO* snr) {
    char id_buf[DEVICE_ID_LENGTH] = "";
    char url_buf[URL_BUFFER_LENGTH] = "";
    read_device_id(id_buf);

    BUILD_URL_BODY(url_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_ID, id_buf);
    append_parameter_int(url_buf, PARAM_KEY_DEVICE_TYPE, DEVICE_TYPE);
    append_parameter_int(url_buf, PARAM_KEY_REQUEST_TYPE, REQUEST_TYPE_REPORT);
    append_parameter_int(url_buf, PARAM_KEY_SENSOR_ID, snr->sensor_id);
    append_parameter_int(url_buf, PARAM_KEY_SENSOR_DATE, get_current_time());
    append_parameter_int(url_buf, PARAM_KEY_SENSOR_VALUE, snr->sensor_value);
    append_parameter_int(url_buf, PARAM_KEY_SENSOR_TYPE, snr->sensor_type);
    if (snr->sensor_name[0] != 0) {
        // TODO initialize sensors array on boot
        APPEND_PARAMETER(url_buf, PARAM_KEY_SENSOR_NAME, snr->sensor_name);
    }

    request_url(url_buf);
    set_last_request(REQUEST_TYPE_REPORT);
}

// For switching NORMAL/NEW CLIENT state
void switch_state(int state){
    // Make sure we only accept new Android clients when user agrees.
    char id_buf[DEVICE_ID_LENGTH] = "";
    char url_buf[URL_BUFFER_LENGTH] = "";
    read_device_id(id_buf);

    BUILD_URL_BODY(url_buf);
    APPEND_PARAMETER(url_buf, PARAM_KEY_DEVICE_ID, id_buf);
    append_parameter_int(url_buf, PARAM_KEY_DEVICE_TYPE, DEVICE_TYPE);
    append_parameter_int(url_buf, PARAM_KEY_REQUEST_TYPE, REQUEST_TYPE_SWITCH_STATE);
    append_parameter_int(url_buf, PARAM_KEY_STATE, state);

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
    append_parameter_int(url_buf, PARAM_KEY_DEVICE_TYPE, DEVICE_TYPE);
    append_parameter_int(url_buf, PARAM_KEY_REQUEST_TYPE, REQUEST_TYPE_SERVER_NAME);

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
    append_parameter_int(url_buf, PARAM_KEY_DEVICE_TYPE, DEVICE_TYPE);
    append_parameter_int(url_buf, PARAM_KEY_REQUEST_TYPE, REQUEST_TYPE_SERVER_SENSOR_LIST);
    append_parameter_int(url_buf, PARAM_KEY_STATE, state);

    request_url(url_buf);
    set_last_request(REQUEST_TYPE_SERVER_SENSOR_LIST);
}

// Append parameters yourself
void request_url(char* url) {
    // Should handle results in INT service
    char buf[URL_BUFFER_LENGTH + 17] = "AT+HTTPPARA=url,"; // AT+HTTPPARA=url,...\r
    int and_at = strlen(url) - 1;

    if (url[and_at] == '&') url[and_at] = 0; // trailing &

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
    // id,name,trigger|id,name,trigger...
    char* now_pos;
    int id = 0;
    char name[SENSOR_NAME_BUFFER_SIZE] = "";
    char trigger[SENSOR_TRIGGER_BUFFER_SIZE] = "";

    if (strstr(buf, "&") == NULL) {
        // Single sensor
        sscanf(buf, "%d,%s,%s", &id, name, trigger);
        add_to_sensors_with_attr(id, name, trigger, SENSOR_TYPE_LIGHT, 0);
#ifdef USE_TRIGGER_ARRAY
        add_to_trigger_with_string(trigger);
#endif
        // Default value
    } else {
        // TODO this produces a warning. Does strtok write to 1st param?
        now_pos = strtok((char*)buf, "&");
        do {
            sscanf(now_pos, "%d,%s,%s", &id, name, trigger);
            add_to_sensors_with_attr(id, name, trigger, SENSOR_TYPE_LIGHT, 0);
#ifdef USE_TRIGGER_ARRAY
            add_to_trigger_with_string(trigger);
#endif
        } while (now_pos = strtok(NULL, "&"));
    }
    set_last_err_request(REQUEST_TYPE_NONE);
}

void process_switch_state(const char* buf) {
    if (BUF_STRNCMP("OK") == 0) {
        log("process_switch_state: OK");
        if (state == STATE_NORMAL) state = STATE_NEW_CLIENT;
        else state = STATE_NORMAL;
        // Wait for this STATE_NEW_CLNT in main function and 
        // prompt user to pair on Android
    }
    // Below is necessary to get it out of shadow of failure
    set_last_err_request(REQUEST_TYPE_NONE);
}

// 0 for header (with :)
// 1 for content (without :)
// TODO: this is really rough
int is_content_line(const char* buf) {
    return strstr(buf, ":") == NULL;
}

void process_http(const char* buf) {
    // Note that we have this function called every \r\n (process_result_common in modem_common)
    // HTTP standard requires \r\n after each line
    // So this will be called for each line...
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
        if (!is_content_line(buf)) return;
        if (is_http_header_received) {
            is_http_header_received = 0; // Clear it at once to prepare for next refresh
            switch (get_last_request()) {
                case REQUEST_TYPE_REPORT:
                    // This OK will be processed in process_result_common
                    process_report(buf);
                    break;
                case REQUEST_TYPE_SERVER_NAME:
                    process_server_name(buf);
                    break;
                case REQUEST_TYPE_SWITCH_STATE:
                    process_switch_state(buf);
                    break;
                case REQUEST_TYPE_SERVER_SENSOR_LIST:
                    process_sensor_list(buf);
                    break;
                case REQUEST_TYPE_USER_REGISTRATION:
                    // Do we ever use this in STM32?
                    break;
            }
            // last_err_request is cleared in each process_*
            // No need to worry here
        }
    }
}

// Called in process_result_common
// Because "OK" will be handled in process_ok in modem_common
// (won't arrive process_http above, thus no process_report)
// so I add handler here.
void process_http_ok() {
    switch (get_last_request()) {
        case REQUEST_TYPE_REPORT:
        case REQUEST_TYPE_SWITCH_STATE:
        case REQUEST_TYPE_USER_REGISTRATION:
            set_last_err_request(REQUEST_TYPE_NONE);
            break;
        case REQUEST_TYPE_SERVER_NAME:
            // Who name his server "OK"?
            // Stand out and I promise I won't kill him.
            process_server_name("OK");
            break;
        case REQUEST_TYPE_SERVER_SENSOR_LIST:
            // Should never return "OK" - server failure
            set_last_err_request(REQUEST_TYPE_SERVER_SENSOR_LIST);
            break;
    }
}