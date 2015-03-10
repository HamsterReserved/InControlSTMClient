/* For internal use of modem_common.c */
/* These variables shouldn't be accessible to other parts of program */

// private class Modem {

#ifndef MODEM_VARS_H
#define MODEM_VARS_H

#include "modem_common.h"

// Status (see above h)
int modem_state = 0;

// Used for keep operaions "atomic" and answer in sync with request
// Remember to clear this when answer is received.
// We only allow commands to be executed when no other commands are pending.
// Can be used for indicator updates also.
int last_command_id = COMMAND_NONE;

#endif