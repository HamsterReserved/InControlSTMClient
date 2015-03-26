/* For internal use of modem_common.c */
/* These variables shouldn't be accessible to other parts of program */

// private class Modem {
// DO NOT INCLUDE THIS IN FILES OTHER THAN MODEM_COMMON.C
// Or there will be too many "instances" for modem_state and last_command_id

#ifndef MODEM_VARS_H
#define MODEM_VARS_H

#include "modem_common.h"
#include "global_config.h"

// Status (see above h)
int modem_state = 0;

// Used for keep operaions "atomic" and answer in sync with request
// Remember to clear this when answer is received.
// We only allow commands to be executed when no other commands are pending.
// Can be used for indicator updates also.
int last_command_id = COMMAND_NONE;

// Set by process_error
// Cleared each time send_command_with_id called 
int last_err_command = COMMAND_NONE;

// Same format as AT+CSQ
// signal : rssi 
// <4 or 99 : <-107 dBm or unknown 
// <10 : <-93dBm
// <16 : <-71dBm
// <22 : <-69dBm
// <28 : <-57dBm
// >= 28 : >= -57 dBm
int signal_strength = 0;

#endif