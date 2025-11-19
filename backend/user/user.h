#ifndef USER_H
#define USER_H

#include <stdbool.h>
#include "../db/db.h"
#define MAX_LENGTH 255

typedef struct {
    int user_id;
    char name[MAX_LENGTH];
    char phone[MAX_LENGTH];
    char email[MAX_LENGTH];
    char password[MAX_LENGTH];
} User;

int fetch_users (User **users, int *count);
int add_user(const User *user);

#endif