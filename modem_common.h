/*
 By Hamster on 2015/03/05
 Basic modem control
*/

#ifndef MODEM_COMMON_H
#define MODEM_COMMON_H

// For printf
#include <stdio.h>
#include "stub_stm32.h"
#include "global_config.h"

/* Local States */

// No response at all
#define MODEM_STATE_FAIL 0

// If returns OK for AT
// If we recerive "OK" without command prefix, just set
// MODEM_STATE_ON if it's fail now. If it's SIM_PRSNT/IN_SRV/REG,
// just leave them unchanged.
#define MODEM_STATE_ON 1

// If +CCID=no ERROR for AT+CCID (fall back to ON if ERROR)
#define MODEM_STATE_SIM_PRESENT (1 << 1)

// If +CSQ=(not 99)
#define MODEM_STATE_IN_SERVICE (1 << 2)

// Returns +CREG=(0 or 5) THIS IS THE NORMAL STATE for other operations
#define MODEM_STATE_REGISTERED (1 << 3)
#define MODEM_STATE_NORMAL MODEM_STATE_REGISTERED
#define MODEM_STATE_GPRS_CONNECTED (1 << 4)

// Only list result-demanding and active-issued commands.
// Don't care +CSQ.
// Those which return ERROR without +xxx prefix need this.
// See last_command_id
#define COMMAND_NONE 0
#define COMMAND_CMGS 1
#define COMMAND_XIIC (1 << 1)
#define COMMAND_HTTPSETUP (1 << 2)
#define COMMAND_HTTPACTION (1 << 3)
#define COMMAND_CCID (1 << 4)
#define COMMAND_CIMI (1 << 5)
// For anything else
#define COMMAND_BUSY -1

// max retries for +XIIC etc
#define MAX_RETRY_COUNT 3

// I see Unicom accepts this too
#define DEFAULT_APN "cmnet"

void set_modem_state(int state);
// This is a bitmask field. You need to clear that when done.
void set_command(int command_id);
// Clear the command_id bit. Should be called in interrupt handler
void clear_command(int command_id);
// Write a command to serial port (set COMMAND_BUSY)
// Used for sending a complete command (AT+CSQ)
void send_command(const char* buf);
// Same as above. Set custom_cmd_id
void send_command_with_id(const char* buf, int custom_command_id);
// Same as above. Doesn't set last_cmd_id and doesn't wait
// Used for sending partial command (AT+CMGS=, number, \r)
// Remember to set_command
void send_text(const char* buf);

void request_signal_strength();
void request_reg_status();
void request_sim_status();
void request_gprs_status();
void request_network_operator();
void request_connection_status();
void refresh_modem_status();
void connect_to_network();
void disconnect_from_network();

// OOP?
int is_sim_present();
int is_network_registered();
int is_gprs_connected();
int get_signal_strength();
int get_last_error_command();
void set_last_error_command(int command_id);
// 0=no signal, 1=one bar, 2=two bars etc, MAX 4
int get_signal_strength_level();
// -1=Unknown 0=China Unicom 1=China Mobile
int get_network_operator();

// Call from interrupt handler.
// buf is result from port
void process_result_common(const char* buf);

#endif