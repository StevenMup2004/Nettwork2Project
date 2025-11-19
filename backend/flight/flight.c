#include "flight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* join_with_dash(const char* row1, const char* row2, const char* row3) {
    // Tăng kích thước bộ nhớ đệm để chứa đủ các ký tự phân cách " - " và ký tự kết thúc chuỗi
    size_t length = strlen(row1) + strlen(row2) + strlen(row3) + 7; 

    char* result = (char*)malloc(length * sizeof(char));
    if (!result) {
        perror("Failed to allocate memory");
        return NULL;
    }
    snprintf(result, length, "%s - %s - %s", row1, row2, row3);
    return result;
}

int fetch_flights(Flight **flights, int *count) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return 1;
    }

    if (mysql_query(conn, 
    "SELECT f.*, ap.*, al.*, da.*, aa.* FROM flights f "
    "JOIN airplanes ap ON f.airplane_id = ap.airplane_id "
    "JOIN airlines al ON ap.airline_id = al.airline_id "
    "JOIN airports da ON f.departure_airport = da.airport_id "
    "JOIN airports aa ON f.arrival_airport = aa.airport_id")) {
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
    *flights = malloc(sizeof(Flight) * num_rows);

    for (int i = 0; i < num_rows; i++) {
        MYSQL_ROW row = mysql_fetch_row(res);
        
        // Sao chép dữ liệu và đảm bảo có ký tự kết thúc chuỗi
        strncpy((*flights)[i].flight_id, row[0], sizeof((*flights)[i].flight_id) - 1);
        (*flights)[i].flight_id[sizeof((*flights)[i].flight_id) - 1] = '\0';

        strncpy((*flights)[i].departure_time, row[3], sizeof((*flights)[i].departure_time) - 1);
        (*flights)[i].departure_time[sizeof((*flights)[i].departure_time) - 1] = '\0';

        // Kiểm tra NULL trước khi dùng atoi để tránh Segmentation Fault
        (*flights)[i].duration_minutes = (row[4] != NULL) ? atoi(row[4]) : 0;
        
        // Sửa lại thứ tự cột: row[10] là Price, row[11] là Capacity
        (*flights)[i].price = (row[10] != NULL) ? atoi(row[10]) : 0; 
        (*flights)[i].capacity = (row[11] != NULL) ? atoi(row[11]) : 0; 

        strncpy((*flights)[i].airplane_name, row[13], sizeof((*flights)[i].airplane_name) - 1);
        (*flights)[i].airplane_name[sizeof((*flights)[i].airplane_name) - 1] = '\0';

        // Xử lý chuỗi ghép và giải phóng bộ nhớ để tránh Memory Leak
        char *dep_airport = join_with_dash(row[17], row[18], row[19]);
        if (dep_airport) {
            strncpy((*flights)[i].departure_airport, dep_airport, sizeof((*flights)[i].departure_airport) - 1);
            (*flights)[i].departure_airport[sizeof((*flights)[i].departure_airport) - 1] = '\0';
            free(dep_airport); // Quan trọng: Giải phóng bộ nhớ sau khi dùng
        }

        char *arr_airport = join_with_dash(row[20], row[21], row[22]);
        if (arr_airport) {
            strncpy((*flights)[i].arrival_airport, arr_airport, sizeof((*flights)[i].arrival_airport) - 1);
            (*flights)[i].arrival_airport[sizeof((*flights)[i].arrival_airport) - 1] = '\0';
            free(arr_airport); // Quan trọng: Giải phóng bộ nhớ sau khi dùng
        }

        (*flights)[i].available_economy = (*flights)[i].capacity - 30 - get_ticket_count_by_flight_id((*flights)[i].flight_id, "Economy");
        (*flights)[i].available_business= 20 - get_ticket_count_by_flight_id((*flights)[i].flight_id, "Business");
        (*flights)[i].available_first_class = 10 - get_ticket_count_by_flight_id((*flights)[i].flight_id, "First class");
    }

    mysql_free_result(res);
    disconnect_db(conn);

    return 0;
}

int get_ticket_count_by_flight_id(const char *flight_id, const char *type) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return -1;
    }

    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[1024];
    
    snprintf(query, sizeof(query), 
             "SELECT bd.type, COUNT(*) AS total_tickets "
             "FROM bookings b "
             "JOIN booking_details bd ON b.booking_id = bd.booking_id "
             "WHERE b.flight_id = '%s' and bd.type = '%s' "
             "GROUP BY bd.type;", flight_id, type);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn); // Đừng quên ngắt kết nối nếu lỗi
        return -1;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return -1;
    }
    
    int count = 0;
    int num_rows = mysql_num_rows(res);
    if (num_rows > 0){
        row = mysql_fetch_row(res);
        if (row != NULL && row[1] != NULL) {
            count = atoi(row[1]);
        }
    }

    mysql_free_result(res);
    disconnect_db(conn);
    return count;
}

char** get_seat_codes_by_flight_id(const char *flight_id, int *seat_count) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return NULL;
    }

    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[1024];

    snprintf(query, sizeof(query),
             "SELECT bd.seat_code "
             "FROM flights f "
             "JOIN bookings b ON f.flight_id = b.flight_id "
             "JOIN booking_details bd ON b.booking_id = bd.booking_id "
             "WHERE f.flight_id = '%s';", flight_id);

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

    *seat_count = mysql_num_rows(res);
    if (*seat_count == 0) {
        mysql_free_result(res);
        disconnect_db(conn);
        return NULL; 
    }

    char **seat_codes = malloc(sizeof(char*) * (*seat_count));
    if (seat_codes == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        mysql_free_result(res);
        disconnect_db(conn);
        return NULL;
    }

    int i = 0;
    while ((row = mysql_fetch_row(res)) != NULL) {
        seat_codes[i] = strdup(row[0]);
        if (seat_codes[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for seat_code\n");
            for (int j = 0; j < i; j++) {
                free(seat_codes[j]);
            }
            free(seat_codes);
            mysql_free_result(res);
            disconnect_db(conn);
            return NULL;
        }
        i++;
    }

    mysql_free_result(res);
    disconnect_db(conn);
    return seat_codes;
}