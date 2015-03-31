/*
 External trigger related function and structures
 */

#ifndef TRIGGER_H
#define TRIGGER_H

#include "global_config.h"
#include "sensor.h"

// Target phone number
#define TRIGGER_TARGET_BUFFER_SIZE 50
// Not used. Make it smaller
#define TRIGGER_CONTENT_BUFFER_SIZE 50
#define MAX_TRIGGER_COUNT 50

#define TRIGGER_ACTION_INVALID -1
#define TRIGGER_ACTION_SEND_SMS 2
// Below two are NOT defined in PHP/Android!
#define TRIGGER_ACTION_FLASH_LED 3
#define TRIGGER_ACTION_BEEP 4

#define COMPARE_TYPE_EQUAL 0
#define COMPARE_TYPE_GREATER_THAN 1
#define COMPARE_TYPE_SMALLER_THAN 2
#define COMPARE_TYPE_DELTA_GREATER_THAN 3

typedef struct {
    char target[TRIGGER_TARGET_BUFFER_SIZE];
    char content[TRIGGER_CONTENT_BUFFER_SIZE];
    int action;
    int compare_value;
    int compare_type;
    int assoc_sensor_id;
} TRIGGER_INFO;

#ifdef USE_TRIGGER_ARRAY
void trigger_array_init();
void add_to_trigger(TRIGGER_INFO* trg);
TRIGGER_INFO* get_trigger_at(int i);
void add_to_trigger_with_string(const char* buf);
#endif

void build_into_trigger(TRIGGER_INFO* trg, const char* buf);
void send_trigger_alert_sms(SENSOR_INFO* snr, TRIGGER_INFO* trg);
void check_and_run_triggers(void);
#endif
