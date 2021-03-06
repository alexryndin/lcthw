#include <stdio.h>
#include "../dbg.h"
#include <dlfcn.h>

typedef int (*lib_function) (const char *data);

int main(int argc, char *argv[]) {
    int rc = 0;
    CHECK(argc == 4, "Usage: %s libex29.so function data", argv[0]);
    char *lib_file = argv[1];
    char *func_to_run = argv[2];
    char *data = argv[3];

    void *lib = dlopen(lib_file, RTLD_NOW);
    CHECK(lib != NULL, "Failed to open the library %s: %s", lib_file, dlerror());

    lib_function func = dlsym(lib, func_to_run);
    CHECK(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());

    rc = func(data);
    CHECK(rc == 0, "Function %s return %d for data: %s", func_to_run, rc, data);
    rc = dlclose(lib);
    CHECK(rc == 0, "Failed to close %s", lib_file);

    return 0;

error:
    return 1;
}
