#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/sha.h>
#include <time.h>

void hash_password_func(const char *password, char *hashed_password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)password, strlen(password), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashed_password + (i * 2), "%02x", hash[i]);
    }
    hashed_password[64] = '\0';
}

void get_current_time(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

bool validate_email(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    return at && dot && at < dot;
}

bool validate_phone(const char *phone) {
    if (strlen(phone) < 10 || strlen(phone) > 15) return false;
    for (int i = 0; i < strlen(phone); i++) {
        if (!isdigit(phone[i])) return false;
    }
    return true;
}

bool validate_password(const char *password) {
    bool has_upper = false, has_lower = false, has_digit = false;
    if (strlen(password) < 8) return false;
    for (int i = 0; i < strlen(password); i++) {
        if (isupper(password[i])) has_upper = true;
        if (islower(password[i])) has_lower = true;
        if (isdigit(password[i])) has_digit = true;
    }
    return has_upper && has_lower && has_digit;
}
