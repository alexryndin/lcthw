#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include "dbg.h"

#define MAX_DATA 100
#define INT_BUFF 12

int read_string(char **out_string, int max_buffer) {
    *out_string = malloc(max_buffer * sizeof(char));
    CHECK_MEM(*out_string);

    char *result = fgets(*out_string, max_buffer, stdin);
    CHECK(result != NULL, "Input error.");
    int i = 0;
    // remove trailing newline (if any)
    for (i = 0; i < max_buffer && (*out_string)[i] != '\0'; i++) {}
    if ((*out_string)[i-1] == '\n') {
        (*out_string)[i-1] = '\0';
    }

    return 0;
error:
    if(*out_string) free(*out_string);
    *out_string = NULL;
    return -1;
}

int read_int(int *out_int) {
    char *input = NULL;
    int rc = read_string(&input, MAX_DATA);
    CHECK(rc == 0, "Failed to read number.");

    *out_int = atoi(input);
    LOG_DEBUG("atoi returned %d", *out_int);

    free(input);
    return 0;
error:
    if (input) free(input);
    return -1;
}

int write_int(int in) {
    int buff[INT_BUFF] = {0};
    int i = 0;
    int m = in < 0 ? 1 : 0;
    int rc = 0;
    int ch = 0;
    LOG_DEBUG("Got int %d to write", in);
    if (m) {
        in *= -1;
    }
    while (in) {
        CHECK(i < INT_BUFF, "INT_BUFF overflow");
        buff[i] = in % 10;
        in /= 10;
        LOG_DEBUG("buff[%d] = %d, in = %d", i, buff[i], in);
        i++;
    }
    if (m) {
        rc = putchar('-');
        CHECK(rc == '-', "Write error.");
    }
    for (i = INT_BUFF - 1; i >= 0; i--) {
        if (buff[i] == '\0') continue;

        ch = 48 + buff[i];
        rc = putchar(ch);
        LOG_DEBUG("Write, rc = %d(%c), ch = %d(%c).", rc, rc, ch, ch);

        CHECK(rc == ch, "Write error, rc = %d(%c), ch = %d(%c).", rc, rc, ch, ch);
    }
    return 0;
error:
    return -1;
}

int writef(const char *fmt, ...) {
    int i = 0;
    int num = 0;
    int rc = 0;
    char *s = NULL;
    int c = '\0';
    va_list argp;
    va_start(argp, fmt);

    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case '\0':
                    SENTINEL("Invalid format, you ended with %%.");
                    break;

                case 'd':
                    num = va_arg(argp, int);
                    rc = write_int(num);
                    CHECK(rc == 0, "Failed to write int");
                    break;

                case 'c':
                    c = va_arg(argp, int);
                    rc = putchar(c);
                    CHECK(rc == c, "Write error.");
                    break;

                case 's':
                    // num here is max_data
                    num = va_arg(argp, int);
                    CHECK(num >= 0, "Wrong max_data");
                    if (num == 0) {
                        va_arg(argp, char*);
                        break;
                    }
                    s = va_arg(argp, char*);
                    for (int i = 0; i < num && s[i] != '\0'; i++) {
                        rc = putchar(s[i]);
                        CHECK(rc == s[i], "Write error.");
                    }
                    break;

                default:
                    SENTINEL("Invalid format.");
            }
        } else {
            rc = putchar(fmt[i]);
            CHECK(rc == fmt[i], "Write error.");
        }

        CHECK(!feof(stdout) && !ferror(stdout), "Output error.");
    }

    va_end(argp);
    return 0;
error:
    va_end(argp);
    return -1;

}

int read_scan(const char * const fmt, ...) {
    int i = 0;
    int rc = 0;
    int *out_int = NULL;
    char *out_char = NULL;
    char **out_string = NULL;
    int max_buffer = 0;

    va_list argp;
    va_start(argp, fmt);

    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case '\0':
                    SENTINEL("Invalid format, you ended with %%.");
                    break;

                case 'd':
                    out_int = va_arg(argp, int *);
                    rc = read_int(out_int);
                    CHECK(rc == 0, "Failed to read int");
                    break;

                case 'c':
                    out_char = va_arg(argp, char *);
                    *out_char = fgetc(stdin);
                    break;

                case 's':
                    max_buffer = va_arg(argp, int);
                    out_string = va_arg(argp, char **);
                    rc = read_string(out_string, max_buffer);
                    CHECK(rc == 0, "Failed to read string.");
                    break;

                default:
                    SENTINEL("Invalid format.");
            }
        } else {
            fgetc(stdin);
        }

        CHECK(!feof(stdin) && !ferror(stdin), "Input error.");
    }

    va_end(argp);
    return 0;
error:
    va_end(argp);
    return -1;
}

int read_stripped(char *buff, size_t max) {
    int i = 0;
    char c = '\0';
    while((c = getchar()) != EOF && i<(max-1)) {
        if (c == '\n') break;
        if (isblank(c)) {
            continue;
        }
        buff[i] = c;
        i++;
    }

    buff[i] = '\0';
    return i;
}



int main(int argc, char *argv[]) {
    char *first_name = NULL;
    char initial = ' ';
    char *last_name = NULL;
    int age = 0;

    printf("What's your First Name? ");
    int rc = read_scan("%s", MAX_DATA, &first_name);
    CHECK(rc == 0, "Failed to read first name.");

    printf("What's yout initial? ");
    rc = read_scan("%cj", &initial);
    CHECK(rc == 0, "Failed to read initial.");
    // scanf("%c\n\n\n\n\n\n\n\n", &initial);

    printf("What's your Last Name? ");
    rc = read_scan("%s", MAX_DATA, &last_name);
    CHECK(rc == 0, "Failed to read last name.");

    printf("How old are you? ");
    rc = read_scan("%d", &age);
    CHECK(rc == 0, "Failed to read last name.");

    printf("----- RESULTS -----\n");

    writef("First Name: %s\n", MAX_DATA, first_name);
    writef("Initial: %c\n", initial);
    writef("Last Name: %s\n", MAX_DATA, last_name);
    writef("Age: %d\n", age);

    return 0;
error:

    return 1;
}
