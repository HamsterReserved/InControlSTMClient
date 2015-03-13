#include "stub_stm32.h"
#include <stdio.h>
#include <string.h>

void delay(long mills) {

}

void read_device_id(char *buf) {
    strcpy(buf, "1");
}

void log(const char* logstr) {
    printf("LOG: %s\n", logstr);
}

void update_display() {
    // Workflow:
    // Step 1. main program calls refresh_network_status
    // Step 2. refresh_net_stat sends bunch of refreshing commands
    // Step 3. modem answers. Interrupt. Jump into process_result
    // Step 4. process_result finishes processing res and calls this function
    // Conclusion: NEVER CALL THIS IN OTHER PLACE!
    //
    // Remember to check get_last_error_command() to show an exclamation mark!
}