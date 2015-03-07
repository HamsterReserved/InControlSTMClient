/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/
#include "stub_stm32.h"
#include "gprs_network.h"

void report_data(SENSOR_INFO* snr) {

}

void switch_state(int state){
    // Make sure we only accept new Android clients when user agrees.
}

int refresh_network_status() {
    // Refresh all stats (signal, GPRS etc)
    // Do this before ANY operation!
    return 0;
}

void connect_to_network() {
    // Keep connected since power up.
    // We know exactly how much traffic we use, unlike iPhone or Android :-)

}

void disconnect_from_network() {

}

void request_self_name(){
    // Poor boy, you don't even know yourself's name.

}

void request_url(char* url) {
    // Should handle results in INT service

}

void request_signal_strength() {

}