#include <stdio.h>
#include <string.h>
#include "dbg.h"
#include "ex23.h"

int normal_copy(char *from, char *to, int count) {
    int i = 0;

    for (i = 0; i < count; i++) {
        to[i] = from[i];
    }

    return i;
}

int duffs_device(char *from, char *to, int count) {
    int n = (count + 7) / 8;

    switch (count % 8) {
        case 0:
            do {
                *to++ = *from++;
                case 7:
                *to++ = *from++;
                case 6:
                *to++ = *from++;
                case 5:
                *to++ = *from++;
                case 4:
                *to++ = *from++;
                case 3:
                *to++ = *from++;
                case 2:
                *to++ = *from++;
                case 1:
                *to++ = *from++;
            } while (--n > 0);
    }
    return count;
}

int zeds_device(char *from, char *to, int count) {
    int n = (count + 7) / 8;

    switch (count % 8) {
        case 0:
again:  *to++ = *from++;
        case 7:
        *to++ = *from++;
        case 6:
        *to++ = *from++;
        case 5:
        *to++ = *from++;
        case 4:
        *to++ = *from++;
        case 3:
        *to++ = *from++;
        case 2:
        *to++ = *from++;
        case 1:
        *to++ = *from++;
        if (--n > 0) goto again;
    }
    return count;
}

DUFF(8)
DUFF(64)

int wikiduff_device(char *from, char *to, int count) {
    int n = (count + 7) / 8;
    switch (count % 8) {
    case 0: do { *to = *from++;
    case 7:      *to = *from++;
    case 6:      *to = *from++;
    case 5:      *to = *from++;
    case 4:      *to = *from++;
    case 3:      *to = *from++;
    case 2:      *to = *from++;
    case 1:      *to = *from++;
            } while (--n > 0);
    }
    return count;
}

int validate_copy(char *data, int count, char expects) {
    int i = 0;
    for (i = 0; i < count; i++) {
        if (data[i] != expects) {
            LOG_ERR("[%d] %c != %c", i, data[i], expects);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
        char from[1000];
        char to[1000];
        int rc = 0;

        memset(from, 'x', 1000);
        memset(to, 'y', 1000);
        CHECK(validate_copy(to, 1000, 'y'), "Not initialized right.");

        rc = normal_copy(from, to, 1000);
        CHECK(rc == 1000, "Normal copy failed: %d", rc);
        CHECK(validate_copy(to, 1000, 'x'), "Normal copy failed: %d", rc);

        memset(to, 'y', 1000);
        rc = duffs_device(from, to, 1000);
        CHECK(rc == 1000, "Duff's device failed: %d", rc);
        CHECK(validate_copy(to, 1000, 'x'), "Duff's device failed: %d", rc);

        memset(to, 'y', 1000);
        rc = zeds_device(from, to, 1000);
        CHECK(rc == 1000, "Zed's device failed: %d", rc);
        CHECK(validate_copy(to, 1000, 'x'), "Zed's device failed: %d", rc);

        memset(to, 'y', 1000);
        rc = wikiduff_device(from, to, 1000);
        CHECK(rc == 1000, "Zed's device failed: %d", rc);
        CHECK(validate_copy(to, 1000, 'x'), "Zed's device failed: %d", rc);
        memset(to, 'y', 1000);
        rc = wikiduff_device_64(from, to, 1000);
        CHECK(rc == 1000, "Zed's device failed: %d", rc);
        CHECK(validate_copy(to, 1000, 'x'), "Zed's device failed: %d", rc);

        return 0;
error:
        return -1;

}
