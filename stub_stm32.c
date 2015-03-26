/*
 * This module is for testing code on Win32 platform
 * Thus I call it STUB_stm32
 * If we are really onto STM32, these functions need to be
 * implemented.
 */
#include "stub_stm32.h"
#include "sensor.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void delay(long mills) {

}

void read_device_id(char *buf) {
    strcpy(buf, "1");
}

void log(const char* logstr) {
#ifdef LOG_ENABLED
    printf("LOG: %s\n", logstr);
#endif
}

void update_display() {
    // Workflow:
    // Step 1. main program calls refresh_network_status
    // Step 2. refresh_net_stat sends bunch of refreshing commands
    // Step 3. modem answers. Interrupt. Jump into process_result_common
    // Step 4. process_result_common finishes processing res and calls this function
    // Conclusion: NEVER CALL THIS IN OTHER PLACE!
    //
    // Remember to check get_last_error_command() to show an exclamation mark!
}

long get_current_time() {
    return time(0);
}

void on_init() {
    // initialize sensors array
    int i = 0;
    for (; i < MAX_SENSORS_COUNT; i++) {
        sensors[i].sensor_id = 0;
        sensors[i].sensor_name[0] = 0;
        sensors[i].sensor_trigger[0] = 0;
        sensors[i].sensor_type = SENSOR_TYPE_LIGHT;
        sensors[i].sensor_value = 0;
    }
}