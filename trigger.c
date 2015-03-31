/*
 Sensor trigger related things
 */

#include "trigger.h"
#include "trigger_private.h"
#include <string.h>

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

void add_to_trigger_with_string(const char* buf) {
    int i = 0;
    char* part_act;
    char* part_cond;
    char* tmp; // I'm silly

    for (; i <= MAX_TRIGGER_COUNT; ++i) {
        if (triggers[i].action == TRIGGER_ACTION_INVALID) break;
    }
    // i should be the first empty space
    if (i == MAX_TRIGGER_COUNT + 1) i--;
    // no space
    // i is safe to use now

    part_cond = strtok(buf, "|");
    part_act = strtok(NULL, "|");

    part_cond = strtok(buf, "|"); // reset strtok
    do {
        sscanf(part_cond, "%d,%d,%d", &triggers[i].compare_type, &triggers[i].compare_value, &triggers[i].assoc_sensor_id);
    } while (part_cond = strtok(NULL, ";"));

    tmp = part_act; // In case it nulled out
    part_act = strtok(part_act, ";");
    if (part_act == NULL) part_act = tmp;
    do {
        sscanf(part_act, "%d,%s,%s", &triggers[i].action, triggers[i].target, triggers[i].content);
    } while (strtok(NULL, ";"));
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
