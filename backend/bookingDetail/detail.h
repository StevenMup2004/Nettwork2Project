#ifndef DETAIL_H
#define DETAIL_H

#include "../db/db.h"
#define MAX_LENGTH 255

typedef struct {
    int booking_id;
    char type[MAX_LENGTH];
    char seat_code[MAX_LENGTH];
} Detail;

int insert_booking_detail(int booking_id, const char *type, const char *seat_code);

#endif 