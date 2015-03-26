/*
By Hamster on 2015/03/21
Internal use for GPRS Network
*/

#ifndef GPRS_NETWORK_PRIVATE_H
#define GPRS_NETWORK_PRIVATE_H

#include "global_config.h"

#define INCONTROL_API_URL "http://incontrol.sinaapp.com/incontrol_api.php"
// 1=Control Center 2=Android Client
// (Why do I get a feeling that I'm programming Android?)
#define DEVICE_TYPE 1

/* Remember to keep in sync with PHP and Android! */

// Place holder
#define REQUEST_TYPE_NONE 0
//#define REQUEST_TYPE_ADD_NEW_DEVICE 99 Only used in factory
#define REQUEST_TYPE_SWITCH_STATE 97
// USER_REGISTRATION is used on Android I suppose?
#define REQUEST_TYPE_USER_REGISTRATION 98
#define REQUEST_TYPE_REPORT 1
#define REQUEST_TYPE_SERVER_NAME 2
#define REQUEST_TYPE_SERVER_SENSOR_LIST 3

#define PARAM_KEY_DEVICE_ID "device_id"
#define PARAM_KEY_DEVICE_TYPE "device_type"
#define PARAM_KEY_REQUEST_TYPE "request_type"
#define PARAM_KEY_SENSOR_ID "sensor_id"
// This is not "sensor_date"!
#define PARAM_KEY_SENSOR_DATE "info_date"
#define PARAM_KEY_SENSOR_VALUE "sensor_value"
#define PARAM_KEY_SENSOR_NAME "sensor_name"
#define PARAM_KEY_SENSOR_TYPE "sensor_type"
#define PARAM_KEY_STATE "state"

#define URL_BUFFER_LENGTH 2048
#define PARAM_BUFFER_LENGTH 100
#define DEVICE_NAME_BUFFER_LENGTH 100

// State machine
int last_request = REQUEST_TYPE_NONE;
int last_err_request = REQUEST_TYPE_NONE;

// Whether we can accept new Android client
int state = STATE_NORMAL;

char device_name[DEVICE_NAME_BUFFER_LENGTH];

#endif