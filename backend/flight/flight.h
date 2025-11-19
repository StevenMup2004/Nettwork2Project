#ifndef FLIGHT_H
#define FLIGHT_H

#include "../db/db.h"
#define MAX_LENGTH 255

typedef struct {
    char flight_id[MAX_LENGTH];
    char departure_airport[MAX_LENGTH];
    char arrival_airport[MAX_LENGTH];
    char departure_time[MAX_LENGTH];
    int duration_minutes;
    char airplane_name[MAX_LENGTH];
    int price;
    int capacity;
    int available_economy;
    int available_business;
    int available_first_class;
} Flight;

int fetch_flights(Flight **flights, int *count);
char* join_with_dash(const char* row1, const char* row2, const char* row3);
int get_ticket_count_by_flight_id(const char *flight_id, const char *type);
char** get_seat_codes_by_flight_id(const char *flight_id, int *seat_count);
#endif 