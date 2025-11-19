#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define MAX_PHONE_LENGTH 15
#define MAX_PASSWORD_LENGTH 65



bool validate_email(const char *email);
bool validate_phone(const char *phone);
bool validate_password(const char *password);
void hash_password_func(const char *password, char *hashed_password);
void get_current_time(char *buffer, size_t size);

#endif