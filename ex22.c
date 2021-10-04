#include <stdio.h>
#include "ex22.h"
#include "dbg.h"

int THE_SIZE = 1000;

static int THE_AGE = 37;

int get_age() {
    return THE_AGE;
}

void set_age(int age) {
    THE_AGE = age;
}

double *update_ratio(double new_ratio) {
    static double ratio = 1.0;

    return &ratio;
}

void print_size() {
    LOG_INFO("I think size is: %d", THE_SIZE);
}
