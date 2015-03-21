/*
By Hamster on 2015/03/21
Internal use for GPRS Network
*/

#ifndef GPRS_NETWORK_PRIVATE_H
#define GPRS_NETWORK_PRIVATE_H

#include "sensor.h"

#define INCONTROL_API_URL "http://incontrol.sinaapp.com/incontrol_api.php"
// 1=Control Center 2=Android Client
// (Why do I get a feeling that I'm programming Android?)
#define DEVICE_TYPE 1

/* Remember to keep in sync with PHP and Android! */

//#define REQUEST_TYPE_ADD_NEW_DEVICE 99 Only used in factory
#define REQUEST_TYPE_SWITCH_STATE 97
#define REQUEST_TYPE_USER_REGISTRATION 98
#define REQUEST_TYPE_REPORT 1
#define REQUEST_TYPE_SERVER_NAME 2
#define REQUEST_TYPE_SERVER_SENSOR_LIST 3

#define STATE_NORMAL 0
#define STATE_NEW_CLIENT 1
//#define STATE_NEW_SENSOR 2

#define URL_BUFFER_LENGTH 512
#define PARAM_BUFFER_LENGTH 100

// State machine
int last_request;

#endif