#ifndef BOOKING_H
#define BOOKING_H

#include "../db/db.h"
#define MAX_LENGTH 255

typedef struct {
    char booking_time[MAX_LENGTH];
    int total_price;
    int user_id;
    char flight_id[MAX_LENGTH];
} Booking;

int insert_booking(const char *booking_time, int total_price, int user_id, const char *flight_id, int *insert_id);
int delete_booking(int booking_id);

#endif 