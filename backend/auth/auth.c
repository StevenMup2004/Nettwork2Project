#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/sha.h>
#include <time.h>
#include "../user/user.h"

User *user= NULL;
int countUser = 0;


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

bool checkEmailExist (const char *email){
    if (fetch_users(&user, &countUser) != 0){
        fprintf(stderr, "Failed to fetch users.\n");
        return false;
    }
    for (int i = 0; i < countUser; i++) {
        if (strcmp(user[i].email, email) == 0) {
            printf("Email already in use\n");
            return true;
        }
    }
    free(user);
    return false;
}

bool register_user(const char *email, const char *name, const char *phone, const char *password) {
    if (checkEmailExist(email)){
        return false;
    }
    if (fetch_users(&user, &countUser)!=0) {
        fprintf(stderr, "Failed to fetch users.\n");
        return false;
    }

    char hashed_password[65];
    hash_password_func(password, hashed_password);
    User newUser;
    strcpy(newUser.name, name);
    strcpy(newUser.phone, phone);
    strcpy(newUser.email, email);
    strcpy(newUser.password, hashed_password);
    if (add_user(&newUser) != 0) {
        fprintf(stderr, "Failed to add user.\n");
        return false;
    }

    printf("Register successfully!\n");
    free(user);
    return true;
}

bool login_user(const char *email, const char *password, int *user_id) {
    char hashed_password[65];
    hash_password_func(password, hashed_password);
    if (fetch_users(&user, &countUser) != 0){
        fprintf(stderr, "Failed to fetch users.\n");
        return false;
    }
    for (int i = 0; i < countUser; i++){
        if (strcmp(user[i].email, email) == 0 && strcmp(user[i].password, hashed_password) == 0){
            *user_id = user[i].user_id;
            printf("Login successfully\n");
            return true;
        }
    }
    return false;
}