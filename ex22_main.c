#include "ex22.h"
#include "dbg.h"

const char *const MY_NAME = "Alex Ryndin";

// int THE_SIZE = 1000;

void scope_demo(int count) {
    LOG_INFO("count is: %d", count);

    if (count > 10) {
        int count = 100;

        LOG_INFO("count in this scope is %d", count);
    }

    LOG_INFO("count is at exit: %d", count);

    count = 3000;

    LOG_INFO("count after assign: %d", count);
}

int main(int argc, char *argv[]) {
    LOG_INFO("My name: %s, age: %d", MY_NAME, get_age());

    set_age(100);

    LOG_INFO("THE_SIZE is now: %d", THE_SIZE);
    print_size();

    THE_SIZE = 9;

    LOG_INFO("THE SIZE is now: %d", THE_SIZE);
    print_size();

    double *ratio = update_ratio(2.0);

    *ratio = 4.;

    LOG_INFO("Ratio at first: %f", *update_ratio(2.0));
    LOG_INFO("Ratio again: %f", update_ratio(10.0));
    LOG_INFO("Ratio once more: %f", update_ratio(300.0));

    int count = 4;
    scope_demo(count);
    scope_demo(count * 20);
    LOG_INFO("count after calling scope_demo: %d", count);

    return 0;
}
