#include "../external/bstrlib/bstrlib.h"
#include "../src/dbg.h"
#include "../src/hashmap_utils.h"
#include "../src/open_hashmap.h"
#include <stdio.h>

static int print_map(OpenHashmapNode *node) {
    printf("%s: %u\n", bdata((bstring)node->key), *(uint32_t *)node->data);
    return 0;
}

int main(int argc, char *argv[]) {
    int rc = 0;

    bstring b = bfromcstr("");
    /*
     What if uint32_t will overflow...
    ⠀⠀⠀⠀⠀⢀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠰⡿⠿⠛⠛⠻⠿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⣀⣄⡀⠀⠀⠀⠀⢀⣀⣀⣤⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⢸⣿⣿⣷⠀⠀⠀⠀⠛⠛⣿⣿⣿⡛⠿⠷⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠘⠿⠿⠋⠀⠀⠀⠀⠀⠀⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⣿⣷⣄⠀⢶⣶⣷⣶⣶⣤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⣿⣿⣿⠀⠀⠀⠀⠀⠈⠙⠻⠗⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⣰⣿⣿⣿⠀⠀⠀⠀⢀⣀⣠⣤⣴⣶⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⣠⣾⣿⣿⣿⣥⣶⣶⣿⣿⣿⣿⣿⠿⠿⠛⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠛⢿⣿⣿⣿⣿⣿⣿⡿⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠉⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    */
    uint32_t *value = NULL;
    FILE *f = NULL;

    if (argc == 1)
        goto exit;

    f = fopen(argv[1], "r");

    int ch = 0;

    OpenHashmap *map = OpenHashmap_create(NULL, NULL);
    while ((ch = fgetc(f)) != EOF) {
        if (isspace(ch)) {
            value = OpenHashmap_get(map, b);
            if (value != NULL) {
                bdestroy(b);
                (*value)++;
            } else {
                value = calloc(1, sizeof(uint32_t));
                CHECK_MEM(value);
                *value = 1;
                rc = OpenHashmap_set(map, b, value);
                CHECK(rc == 0, "map set failed.");
            }
            b = bfromcstr("");
        } else {
            bconchar(b, ch);
        }
    }
    OpenHashmap_traverse(map, print_map);
    CHECK(errno == 0, "Error reading file.");

exit:
    if (f != NULL) {
        fclose(f);
        f = NULL;
    }
    return rc;
error:
    rc = 1;
    goto exit;
}
