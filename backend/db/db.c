#include "db.h"
#include <stdio.h>
#include <stdlib.h>

#define HOST "localhost"
#define USER "root"
#define PASS "12345678"
#define DBNAME "ticketrail"

MYSQL* connect_db() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }

    if (mysql_real_connect(conn, HOST, USER, PASS, DBNAME, 3306, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

void disconnect_db(MYSQL *conn) {
    mysql_close(conn);
}