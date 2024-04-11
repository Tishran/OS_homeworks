#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char *argv[]) {
    struct rtcdate* r = malloc(sizeof(struct rtcdate));
    (void) r;
    if (date(r) < 0) {

    } else {
        char* month_format = "0%d-";
        if (r->month >= 10) {
            month_format = "%d-";
        }

        char* day_format = "0%d ";
        if (r->day >= 10) {
            day_format = "%d ";
        }

        char* hour_format = "0%d:";
        if (r->hour >= 10) {
            hour_format = "%d:";
        }

        char* minute_format = "0%d:";
        if (r->minute >= 10) {
           minute_format = "%d:";
        }

        char* second_format = "0%d\n";
        if (r->second >= 10) {
            second_format = "%d\n";
        }

        printf(1, "%d-", r->year);
        printf(1, month_format, r->month);
        printf(1, day_format, r->day);
        printf(1, hour_format, r->hour);
        printf(1, minute_format, r->minute);
        printf(1, second_format, r->second);
    }

    exit();
}
