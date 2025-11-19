#ifndef TICKET_H
#define TICKET_H

#include "../db/db.h"
#define MAX_LENGTH 255

typedef struct {
    char flight_id[MAX_LENGTH];
    char departure_airport[MAX_LENGTH];
    char arrival_airport[MAX_LENGTH];
    char departure_time[MAX_LENGTH];
    int duration_minutes;
    char airplane_name[MAX_LENGTH];
    int total_price;
    char list_ticket[MAX_LENGTH];
    int booking_id;
} Ticket;

char* join_with_dash2(const char* row1, const char* row2, const char* row3);
int fetch_tickets(Ticket **tickets, int *count, int user_id);
char* get_seat_codes_by_booking_id(int *seat_counts,const int booking_id);

#endif 