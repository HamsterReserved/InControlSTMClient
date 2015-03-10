/*
 By Hamster on 2015/03/10 (not first created)
 Common/basic control and interrupt handler for modem.
 */
#include "modem_common.h"
#include "modem_vars.h"

// Let others know if modem is living
// State: see modem_common.h
void set_modem_state(int state) {
    modem_state = state;
}

void set_command(int command_id) {
    last_command_id = command_id; // This is = because we are atomic
}

void clear_command(int command_id) {
    last_command_id &= ~command_id;
}

void send_text(const char* buf) {
    printf(buf);
}

/********* DO NOT ATTEMPT TO ADD \r\n IN SEND_COMMAND *************/

void send_command(const char* buf) {
    while (last_command_id != COMMAND_NONE) {
        delay(10); // Wait until all other commands are done
        log(__FILE__ ": waiting for other command");
    }
    set_command(COMMAND_BUSY); // We don't know what's happening but it's just busy

    // F says printf does the trick. I just keep this in case anything bad happens.
    printf(buf);
}

// Why doesn't C support overload?
void send_command_with_id(const char* buf, int custom_command_id) {
    while (last_command_id != COMMAND_NONE) {
        delay(10); // Wait until all other commands are done
        log(__FILE__ ": waiting for other command");
    }
    set_command(custom_command_id);

    // F says printf does the trick. I just keep this in case anything bad happens.
    printf(buf);
}

void request_signal_strength() {
    send_command("AT+CSQ\r");
}

void request_reg_status() {
    send_command("AT+CREG?\r");
}

void request_sim_status() {
    // Will this be ever used since there's signal strength?
    send_command("AT+CCID\r");
}

void request_gprs_status() {
    send_command("AT+CGATT?\r");
}

void request_connection_status() {
    send_command("AT+XIIC?\r");
}

void connect_to_network() {
    // Keep connected since power up.
    // We know exactly how much traffic we use, unlike iPhone or Android :-)
    int retry_count = 0;
    while (retry_count < MAX_RETRY_COUNT) {
        if (modem_state == MODEM_STATE_NORMAL) {
            // No need to delay manually.
            // We can make sure that one command will be
            // executed after last one returns "OK" or "ERROR"
            send_command("AT+CGDCONT = 1, \"IP\", \"" DEFAULT_APN "\"\r");
            send_command("AT+CGATT=1\r");
            send_command_with_id("AT+XIIC=1\r", COMMAND_XIIC);
            // Same as above. Auto waiting for result.
            request_gprs_status(); // This is async
            delay(100); // Wait for result
            if (modem_state == MODEM_STATE_GPRS_CONNECTED) return;
        }
        delay(1000); // Give it chances to sort network issue before retry
        retry_count++;
    }
}

void disconnect_from_network() {
    send_command("AT+CGATT=0\r"); // Should be enough
}

void refresh_modem_status() {
    // NOTE: since operations are atomic, it's time-costly to refresh all
    // so I kept them separated if we want only one stat (e.g. in connect_to_network)
    request_sim_status();
    request_signal_strength();
    request_reg_status();
    request_gprs_status();
}

void process_result(const char* buf) {

}