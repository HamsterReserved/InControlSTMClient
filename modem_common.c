/*
 By Hamster on 2015/03/10 (not first created)
 Common/basic control and interrupt handler for modem.
 */

#include <string.h>
#include "modem_common.h"
#include "modem_vars.h"

// =0 means buf=a
#define BUF_STRNCMP(a) (strncmp(buf, (a), strlen((a))))

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

/********* DO NOT ATTEMPT TO ADD \r\n IN SEND_COMMAND *************/

void send_command_with_id(const char* buf, int custom_command_id) {
    while (last_command_id != COMMAND_NONE) {
        delay(10); // Wait until all other commands are done
        log(__FILE__ ": waiting for other command");
    }
    set_command(custom_command_id);
    last_err_command = COMMAND_NONE; // We are on the new way!

    // F says printf does the trick. I just keep this in case anything bad happens.
    printf(buf);
}

// Why doesn't C support overload?
void send_command(const char* buf) {
    send_command_with_id(buf, COMMAND_BUSY);
}

void send_text(const char* buf) {
    printf(buf);
}

/* Request senders */
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
    // If you want to show these on screen, call this and wait for 1sec
    // then you can read them.
    request_sim_status();
    request_signal_strength();
    request_reg_status();
    request_gprs_status();
}

int is_sim_present() { return modem_state >= MODEM_STATE_SIM_PRESENT; }

int is_network_registered() { return modem_state >= MODEM_STATE_REGISTERED; }

int is_gprs_connected() { return modem_state >= MODEM_STATE_GPRS_CONNECTED; }

int get_signal_strength() { return signal_strength; }

int get_last_error_command() { return last_err_command; }

/* These process_*  are for internal use only. No need to spam header file. */
void process_csq(const char* buf) {
    // <CRLF>+CSQ: 1, 99<CRLF>
    // <CRLF>OK<CRLF>
    int signal = 0; //1
    int ber = 0; //99, bit error rate
    int scan_count = 0;
    scan_count = sscanf(buf, "+CSQ: %d, %d", &signal, &ber); // Leave alone "OK"
    signal_strength = signal;

    if (signal > 4) {
        modem_state |= MODEM_STATE_IN_SERVICE;
    } else {
        modem_state &= ~MODEM_STATE_IN_SERVICE;
    }
}

void process_ccid(const char* buf) {
    // Already contains "+CCID" : ICCID is present
    // Don't care what ICCID is.
    // Otherwise only an "ERROR" and won't arrive here
    modem_state |= MODEM_STATE_SIM_PRESENT;
}

void process_xiic(const char* buf) {
    // +XIIC:    1, 10.232.165.29  
    // OK
    // +XIIC:    0, 0.0.0.0
    // OK
    int link_no = 0;
    char ip_buf[16] = "0.0.0.0"; // Max. 255.255.255.255 with 0
    sscanf(buf, "+XIIC: %d, %s", &link_no, ip_buf);
    if (ip_buf[0] == '0') { // First letter in IP MUST NOT BE 0
        modem_state &= ~MODEM_STATE_GPRS_CONNECTED;
    } else {
        modem_state |= MODEM_STATE_GPRS_CONNECTED;
    }
    // No need to clear_command. This is run with "AT+XIIC?"
}

void process_cmgs(const char* buf) {
    // +CMGS: 23
    // OK
    // What's the ERROR message? Does it come with +CMGS?
    // Assume not here.
    int msg_no = 0;
    int scan_count = 0;
    scan_count = sscanf(buf, "+CMGS: %d", &msg_no);
    clear_command(COMMAND_CMGS);
}

void process_creg(const char* buf) {
    // <CRLF>+CREG: 0, 1<CRLF>
    // <CRLF>OK<CRLF>
    int n = 0; //0
    int status = 0; //1
    int scan_count = 0;
    scan_count = sscanf(buf, "+CREG: %d, %d", &n, &status);
    if (status == 1 || status == 5) {
        modem_state |= MODEM_STATE_REGISTERED;
    } else {
        modem_state &= ~MODEM_STATE_REGISTERED;
    }
}

void process_cgatt(const char* buf) {
    // Not exist in AT Command Set!!??
}

void process_error() {
    log("ERROR returned for last command\n");
    clear_command(last_command_id);
    switch (last_command_id) {
        // No need to set last_err_cmd to COMMAND_BUSY when last cmd is BUSY
        case COMMAND_CMGS:
        case COMMAND_HTTPACTION:
        case COMMAND_HTTPSETUP:
        case COMMAND_XIIC:
            last_err_command = last_command_id;
            break;
    }
}

void process_ok() {
    log("last command returns OK\n");
    clear_command(last_command_id);
    last_err_command = COMMAND_NONE;
    // I guess this should be NONE when the cmd is sent
    // and operations are atomic
}

void process_result(const char* buf) {
    // Trim first?
    modem_state |= MODEM_STATE_ON; // Indicate it's alive.
    if (BUF_STRNCMP("+CSQ") == 0) {
        process_csq(buf);
    } else if (BUF_STRNCMP("+CCID") == 0) {
        process_ccid(buf);
    } else if (BUF_STRNCMP("+XIIC") == 0) {
        process_xiic(buf);
    } else if (BUF_STRNCMP("+CMGS") == 0) {
        process_cmgs(buf);
    } else if (BUF_STRNCMP("+CREG") == 0) {
        process_creg(buf);
    } else if (BUF_STRNCMP("+CGATT") == 0) {
        process_cgatt(buf);
    } else if (BUF_STRNCMP("ERROR") == 0) {
        process_error();
    } else if (BUF_STRNCMP("OK") == 0) {
        process_ok();
    }
    update_display();
}