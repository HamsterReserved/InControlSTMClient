/*
 Sensor trigger related things
 */

#include "trigger.h"
#include "trigger_private.h"
#include "sensor.h"
#include "sms.h"
#include "global_config.h"

#include <string.h>
#include <stdio.h>

#ifdef USE_TRIGGER_ARRAY
TRIGGER_INFO* get_trigger_at(int i) {
    return &triggers[i];
}

void add_to_trigger(TRIGGER_INFO* trg) {
    int i = 0;

    for (; i <= MAX_TRIGGER_COUNT; ++i) {
        if (triggers[i].action == TRIGGER_ACTION_INVALID) break;
    }
    // i should be the first empty space
    if (i == MAX_TRIGGER_COUNT + 1) i--;

    triggers[i].action = trg->action;
    triggers[i].assoc_sensor_id = trg->assoc_sensor_id;
    triggers[i].compare_type = trg->compare_type;
    triggers[i].compare_value = trg->compare_value;
    strcpy(triggers[i].content, trg->content);
    strcpy(triggers[i].target, trg->target);
}
#endif

void build_into_trigger(TRIGGER_INFO* trg, const char* buf) {
    char* part_act;
    char* part_cond;
    char* tmp; // I'm silly
    
    part_cond = strtok((char*)buf, "|");
    part_act = strtok((char*)NULL, "|");

    // string like "null"
    if (part_cond == NULL || part_act == NULL) {
        trg->action = TRIGGER_ACTION_INVALID;
        return;
    }

    part_cond = strtok(buf, "|"); // reset strtok
    do {
        sscanf(part_cond, "%d,%d,%d", &(trg->compare_type), &(trg->compare_value), &(trg->assoc_sensor_id));
    } while (part_cond = strtok(NULL, ";"));

    tmp = part_act; // In case it nulled out
    part_act = strtok(part_act, ";");
    if (part_act == NULL) part_act = tmp;
    do {
        sscanf(part_act, "%d,%s,%s", &(trg->action), trg->target, trg->content);
    } while (strtok(NULL, ";"));
}

#ifdef USE_TRIGGER_ARRAY
void add_to_trigger_with_string(const char* buf) {
    int i = 0;
    TRIGGER_INFO trg;

    for (; i <= MAX_TRIGGER_COUNT; ++i) {
        if (triggers[i].action == TRIGGER_ACTION_INVALID) break;
    }
    // i should be the first empty space
    if (i == MAX_TRIGGER_COUNT + 1) i--;
    // no space
    // i is safe to use now

    build_into_trigger(&trg, buf);
    add_to_trigger(&trg);
}

void trigger_array_init() {
    int i = 0;
    for (; i <= MAX_TRIGGER_COUNT; ++i) {
        triggers[i].action = TRIGGER_ACTION_INVALID;
        triggers[i].assoc_sensor_id = i + 1;
        triggers[i].compare_value = -1;
        triggers[i].content[0] = 0;
        triggers[i].target[0] = 0;
    }
}
#endif

void send_trigger_alert_sms(SENSOR_INFO* snr, TRIGGER_INFO* trg) {
    char msg_buf[MESSAGE_BUFFER_SIZE] = "";
    char hex_buf[MESSAGE_BUFFER_SIZE] = "";
    SMS_SUBMIT_PARAM sms_param;

    // Note that SMS_SUBMIT_PARAM has three pointers rather than char array
    switch (trg->compare_type) {
        case COMPARE_TYPE_EQUAL:
            sprintf(msg_buf, "[InControl] Value of sensor %s (ID=%d) has reached %d .",
                snr->sensor_name, snr->sensor_id, snr->sensor_value);
            break;
        case COMPARE_TYPE_GREATER_THAN:
            sprintf(msg_buf, "[InControl] Value of sensor %s (ID=%d) has reached %d, "
                "greater than %d you set.",
                snr->sensor_name, snr->sensor_id, snr->sensor_value ,trg->compare_value);
            break;
        case COMPARE_TYPE_SMALLER_THAN:
            sprintf(msg_buf, "[InControl] Value of sensor %s (ID=%d) has reached %d, "
                "smaller than %d you set.",
                snr->sensor_name, snr->sensor_id, snr->sensor_value, trg->compare_value);
            break;
        case COMPARE_TYPE_DELTA_GREATER_THAN:
            sprintf(msg_buf, "[InControl] Value of sensor %s (ID=%d) has reached %d, "
                "delta is greater than %d you set.",
                snr->sensor_name, snr->sensor_id, snr->sensor_value, trg->compare_value);
            break;
    }
    //ascii_encode_string(hex_buf, msg_buf);
    // no need to convert as we use TEXT mode to send
    sms_param.content = msg_buf;
    sms_param.dest_number = trg->target;
    sms_param.service_center = NULL;
    send_sms(msg_buf, &sms_param);
}

void check_and_run_triggers() {
    int i = 0;
    TRIGGER_INFO trg;
    SENSOR_INFO* snr;

    for (; i < MAX_SENSORS_COUNT; i++) {
        snr = get_sensor_at(i);
        build_into_trigger(&trg, snr->sensor_trigger);
        switch (trg.compare_type) {
            case COMPARE_TYPE_EQUAL:
                if (snr->sensor_value == trg.compare_value) {
                    send_trigger_alert_sms(snr, &trg);
                }
                break;
            case COMPARE_TYPE_GREATER_THAN:
                if (snr->sensor_value > trg.compare_value) {
                    send_trigger_alert_sms(snr, &trg);
                }
                break;
            case COMPARE_TYPE_SMALLER_THAN:
                if (snr->sensor_value < trg.compare_value) {
                    send_trigger_alert_sms(snr, &trg);
                }
                break;
            case COMPARE_TYPE_DELTA_GREATER_THAN:
                // not implemented
                break;
        }
    }
}
