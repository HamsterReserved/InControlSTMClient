/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/
#include "stub_stm32.h"
#include "gprs_network.h"
#include "modem_common.h"
#include "sensor.h"

void report_data(SENSOR_INFO* snr) {
    char params[PARAM_BUFFER_LENGTH][7] = { "", "", "", "", "", "", "" };
    char id_buf[DEVICE_ID_LENGTH] = "";
    read_device_id(id_buf);

    // params TODO params[0]="device_id"
    // strcpy(params[1], id_buf);
    // params[2]="sensor_type";
    // params[3]=1;...
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

void request_url(char* url, char** params, int param_count) {
    // Should handle results in INT service
    char buf[URL_BUFFER_LENGTH];
}

