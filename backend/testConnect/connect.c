#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../booking/booking.h"
int main() {
    int booking_id;
    printf("Enter the booking_id to delete: ");
    scanf("%d", &booking_id);

    int result = delete_booking(booking_id);

    if (result == 0) {
        printf("Booking with ID %d has been successfully deleted.\n", booking_id);
    } else {
        printf("Failed to delete booking with ID %d. Please check the booking ID or database.\n", booking_id);
    }
    return 0;
}