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

typedef struct {
    int sensor_type;
    char sensor_name[SENSOR_NAME_BUFFER_SIZE];
    int sensor_value;
    char sensor_trigger[SENSOR_TRIGGER_BUFFER_SIZE];
} SENSOR_INFO;

void switch_state(int state);
void report_data(SENSOR_INFO* snr);
int is_network_available();
void connect_to_network();
void fetch_self_name(char* buf);
#endif
