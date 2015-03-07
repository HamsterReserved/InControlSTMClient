/*
 By Hamster on 2015/03/05
 Reporting data to cloud and read from cloud as well.
*/

#ifndef GPRS_NETWORK_H
#define GPRS_NETWORK_H

#define INCONTROL_API_URL "http://incontrol.sinaapp.com/incontrol_api.php"
// 1=Control Center 2=Android Client
// (Why do I get a feeling that I'm programming Android?)
#define DEVICE_TYPE 1

/* Remember to keep in sync with PHP and Android! */

//#define REQUEST_TYPE_ADD_NEW_DEVICE 99 Only used in factory
#define REQUEST_TYPE_SWITCH_STATE 97
#define REQUEST_TYPE_USER_REGISTRATION 98
#define REQUEST_TYPE_REPORT 1

#define STATE_NORMAL 0
#define STATE_NEW_CLIENT 1
//#define STATE_NEW_SENSOR 2

#define SENSOR_TYPE_LIGHT 0
#define SENSOR_TYPE_ELECTRICITY 1
#define SENSOR_TYPE_MOTION 2
#define SENSOR_TYPE_SWITCH 3
#define SENSOR_TYPE_IR 4

#define SENSOR_NAME_BUFFER_SIZE 100
#define SENSOR_TRIGGER_BUFFER_SIZE 500

/* Local States */
// Do OR on modem_state for new request
// Do AND(NOT) on modem_state for each answer acoordingly

// No response at all
#define MODEM_STATE_FAIL 0
// If returns OK for AT
#define MODEM_STATE_ON 1
// If +CCID=no ERROR for AT+CCID (fall back to ON if ERROR)
#define MODEM_STATE_SIM_PRESENT 2
// If +CSQ=(not 99)
#define MODEM_STATE_IN_SERVICE 4
// Returns +CREG=(0 or 5) THIS IS THE NORMAL STATE for other operations
#define MODEM_STATE_REGISTERED 8
#define MODEM_STATE_NORMAL MODEM_STATE_REGISTERED

// Only list result-demanding and active-issued commands.
// Don't care +CSQ/+CCID etc.
// See last_command_id
#define COMMAND_CMGS 1
#define COMMAND_XIIC 2
#define COMMAND_HTTPSETUP 3
#define COMMAND_HTTPACTION 4
// For anything else
#define COMMAND_BUSY 5

// Status (above)
int modem_state;
// Used for keep operaions "atomic" and answer in sync with request
// Remember to clear this when answer is received.
// We only allow commands to be executed when no other commands are pending.
// Can be used for indicator updates also.
int last_command_id;

typedef struct {
    int sensor_type;
    char sensor_name[SENSOR_NAME_BUFFER_SIZE];
    int sensor_value;
    char sensor_trigger[SENSOR_TRIGGER_BUFFER_SIZE];
} SENSOR_INFO;

void switch_state(int state);
void report_data(SENSOR_INFO* snr);
void refresh_network_status();
void connect_to_network();
void disconnect_from_network();
void request_self_name();
void request_url(char* url);
void request_signal_strength();
void request_reg_status();
void request_sim_status();
void request_gprs_status();
void request_connection_status();
#endif
