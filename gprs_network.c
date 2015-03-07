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

void refresh_network_status() {
    // Refresh all stats (signal, GPRS etc)
    // Do this before ANY operation!
    // Call every refresh function
    // NOTE: since operations are atomic, it's time-costly to refresh all
    // so I kept them separated if we want only one stat
    request_sim_status();
    request_signal_strength();
    request_reg_status();
    request_gprs_status();
    // refresh connection if we are manually connecting
}

void connect_to_network() {
    // Keep connected since power up.
    // We know exactly how much traffic we use, unlike iPhone or Android :-)

}

void disconnect_from_network() {

}

void request_self_name() {
    // Poor boy, you don't even know yourself's name.

}

void request_url(char* url) {
    // Should handle results in INT service

}

void request_signal_strength() {
    // AT+CSQ
}

void request_reg_status() {
    // AT+CREG
}

void request_sim_status() {
    // Will this be ever used since there's signal strength?
}

void request_gprs_status() {
    // AT+CGATT?
}

void request_connection_status() {
    // AT+XIIC?
}