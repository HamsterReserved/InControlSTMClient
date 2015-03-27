
#include "sms.h"
#include "modem_common.h"
#include "gprs_network.h"
#include "sensor.h"
#include "stub_stm32.h"
#include <string.h>

void test_sms() {
    SMS_SUBMIT_PARAM sms;
    char content_chs[] = "¹þ¹þ";
    char content_eng[] = "hello";
    char sender[] = "+8615527270000";
    char center[] = "+8613000713500";
    sms.content = content_chs;
    sms.dest_number = sender;
    sms.service_center = center;

    send_sms(NULL, &sms);
    clear_command(COMMAND_CMGS); // Should be in interrupt handler (+CMGS: ERROR)
    // Manually emulate clearing because we have no interrupt in VS

    sms.service_center = NULL;
    send_sms(NULL, &sms);
    clear_command(COMMAND_CMGS);

    sms.content = content_eng;
    send_sms(NULL, &sms);
    clear_command(COMMAND_CMGS);
}

void test_gprs() {
    SENSOR_INFO test_snr;
    char buf_for_all[1000];

    // Modem hardware
    request_connection_status();
    process_result_common("+XIIC:   0,0.0.0.0\r\nOK\r\n");

    // request_gprs_status(); // Not supported

    request_reg_status();
    process_result_common("+CREG: 0,0\r\nOK\r\n");

    request_signal_strength();
    process_result_common("+CSQ: 20,  99\r\nOK\r\n");

    request_sim_status();
    //process_result_common("+CCID: 89860\r\nOK\r\n");
    process_result_common("ERROR\r\n");

    // GPRS
    request_self_name();
    clear_command(COMMAND_BUSY);

    request_sensor_list();
    clear_command(COMMAND_BUSY);

    test_snr.sensor_id = 1008611;
    strcpy(test_snr.sensor_name, "Test_sensor"); // NO SPACE!
    test_snr.sensor_type = SENSOR_TYPE_ELECTRICITY;
    test_snr.sensor_value = 100;

    report_data(&test_snr);
    switch_state(STATE_NEW_CLIENT);
}

void test_operator() {
    request_network_operator();
    process_result_common("460010000000000\r\n");
    get_network_operator();
}

int main() { // For test
    void on_init(); // stub_stm32.c
    on_init();
    test_sms();
    test_gprs();
    test_operator();
}