

#include "sms.h"
#include "modem_common.h"
#include "gprs_network.h"
#include "sensor.h"
#include "stub_stm32.h"

void test_sms() {
    SMS_SUBMIT_PARAM sms;
    char content_chs[] = "¹þ¹þ";
    char content_eng[] = "hehe";
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
    clear_command(COMMAND_BUSY);
    request_gprs_status();
    clear_command(COMMAND_BUSY); // This will clear everything
    request_reg_status();
    clear_command(COMMAND_BUSY);
    request_signal_strength();
    clear_command(COMMAND_BUSY);
    request_sim_status();
    clear_command(COMMAND_BUSY);

    // GPRS
    request_self_name();
    clear_command(COMMAND_BUSY);
    request_sensor_list();
    clear_command(COMMAND_BUSY);

    test_snr.sensor_id = 1008611;
    strcpy(test_snr.sensor_name, "Test sensor");
    test_snr.sensor_type = SENSOR_TYPE_ELECTRICITY;
    test_snr.sensor_value = 100;

    report_data(&test_snr);
    switch_state(STATE_NEW_CLIENT);
}

int main() { // For test
    void on_init(); // stub_stm32.c
    on_init();
    test_sms();
    test_gprs();
}