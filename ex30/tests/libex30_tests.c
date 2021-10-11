#include "minunit.h"
#include <dlfcn.h>

typedef int (*lib_function) (const char *data, size_t len);
char *lib_file = "build/lib.so";
void *lib = NULL;

int check_function(const char *func_to_run, const char *data, size_t len, int expected) {
    lib_function func = dlsym(lib, func_to_run);
    CHECK(func != NULL,
            "Did not find %s function in the library %s: %s",
            func_to_run,
            lib_file,
            dlerror()
    );

    int rc = func(data, len);
    CHECK(rc == expected, "Function %s return %d for data: %s",
            func_to_run, rc, data);

    return 1;
error:
    return 0;
}

char *test_dlopen() {
    lib = dlopen(lib_file, RTLD_NOW);
    MU_ASSERT(lib != NULL, "Failed to open the library to test");
    return NULL;
}

char *test_functions() {
    MU_ASSERT(check_function("print_message", "Hello", 5, 0), "print_a_message failed.");
    MU_ASSERT(check_function("uppercase", "Hello", 5, 0), "uppercase failed.");
    MU_ASSERT(check_function("lowercase", "Hello", 5, 0), "lowercase failed.");

    return NULL;
}

char *test_failures() {
    MU_ASSERT(check_function("fail_on_purpose", "Hello", 5, 1),
            "fail_on_purpose should fail.");
    return NULL;
}

char *test_dlclose() {
    int rc = dlclose(lib);
    MU_ASSERT(rc == 0, "Failed to close lib.");

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_dlopen);
    MU_RUN_TEST(test_functions);
    MU_RUN_TEST(test_failures);
    MU_RUN_TEST(test_dlclose);

    return NULL;
}

RUN_TESTS(all_tests);
