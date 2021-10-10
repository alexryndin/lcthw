#include <stdio.h>
#include <ctype.h>
#include "dbg.h"

int print_message(const char *msg, size_t len) {
    (void)(len); // suppress "unused" warning
    printf("A string: %s\n", msg);

    return 0;
}

int uppercase(const char *msg, size_t len) {
    size_t i = 0;

    for(i = 0; i < len && msg[i] != '\0'; i++) {
        printf("%c", toupper(msg[i]));
    }

    printf("\n");

    return 0;
}

int lowercase(const char *msg, size_t len) {
    size_t i = 0;

    // BUG: \0 termination problem
    for(i = 0; i < len && msg[i] != '\0'; i++) {
        printf("%c", tolower(msg[i]));
    }

    printf("\n");

    return 0;
}

int fail_on_purpose(const char *msg, size_t len) {
    (void)(len); // suppress "unused" warning
    (void)(msg); // suppress "unused" warning
    return 1;
}
