#include "../external/bstrlib/bstrlib.h"
#include "minunit.h"

char *test_simple() {
    char *t = "test";
    bstring b = bfromcstr("test");
    printf("%s\n", bdata(b));
    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);

    return NULL;
}

RUN_TESTS(all_tests);
