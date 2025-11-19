#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* get_current_time_now() {

    time_t raw_time = time(NULL);
    if (raw_time == -1) {
        perror("Failed to get current time");
        exit(EXIT_FAILURE);
    }

    struct tm *time_info = localtime(&raw_time);
    if (time_info == NULL) {
        perror("Failed to convert time to local time");
        exit(EXIT_FAILURE);
    }
    char *time_str = malloc(20);
    if (time_str == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", time_info);

    return time_str;
}


const char* get_seat_class(const char *seat_type) {

    if (seat_type[0] == '1') {
        return "First class";
    } else if (seat_type[0] == '2' || seat_type[0] == '3') {
        return "Business";
    } else {
        return "Economy";
    }
}