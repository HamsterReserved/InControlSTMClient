#include "sms.h"
#include "modem_common.h"

int main() { // For test
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

    sms.service_center = NULL;
    send_sms(NULL, &sms);
    clear_command(COMMAND_CMGS);

    sms.content = content_eng;
    send_sms(NULL, &sms);
    clear_command(COMMAND_CMGS);
}