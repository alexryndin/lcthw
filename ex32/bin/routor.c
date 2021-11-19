#include "dbg.h"
#include <bstrlib/bstrlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <tstree.h>

volatile sig_atomic_t stop = 0;

void inthand(int signum) {
    if (signum == SIGINT)
        stop = 1;
}

static const char *USAGE = "Usage: %s filepath\n";

static bstring read_line(const char *prompt) {
    printf("%s", prompt);
    bstring ret = bgets((bNgetc)fgetc, stdin, '\n');
    CHECK(btrimws(ret) == BSTR_OK, "Failed to trim input");

error:
    return ret;
}

static int add_route_data(TSTree *routes, bstring line) {
    int rc = 0;
    struct bstrList *split = bsplit(line, ' ');
    CHECK(split != NULL && split->qty == 2,
          "Wrong line, two space separated words expected.");

    CHECKRC(TSTree_insert(routes, bdata(split->entry[0]),
                          blength(split->entry[0]),
                          bstrcpy(split->entry[1])) != NULL,
            0, "Failed to insert route.");

    rc = 1;

error:
    if (split != NULL) {
        bstrListDestroy(split);
    }
    return rc;
}

static bstring match_url(TSTree *routes, bstring url) {
    bstring res = NULL;
    CHECK(routes != NULL, "Null routes.");
    CHECK(url != NULL, "Null url.");

    res = TSTree_search(routes, bdata(url), blength(url));

    if (res == NULL) {
        printf("Exact match not found, trying prefix.\n");
        res = TSTree_search_prefix(routes, bdata(url), blength(url));
    }

    if (res == NULL) {
        printf("Nothing found for url %s\n", bdata(url));
    } else {
        printf("Match: %s\n", bdata(res));
    }

error:
    return res;
}

static TSTree *TSTree_load_routes(const char *filepath) {
    FILE *f = NULL;
    bstring _read = NULL;
    TSTree *ret = TSTree_create();
    CHECK(ret != NULL, "Failed to create TSTree");

    f = fopen(filepath, "r");
    CHECK(f != NULL, "Failed to open %s", filepath);

    while ((_read = bgets((bNgetc)fgetc, f, '\n')) != NULL) {
        if (btrimws(_read) != BSTR_OK) {
            LOG_ERR("Failed to trim read string, skipping...");
            bdestroy(_read);
            _read = NULL;
            continue;
        }

        add_route_data(ret, _read);
    }
    goto exit;

error:
    if (ret != NULL) {
        TSTree_destroy(ret);
        ret = NULL;
    }
exit:
    if (f != NULL) {
        fclose(f);
        f = NULL;
    }
    if (_read != NULL) {
        bdestroy(_read);
        _read = NULL;
    }
    return ret;
}

static void bstring_destroy_cb(bstring value, void *data) {
    (void)data;
    bdestroy(value);
}

static void destroy_routes(TSTree *routes) {
    TSTree_traverse(routes, (TSTree_traverse_cb)bstring_destroy_cb, NULL);
    TSTree_destroy(routes);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf(USAGE, argv[0]);
        return 1;
    }

    bstring _read = NULL;

    TSTree *routes = TSTree_load_routes(argv[1]);
    CHECK(routes != NULL, "Failed to load routes.");

    while (true) {
        if (stop) {
            break;
        }
        _read = read_line("> ");
        if (_read == NULL) {
            CHECK(errno == 0, "Error reading line.");
            break;
        }

        match_url(routes, _read);
        bdestroy(_read);
        _read = NULL;
    }

error:
    if (routes != NULL) {
        destroy_routes(routes);
    }
    if (_read != NULL) {
        bdestroy(_read);
    }
    return 0;
}
