/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/

#ifndef GPRS_NETWORK_H
#define GPRS_NETWORK_H

void switch_state(int state);
void report_data(SENSOR_INFO* snr);
void request_self_name();
void request_sensor_trigger(int sensor_id);
void request_url(char* url);

#endif
