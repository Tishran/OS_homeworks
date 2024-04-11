#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char *argv[]) {
    struct rtcdate* r = malloc(sizeof(struct rtcdate));
    (void) r;
    if (date(r) < 0) {

    } else {
        printf(1, "%d-%d-%d %d:%d:%d\n", r->year, r->month, r->day, r->hour, r->minute, r->second);
    }

    exit();
}
