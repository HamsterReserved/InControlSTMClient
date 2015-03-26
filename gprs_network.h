/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/

#ifndef GPRS_NETWORK_H
#define GPRS_NETWORK_H

#include "sensor.h"

// OOP-like things
void set_last_request(int request);
void clear_last_request();
int get_last_request();
void set_last_err_request(int request);
int get_last_err_request();
void set_state(int state); // Not modem hardware state
int get_state();
void set_device_name(const char* buf);
void get_device_name(char* dest_buf);

void switch_state(int state);
void report_data(SENSOR_INFO* snr);
void request_self_name();
void request_sensor_list();
void request_url(char* url);
void process_http(const char* buf);
void process_http_ok();

#endif
