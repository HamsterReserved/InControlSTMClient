/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/
#include "stub_stm32.h"
#include "gprs_network.h"
#include "modem_common.h"
#include "sensor.h"

void report_data(SENSOR_INFO* snr) {

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

void request_url(char* url) {
    // Should handle results in INT service

}

