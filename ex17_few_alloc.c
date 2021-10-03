#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Database {
    unsigned int _MAX_DATA;
    unsigned int _MAX_ROWS;
    void *blob;
};

size_t get_address_size(unsigned int max_data) {
    return
        sizeof(int)  +               // id
        sizeof(int)  +               // set
        sizeof(char)  * max_data +  // name
        sizeof(char)  * max_data;   // email
}

size_t get_database_size(unsigned int max_data, unsigned int max_rows) {
    return
        sizeof(unsigned int) +                // _MAX_DATA
        sizeof(unsigned int) +                // _MAX_ROWS
        sizeof(int) * max_rows +              // id
        sizeof(int) * max_rows +              // set
        sizeof(char) * max_data * max_rows +  // name
        sizeof(char) * max_data * max_rows;   // email
}

struct Connection {
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn);

void die(const char *message, struct Connection *conn) {
    Database_close(conn);
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

void Address_print(struct Address *addr, unsigned int max_data) {
    printf("%d %s %s\n", addr->id, (char*)&addr->name, (char*)(&addr->name + max_data));
}

void Database_load(struct Connection *conn) {
    if (fread(&conn->db->_MAX_DATA, sizeof(conn->db->_MAX_DATA), 1, conn->file) != 1)
        die("Failed to load database.", conn);
    if (fread(&conn->db->_MAX_ROWS, sizeof(conn->db->_MAX_ROWS), 1, conn->file) != 1)
        die("Failed to load database.", conn);
    if (
            fread(
                &conn->db,
                get_database_size(conn->db->_MAX_DATA, conn->db->_MAX_ROWS),
                1,
                conn->file
            ) != 1
    ) die("Failed to load id.", conn);
}

struct Connection *Database_open(
        const char *filename,
        char mode,
        unsigned int max_data,
        unsigned int max_rows
) {
    struct Connection *conn = malloc(sizeof(struct Connection));
    if (!conn)
        die("Memory error", conn);

    conn->db = malloc(get_database_size(max_data, max_rows));
    if (!conn->db)
        die("Memory error", conn);

    if (mode == 'c') {
        conn->db->_MAX_DATA = max_data;
        conn->db->_MAX_ROWS = max_rows;
        conn->db->blob = &conn->db->blob;
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");
        if (conn->file) {
            Database_load(conn);
        }
    }

    if (!conn->file)
        die("Failed to open the file", conn);

    return conn;
}

void Database_close(struct Connection *conn) {
    if (conn) {
        if (conn->file)
            fclose(conn->file);
        if (conn->db) {
            free(conn->db);

        }
        free(conn);
    }
}


void Database_write(struct Connection *conn) {
    rewind(conn->file);

    if (
            fwrite(
                &conn->db,
                get_database_size(conn->db->_MAX_DATA, conn->db->_MAX_ROWS),
                1,
                conn->file
            ) != 1
    ) die("Failed to write id.", conn);

    int rc = fflush(conn->file);
    if (rc == -1)
        die("Cannot flush database.", conn);
}

void Database_create(struct Connection *conn) {
    int i = 0;

    for (i = 0; i < conn->db->_MAX_ROWS; i++) {
        void *addr = conn->db->blob + i * get_address_size(conn->db->_MAX_DATA);
        (*(struct Address*)addr).set = 0;
        (*(struct Address*)addr).id = i;
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email) {
   // struct Address *addr = &conn->db->rows[id];
    struct Address *addr = conn->db->blob + id * get_address_size(conn->db->_MAX_DATA);
    if ((*(struct Address*)addr).set)
        die("Already set, delete it first", conn);

    ((struct Address*)addr)->set = 1;
    char *res = strncpy((char*)(&addr->name), name, conn->db->_MAX_DATA);
    *((&addr->name) + conn->db->_MAX_DATA - 1) = '\0';
    if (!res)
        die("Name copy failed", conn);

    res = strncpy((char*)((&addr->name) + conn->db->_MAX_DATA), email, conn->db->_MAX_DATA);
    *((&addr->name) + conn->db->_MAX_DATA + conn->db->_MAX_DATA  - 1) = '\0';
    if (!res)
        die("Email copy failed", conn);
}

void Database_get(struct Connection *conn, int id) {
    struct Address *addr = conn->db->blob + id * get_address_size(conn->db->_MAX_DATA);

    if (addr->set) {
        Address_print(addr, conn->db->_MAX_DATA);
    } else {
        die("ID is not set", conn);
    }
}

void Database_delete(struct Connection *conn, int id) {
    //struct Address addr = {.id = id, .set = 0 };
    //conn->db->rows[id] = addr;
    struct Address *addr = conn->db->blob + id * get_address_size(conn->db->_MAX_DATA);
    addr->set = 0;
}

void Database_list(struct Connection *conn) {
    int i = 0;

    for (i = 0; i < conn->db->_MAX_ROWS; i++) {
        struct Address *cur = conn->db->blob + i * get_address_size(conn->db->_MAX_DATA);

        if (cur->set) {
            Address_print(cur, conn->db->_MAX_DATA);
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc < 3)
        die("Usage: ex17 <dbfile> <action> [action params]", NULL);

    char *filename = argv[1];
    char action = argv[2][0];
    unsigned int max_data = 0, max_rows = 0;
    if (action == 'c') {
        if (argc != 5)
            die("Need an max_data and max_rows to set", NULL);
        max_data = atoi(argv[3]);
        max_rows = atoi(argv[4]);
    }
    struct Connection *conn = Database_open(filename, action, max_data, max_rows);
    int id = 0;

    if (action != 'c') {
        if (argc > 3) id = atoi(argv[3]);
        if (id >= conn->db->_MAX_ROWS) die("There's not that many records.", conn);
    }

    switch (action) {
        case 'c':
            if (argc != 5)
                die("Need an max_data and max_rows to set", conn);
            Database_create(conn);
            Database_write(conn);
            break;
        case 'g':
            if (argc != 4)
                die("Need an id to get", conn);
            Database_get(conn, id);
            break;
        case 's':
            if (argc != 6)
                die("Need an id, name, email to set", conn);

            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn);
            break;
        case 'd':
            if (argc != 4)
                die("Need id to delete", conn);
            Database_delete(conn, id);
            Database_write(conn);
            break;

        case 'l':
            Database_list(conn);
            break;

        default:
            die("Invalid action: c=create, g=get, s=set, d=del, l=list", conn);

    }

    Database_close(conn);

    return 0;
}
