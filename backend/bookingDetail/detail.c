#include "detail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int insert_booking_detail(int booking_id, const char *type, const char *seat_code) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return 1;
    }

    
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO booking_details (booking_id, type, seat_code) "
             "VALUES (%d, '%s', '%s')",
             booking_id, type, seat_code);

    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "INSERT failed. Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    printf("Booking detail inserted successfully.\n");
    mysql_close(conn);
    return 0;
}