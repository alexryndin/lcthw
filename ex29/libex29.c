#include <stdio.h>
#include <ctype.h>
#include "../dbg.h"

int print_message(const char *msg) {
    printf("A string: %s\n", msg);

    return 0;
}

int uppercase(const char *msg) {
    int i = 0;

    // BUG: \0 termination problem
    for(i = 0; msg[i] != '\0'; i++) {
        printf("%c", toupper(msg[i]));
    }

    printf("\n");

    return 0;
}

int lowercase(const char *msg) {
    int i = 0;

    // BUG: \0 termination problem
    for(i = 0; msg[i] != '\0'; i++) {
        printf("%c", tolower(msg[i]));
    }

    printf("\n");

    return 0;
}

int fail_on_purpose(const char *msg) {
    return 1;
}
