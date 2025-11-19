#ifndef ANNOUNCE_H
#define ANNOUNCE_H

#include "../db/db.h"
#define MAX_LENGTH 255

typedef struct {
    int announce_id;
    char flight_id[MAX_LENGTH];
    char content[MAX_LENGTH];
    char updated_at[MAX_LENGTH];
} Announce;

int fetch_announces(Announce **announces, int *count);

#endif 