#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"

#define MAX_DATA 100

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES,
    BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

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
    char buff[MAX_DATA];
    Person you = { .age = 17 };
    int i = 0;
    char *in = NULL;

    printf("What's your First Name? ");
    in = fgets(you.first_name, MAX_DATA, stdin);
    CHECK(in != NULL, "Failed to read first name.");

    printf("What's your Last Name? ");
    int rc = read_stripped(you.last_name, MAX_DATA);
    CHECK(rc > 0, "Failed to read last name.");

    printf("How old are you? ");
    in = fgets(buff, MAX_DATA, stdin);
    CHECK(in != NULL, "Failed to read age.");
    you.age = atoi(buff);
    CHECK(you.age != 0, "You have to enter a non-zero number.");

    printf("What color are your eyes:\n");
    for (i = 0; i <= OTHER_EYES; i++) {
        printf("%d) %s\n", i + 1, EYE_COLOR_NAMES[i]);
    }
    printf("> ");
    int eyes = -1;
    rc = fscanf(stdin, "%d", &eyes);
    CHECK(rc > 0, "You have to enter a number.");

    you.eyes = eyes - 1;
    CHECK(you.eyes <= OTHER_EYES && you.eyes >= 0, "Do it right, that's not an option");

    printf("How much do you make an hour? ");
    rc = fscanf(stdin, "%f", &you.income);
    CHECK(rc > 0, "Enter a floating point number.");

    printf("----- RESULTS -----\n");

    printf("First Name: %s", you.first_name);
    printf("Last Name: %s\n", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %f\n", you.income);

    return 0;
error:

    return 1;
}
