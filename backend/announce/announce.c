#include "announce.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int fetch_announces(Announce **announces, int *count){
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return 1;
    }

    if (mysql_query(conn, "SELECT * FROM announces")) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 2;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 3;
    }

    int num_rows = mysql_num_rows(res);
    *count = num_rows;
    *announces = malloc(sizeof(Announce) * num_rows);

    for (int i = 0; i < num_rows; i++) {
        MYSQL_ROW row = mysql_fetch_row(res);
        (*announces)[i].announce_id= atoi(row[0]);
        strncpy((*announces)[i].flight_id, row[1], sizeof((*announces)[i].flight_id) - 1);
        strncpy((*announces)[i].content, row[2], sizeof((*announces)[i].content) - 1);
        strncpy((*announces)[i].updated_at, row[4], sizeof((*announces)[i].updated_at) - 1);
    }

    mysql_free_result(res);
    disconnect_db(conn);

    return 0;

}