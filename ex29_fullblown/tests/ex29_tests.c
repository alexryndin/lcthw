#include <stdio.h>
#include "../src/dbg.h"
#include <dlfcn.h>

typedef int (*lib_function) (const char *data, size_t len);

int test(void *lib, char *func, char *msg, size_t len) {
    int rc = 1;
    lib_function f = dlsym(lib, func);
    CHECK(f != NULL, "Did not find %s function in the library, error: %s", func, dlerror());

    rc = f(msg, len);
    CHECK(rc == 0, "Function %s return %d for data: %s", func, rc, msg);

    return 0;

error:
    return rc;
}

struct Test {
    char *func;
    char *msg;
    size_t len;
    int rc;
};

struct Test Tests[] = {
    {
        .func = "lowercase",
        .msg = "hi",
        .len = 4,
        .rc = 0,
    },
    {
        .func = "uppercase",
        .msg = "hi",
        .len = 1,
        .rc = 0,
    },
};

int main() {
    int rc = 0;
    char *lib_file = "build/lib.so";
    void *lib = dlopen(lib_file, RTLD_NOW);
    CHECK(lib != NULL, "Failed to open the library %s: %s", lib_file, dlerror());

    size_t number_of_tests = sizeof(Tests)/sizeof(struct Test);

    for (size_t i = 0; i < number_of_tests; i++) {
        rc = test(lib, Tests[i].func, Tests[i].msg, Tests[i].len);
        CHECK(rc == Tests[i].rc, "Test #%zu failed, expected rc %d, got rc %d", i, Tests[i].rc, rc);
    }

    rc = dlclose(lib);
    CHECK(rc == 0, "Failed to close %s", lib_file);

    return 0;

error:
    return 1;
}
