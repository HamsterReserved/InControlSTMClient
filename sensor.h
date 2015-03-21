// This header doesn't have a corresponding C file.

#ifndef SENSOR_H
#define SENSOR_H

#define SENSOR_TYPE_LIGHT 0
#define SENSOR_TYPE_ELECTRICITY 1
#define SENSOR_TYPE_MOTION 2
#define SENSOR_TYPE_SWITCH 3
#define SENSOR_TYPE_IR 4

#define SENSOR_NAME_BUFFER_SIZE 100
#define SENSOR_TRIGGER_BUFFER_SIZE 500

typedef struct {
    int sensor_id;
    int sensor_type;
    char sensor_name[SENSOR_NAME_BUFFER_SIZE];
    int sensor_value;
    char sensor_trigger[SENSOR_TRIGGER_BUFFER_SIZE];
} SENSOR_INFO;

#endif SENSOR_H