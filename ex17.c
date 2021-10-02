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
    struct Address *rows;
};

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

void Address_print(struct Address *addr) {
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn) {
    if (fread(&conn->db->_MAX_DATA, sizeof(conn->db->_MAX_DATA), 1, conn->file) != 1)
        die("Failed to load database.", conn);
    if (fread(&conn->db->_MAX_ROWS, sizeof(conn->db->_MAX_ROWS), 1, conn->file) != 1)
        die("Failed to load database.", conn);
    struct Address *rows = malloc(sizeof(struct Address) * conn->db->_MAX_ROWS);
    if (!rows)
        die("Memory error", conn);
    conn->db->rows = rows;
    for (int i = 0; i < conn->db->_MAX_ROWS; i++) {
        if (
                fread(
                    &rows[i].id,
                    sizeof(rows[i].id),
                    1,
                    conn->file
                ) != 1
        ) die("Failed to load id.", conn);
        if (
                fread(
                    &rows[i].set,
                    sizeof(rows[i].set),
                    1,
                    conn->file
                ) != 1
        ) die("Failed to load set.", conn);
        char *name = malloc(sizeof(char) * conn->db->_MAX_DATA);
        char *email = malloc(sizeof(char) * conn->db->_MAX_DATA);

        if (!name || !email)
            die("Memory error", conn);

        rows[i].name = name;
        rows[i].email = email;
        if (
                fread(
                    name,
                    sizeof(char),
                    conn->db->_MAX_DATA,
                    conn->file
                ) != conn->db->_MAX_DATA
        ) die("Failed to load name.", conn);
        if (
                fread(
                    email,
                    sizeof(char),
                    conn->db->_MAX_DATA,
                    conn->file
                ) != conn->db->_MAX_DATA
        ) die("Failed to load email.", conn);

    }
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

    conn->db = malloc(sizeof(struct Database));
    if (!conn->db)
        die("Memory error", conn);

    if (mode == 'c') {
        conn->db->_MAX_DATA = max_data;
        conn->db->_MAX_ROWS = max_rows;
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
            if (conn->db->rows) {
                for (int i = 0; i < conn->db->_MAX_ROWS; i++) {
                    free(conn->db->rows[i].name);
                    free(conn->db->rows[i].email);
                }
                free(conn->db->rows);
            }
            free(conn->db);

        }
        free(conn);
    }
}


void Database_write(struct Connection *conn) {
    rewind(conn->file);

    if (
        fwrite(
            &conn->db->_MAX_DATA,
            sizeof(conn->db->_MAX_DATA),
            1,
            conn->file
        ) != 1
    ) die("Failed to write max_data.", conn);
    if (
        fwrite(
            &conn->db->_MAX_ROWS,
            sizeof(conn->db->_MAX_ROWS),
            1,
            conn->file
        ) != 1
    ) die("Failed to write max_rows.", conn);
    for (int i = 0; i < conn->db->_MAX_ROWS; i++) {
        if (
                fwrite(
                    &conn->db->rows[i].id,
                    sizeof(conn->db->rows[i].id),
                    1,
                    conn->file
                ) != 1
        ) die("Failed to write id.", conn);
        if (
                fwrite(
                    &conn->db->rows[i].set,
                    sizeof(conn->db->rows[i].set),
                    1,
                    conn->file
                ) != 1
        ) die("Failed to write set.", conn);

        if (
                fwrite(
                    conn->db->rows[i].name,
                    sizeof(char),
                    conn->db->_MAX_DATA,
                    conn->file
                ) != conn->db->_MAX_DATA
        ) die("Failed to write name.", conn);
        if (
                fwrite(
                    conn->db->rows[i].email,
                    sizeof(char),
                    conn->db->_MAX_DATA,
                    conn->file
                ) != conn->db->_MAX_DATA
        ) die("Failed to write email.", conn);
    }

    int rc = fflush(conn->file);
    if (rc == -1)
        die("Cannot flush database.", conn);
}

void Database_create(struct Connection *conn) {
    int i = 0;

    struct Address *rows = malloc(sizeof(struct Address) * conn->db->_MAX_ROWS);
    if (!rows)
        die("Memory error", conn);
    conn->db->rows = rows;
    for (i = 0; i < conn->db->_MAX_ROWS; i++) {
        char* name = calloc(sizeof(char), conn->db->_MAX_DATA);
        char* email = calloc(sizeof(char), conn->db->_MAX_DATA);
        struct Address addr = { .id = i, .set = 0, .name = name, .email = email };
        conn->db->rows[i] = addr;
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email) {
    struct Address *addr = &conn->db->rows[id];
    if (addr->set)
        die("Already set, delete it first", conn);

    addr->set = 1;
    char *res = strncpy(addr->name, name, conn->db->_MAX_DATA);
    addr->name[conn->db->_MAX_DATA - 1] = '\0';

    if (!res)
        die("Name copy failed", conn);

    res = strncpy(addr->email, email, conn->db->_MAX_DATA);
    addr->email[conn->db->_MAX_DATA - 1] = '\0';
    if (!res)
        die("Email copy failed", conn);
}

void Database_get(struct Connection *conn, int id) {
    struct Address *addr = &conn->db->rows[id];

    if (addr->set) {
        Address_print(addr);
    } else {
        die("ID is not set", conn);
    }
}

void Database_delete(struct Connection *conn, int id) {
    //struct Address addr = {.id = id, .set = 0 };
    //conn->db->rows[id] = addr;
    conn->db->rows[id].set = 0;
}

void Database_list(struct Connection *conn) {
    int i = 0;
    struct Database *db = conn->db;

    for (i = 0; i < conn->db->_MAX_ROWS; i++) {
        struct Address *cur = &db->rows[i];

        if (cur->set) {
            Address_print(cur);
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
