#include "ticket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int seat_counts = 0;

char* join_with_dash2(const char* row1, const char* row2, const char* row3) {

    size_t length = strlen(row1) + strlen(row2) + strlen(row3) + 3; 

    char* result = (char*)malloc(length * sizeof(char));
    if (!result) {
        perror("Failed to allocate memory");
        return NULL;
    }
    snprintf(result, length, "%s - %s - %s", row1, row2, row3);
    return result;
}


  

int fetch_tickets(Ticket **tickets, int *count, int user_id) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return 1;
    }

    char query[2048];
    snprintf(query, sizeof(query),
        "SELECT f.flight_id, f.departure_time, f.duration_minutes, al.name, da.airport_id, "
        "da.name, da.city, aa.airport_id, aa.name, aa.city, "
        "b.total_price, b.booking_id FROM flights f JOIN airplanes ap "
        "ON f.airplane_id = ap.airplane_id JOIN airlines "
        "al ON ap.airline_id = al.airline_id JOIN airports da "
        "ON f.departure_airport = da.airport_id JOIN airports aa "
        "ON f.arrival_airport = aa.airport_id RIGHT JOIN bookings b "
        "ON f.flight_id = b.flight_id "
        "WHERE user_id = %d", user_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 1;
    }

    int num_rows = mysql_num_rows(res);
    *count = num_rows;
    *tickets = malloc(sizeof(Ticket) * num_rows);

    for (int i = 0; i < num_rows; i++) {
        MYSQL_ROW row = mysql_fetch_row(res);
        strncpy((*tickets)[i].flight_id, row[0], sizeof((*tickets)[i].flight_id) - 1);
        strncpy((*tickets)[i].departure_time, row[1], sizeof((*tickets)[i].departure_time) - 1);
        (*tickets)[i].duration_minutes= atoi(row[2]);
        strncpy((*tickets)[i].airplane_name, row[3], sizeof((*tickets)[i].airplane_name) - 1);
        strncpy((*tickets)[i].departure_airport, join_with_dash2(row[4], row[5], row[6]), sizeof((*tickets)[i].departure_airport) - 1);
        strncpy((*tickets)[i].arrival_airport, join_with_dash2(row[7], row[8], row[9]), sizeof((*tickets)[i].arrival_airport) - 1);
        (*tickets)[i].total_price= atoi(row[10]);
        (*tickets)[i].booking_id= atoi(row[11]);
        strncpy((*tickets)[i].list_ticket, get_seat_codes_by_booking_id(&seat_counts, (*tickets)[i].booking_id), sizeof((*tickets)[i].list_ticket) - 1);
    }

    mysql_free_result(res);
    disconnect_db(conn);

    return 0;
}

char* get_seat_codes_by_booking_id(int *seat_counts,const int booking_id) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return NULL;
    }

    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[1024];

    snprintf(query, sizeof(query),
             "SELECT bd.seat_code "
             "FROM "
             "bookings b "
             "JOIN booking_details bd ON b.booking_id = bd.booking_id "
             "WHERE b.booking_id = %d", booking_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return NULL;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return NULL;
    }

    *seat_counts = mysql_num_rows(res);
    if (*seat_counts == 0) {
        mysql_free_result(res);
        disconnect_db(conn);
        return NULL; 
    }

    char *seat_codes = NULL;
    size_t total_length = 0;

    while ((row = mysql_fetch_row(res)) != NULL) {
        size_t row_length = strlen(row[0]);
        char *temp = realloc(seat_codes, total_length + row_length + 2); 
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed for seat_code\n");
            free(seat_codes);
            mysql_free_result(res);
            disconnect_db(conn);
            return NULL;
        }
        seat_codes = temp;
        
        strcpy(seat_codes + total_length, row[0]);
        total_length += row_length;

        seat_codes[total_length] = ' '; 
        total_length++;
        seat_codes[total_length] = '\0'; 
    }

    if (seat_codes == NULL) {
        seat_codes = strdup(""); 
        if (seat_codes == NULL) {
            fprintf(stderr, "Memory allocation failed for empty seat_code\n");
            mysql_free_result(res);
            disconnect_db(conn);
            return NULL;
        }
    }

    mysql_free_result(res);
    disconnect_db(conn);
    return seat_codes;
}

