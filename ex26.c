#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>
#include <glob.h>


static const char * const DOTFILE_PATH = "~/.logfind";
static const char * const USAGE = "[-o] word [words...]";

int process_file(char *path, int flags, size_t wordc, char **wordv) {
    FILE *f = NULL;
    ssize_t read = 0;
    size_t len = 0, line = 0, found_count = 0;
    char *read_line = NULL;
    f = fopen(path, "r");
    CHECK(f, "Error opening file %s", path);

    while( (read = getline(&read_line, &len, f)) != -1) {
        found_count = 0;
        for (int i = 0; i < wordc; i++) {
            if (flags && found_count > 0) {
                break;
            }

            LOG_DEBUG("comparing line %s with %s", read_line, wordv[i]);

            if (strstr(read_line, wordv[i])) {
                found_count += 1;
            }
        }
        if ( (found_count && flags) || ((found_count == wordc) && !flags) ) {
            LOG_DEBUG("found_count = %d, wordc = %d, flags = %d", found_count, wordc, flags);
            printf("%s:%lu: %s", path, line, read_line);
        }

        line  += 1;
    }

    if (read_line) free(read_line);
    if (f) fclose(f);
    return 0;
error:
    if (read_line) free(read_line);
    if (f) fclose(f);
    return -1;
}

int process_glob(char *path, int flags, size_t wordc, char **wordv) {
    glob_t globbuf;
    int rc = 0;
    rc = glob(path, 0, NULL, &globbuf);

    if (rc == GLOB_NOMATCH) {
        LOG_DEBUG("glob didn't found any path for %s", path);
        errno = 0;
        goto exit;
    }

    CHECK( rc == 0 || rc == GLOB_NOMATCH, "glob failed with rc %d", rc);

    for(int i = 0; i < globbuf.gl_pathc; i++) {
        LOG_DEBUG("glob found path %s", globbuf.gl_pathv[i]);
        process_file(globbuf.gl_pathv[i], flags, wordc, wordv);
    }

    goto exit;
error:
    globfree(&globbuf);
    return -1;
exit:
    globfree(&globbuf);
    return 0;

}

int main(int argc, char *argv[]) {
    wordexp_t p = {0};
    FILE *dotfile = NULL;
    int words_start = 1, flags = 0;
    char *read_glob = NULL;
    ssize_t read = 0;
    size_t len = 0;

    CHECK(argc > 1, "Args error, Usage: %s", USAGE);

    if (strstr(argv[1], "-o")) {
        CHECK(argc > 2, "Args error, Usage: %s", USAGE);
        words_start = 2;
        flags = 1;
    }

    wordexp(DOTFILE_PATH, &p, 0);

    CHECK(p.we_wordc > 0 && p.we_wordv[0] != NULL, "%s not found", DOTFILE_PATH);

    char *path = p.we_wordv[0];

    dotfile = fopen(path, "r");
    CHECK(dotfile, "Error opening dotfile %s", path);

    while( (read = getline(&read_glob, &len, dotfile)) != -1) {
        if (read_glob[read-1] == '\n') read_glob[read-1] = '\0';
        LOG_DEBUG("read glob %s", read_glob);
        process_glob(read_glob, flags, argc - words_start, argv + words_start);
    }

    CHECK(errno == 0, "Error reading dotfile");

    if (dotfile) fclose(dotfile);
    if (read_glob) free(read_glob);
    wordfree(&p);
    return 0;
error:
    if (dotfile) fclose(dotfile);
    if (read_glob) free(read_glob);
    wordfree(&p);
    return 1;
}
