#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "./utils/utils.h"
#include "./auth/auth.h"
#include "./bookingDetail/detail.h"
#include "./flight/flight.h"
#include "./ticket/ticket.h"
#include "./announce/announce.h"
#include "./booking/booking.h"

#define PORT 8080
#define BUFFER_SIZE 1024
char *token;
Flight *flights = NULL;
Ticket *tickets = NULL;
Announce *announces = NULL;
int count_announce = 0;
int count_ticket = 0;
int count_flight = 0;
int seat_count = 0;
int user_id = 0;
int total_price = 0;
char flight_id[BUFFER_SIZE];
char seat_list[BUFFER_SIZE];
int booking_id = 0;
char **seats;
char seat_code[10]; 


void format_tickets_to_string(Ticket *tickets, int count_ticket, char *output_buffer) {
    output_buffer[0] = '\0'; 
    int BUFFER_SIZE_MAX = 4096;
    for (int i = 0; i < count_ticket; i++) {
        char line[BUFFER_SIZE_MAX];
        snprintf(line, BUFFER_SIZE_MAX, "%d\n%s\n%s\n%d\n%s\n%s\n%s\n%d\n%s\n",
                 tickets[i].booking_id,
                 tickets[i].flight_id,
                 tickets[i].departure_time,
                 tickets[i].duration_minutes,
                 tickets[i].airplane_name,
                 tickets[i].departure_airport,
                 tickets[i].arrival_airport,
                 tickets[i].total_price,
                 tickets[i].list_ticket);
        strncat(output_buffer, line, BUFFER_SIZE_MAX - strlen(output_buffer) - 1);
    }
}

void send_announces(Announce *announces, int count_announce, char *output_buffer){
    output_buffer[0] = '\0'; 
    int BUFFER_SIZE_MAX = 4096;
    for (int i = 0; i < count_announce; i++) {
        char line[BUFFER_SIZE_MAX];
        snprintf(line, BUFFER_SIZE_MAX, "%d\n%s\n%s\n%s\n",
                 announces[i].announce_id,
                 announces[i].flight_id,
                 announces[i].content,
                 announces[i].updated_at);
        strncat(output_buffer, line, BUFFER_SIZE_MAX - strlen(output_buffer) - 1);
    }
}
void send_flights(int client_sock, Flight *flights, int count) {
    char buffer[65536]; 
    int offset = 0;  
    for (int i = 0; i < count; i++) {
        int bytes_written = snprintf(buffer + offset, sizeof(buffer) - offset,
                                     "Id %s\nTime %s\nDuration %d\nCapacity %d\nPrice %d\nName %s\nDeparture %s\nArrival %s\nEconomy %d\nBusiness %d\nFirst %d\n\n",
                                     flights[i].flight_id, flights[i].departure_time, flights[i].duration_minutes, flights[i].capacity, flights[i].price,
                                     flights[i].airplane_name, flights[i].departure_airport, flights[i].arrival_airport,
                                     flights[i].available_economy, flights[i].available_business, flights[i].available_first_class);

        if (bytes_written < 0 || offset + bytes_written >= sizeof(buffer)) {
            fprintf(stderr, "Buffer overflow or formatting error!\n");
            return; 
        }

        offset += bytes_written; 
    }

    int bytes_sent = send(client_sock, buffer, offset, 0);
    if (bytes_sent < 0) {
        perror("Failed to send flights data");
    } else {
        printf("Sent %d bytes of flight data.\n", bytes_sent);
    }
}


int calculate_checksum(const char *data) {
    int checksum = 0;
    while (*data) {
        checksum += *data++;
    }
    return checksum;
}


char* returnMsg (char* message){
    const char *header = "RESPONSE_HEADER";
    int checksum = calculate_checksum(message);

    char *response = (char*)malloc(BUFFER_SIZE);
    if (response == NULL) {
        return NULL;
    }
    snprintf(response, BUFFER_SIZE, "%s|%s|%d", header, message, checksum);
    return response; 
}

void *handle_client(void *client_socket) {
    bool is_logged_in = false;
    int user_id = 0;
    int option=10;
    char int_str[12];
    char email[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    char phone[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    int sock = *(int*)client_socket;
    char buffer[BUFFER_SIZE];
    int read_size;

    while (1) {
        if (!is_logged_in){
           recv(sock, buffer, sizeof(buffer), 0);
           buffer[strcspn(buffer, "\n")] = 0;
           if (strncmp(buffer, "REGISTER", strlen("REGISTER")) == 0){
                printf("%s\n", buffer);      
                token = strtok(buffer, " ");
    
                token = strtok(NULL, ":");
                strcpy(name, token);

                token = strtok(NULL, ":");
                strcpy(phone, token);
                
                token = strtok(NULL, ":");
                strcpy(email, token);
                
                token = strtok(NULL, ":");
                strcpy(password, token);
                
                printf("Check name: %s\n", name);
                printf("Check phone: %s\n", phone);
                printf("Check email: %s\n", email);
                printf("Check password: %s\n", password);
                if (checkEmailExist(email)){
                    send(sock, "EMAIL_EXISTED", strlen("EMAIL_EXISTED") + 1, 0);
                }else {
                    if (register_user(email, name, phone, password)){
                        send(sock, "SUCCESS", strlen("SUCCESS") + 1, 0);
                    }
                    else send(sock, "FAILED", strlen("FAILED") + 1, 0);
                }
            } else if (strncmp(buffer, "LOGIN", strlen("LOGIN")) == 0){
                printf("%s\n", buffer);      
                token = strtok(buffer, " ");
                
                token = strtok(NULL, ":");
                strcpy(email, token);
                
                token = strtok(NULL, ":");
                strcpy(password, token);

                printf("Check email: %s\n", email);
                printf("Check password: %s\n", password);
                if (!checkEmailExist(email)){
                    send(sock, "EMAIL_NOT_FOUND", strlen("EMAIL_NOT_FOUND") + 1, 0);
                }else {
                    if (login_user(email, password, &user_id)){
                        printf("Check user id: %d\n", user_id);
                        char msg_with_id[256];
                        snprintf(msg_with_id, sizeof(msg_with_id), "SUCCESS:%d", user_id);
                        send(sock, msg_with_id, strlen(msg_with_id) + 1, 0);  
                        is_logged_in = true;
                    }
                    else send(sock, "FAILED", strlen("FAILED") + 1, 0);
                }
            }
        }
        else {
            memset(buffer, 0, BUFFER_SIZE);
            recv(sock, buffer, sizeof(buffer), 0);
            buffer[strcspn(buffer, "\n")] = 0;
            if (strncmp(buffer, "GET FLIGHTS", strlen("GET FLIGHTS")) == 0){
                if (fetch_flights(&flights, &count_flight) != 0) {
                    fprintf(stderr, "Failed to fetch flights.\n");
                }
                send(sock, &count_flight, sizeof(count_flight), 0);
                send_flights(sock, flights, count_flight);
            }
            else if (strncmp(buffer, "GET ORDERED SEATS", strlen("GET ORDERED SEATS")) == 0){
                int ret = sscanf(buffer, "GET ORDERED SEATS %s", seat_code);
                if (ret == 1) {
                    printf("Received seat code: %s\n", seat_code);
                    char **seats = get_seat_codes_by_flight_id(seat_code, &seat_count);
                    if (seats!= NULL) {
                        char response[4096]; 
                        int written = snprintf(response, sizeof(response), "SEAT COUNT: %d\n", seat_count);
                        if (written < 0) {
                            perror("Error writing seat count");
                            return 0;
                        }
                        send(sock, response, written, 0);  
                        printf("Check\n");
                        int total_length = 0;
                        char *send_message = NULL;
                        const char *delimiter = ","; 
                        for (int i = 0; i < seat_count; i++) {
                            total_length += strlen(seats[i]) + strlen(delimiter);
                        }
                        send_message = malloc(total_length + 1);
                        if (!send_message) {
                            perror("Memory allocation failed");
                            return 0;
                        }
                        send_message[0] = '\0'; 
                        for (int i = 0; i < seat_count; i++) {
                            strcat(send_message, seats[i]);
                            if (i < seat_count - 1) {
                                strcat(send_message, delimiter);
                            }
                            free(seats[i]);
                        }
                        free(seats); 
                        send(sock, send_message, strlen(send_message), 0);
                        printf("Sent seats: %s\n", send_message);
                        free(send_message);
                        printf("Sent seats\n");
                    }
                    else {
                        printf("No Sent\n");
                        send(sock,"NOTHING", strlen("NOTHING") + 1, 0);
                    }
                } else {
                    printf("Invalid format\n");
                }
            }
            else if (sscanf(buffer, "GET LIST TICKETS: %d", &user_id) == 1) {
                printf("User id received: %d\n", user_id);
                if (fetch_tickets(&tickets, &count_ticket, user_id) != 0) {
                    fprintf(stderr, "Failed to fetch tickets.\n");
                }
                printf("Number tickets: %d\n", count_ticket);
                send(sock, &count_ticket, sizeof(count_ticket), 0);
                if (count_ticket == 0){
                    send(sock, "NO TICKETS", strlen("NO TICKETS") + 1, 0);
                }
                else {
                    format_tickets_to_string(tickets, count_ticket, buffer);
                    printf("Send to client: %s\n", buffer);
                    send(sock, buffer, strlen(buffer), 0);
                }
            }
            else if (strncmp(buffer, "GET ANNOUNCES", strlen("GET ANNOUNCES")) == 0){
                 if (fetch_announces(&announces, &count_announce) != 0) {
                    fprintf(stderr, "Failed to fetch announces.\n");
                }
                printf("Number tickets: %d\n", count_announce);
                send(sock, &count_announce, sizeof(count_announce), 0);
                if (count_announce == 0){
                    send(sock, "NO ANNOUNCES", strlen("NO ANNOUNCES") + 1, 0);
                }
                else {
                    send_announces(announces, count_announce, buffer);
                    printf("Send to client list of announces: %s\n", buffer);
                    send(sock, buffer, strlen(buffer), 0);
                }
            }
            else if (strncmp(buffer, "DELETE BOOKING", strlen("DELETE BOOKING")) == 0){
                    if (sscanf(buffer, "DELETE BOOKING: %d", &booking_id) == 1) {
                        printf("Booking id received: %d\n", booking_id);
                        int result = delete_booking(booking_id);
                        if (result == 0) {
                            send(sock, "SUCCESS", strlen("SUCCESS")  + 1, 0);
                        } else {
                            send(sock, "FAILED", strlen("FAILED") + 1, 0);
                        }
                }
            }
             else if (strncmp(buffer, "CREATE NEW BOOKING", strlen("CREATE NEW BOOKING")) == 0){
                if (sscanf(buffer, "CREATE NEW BOOKING %d %d %s", &total_price, &user_id, flight_id) == 3) {
                    printf("Parsed data: total_price=%d, user_id=%d, flight_id=%s\n", total_price, user_id, flight_id);
                    int result = insert_booking(get_current_time_now(), total_price, user_id, flight_id, &booking_id);
                     if (result == 0) {
                        snprintf(buffer, sizeof(buffer), "SUCCESS BOOKING_ID: %d", booking_id);
                        send(sock, buffer, strlen(buffer), 0);
                    } else {
                        send(sock, "FAILED TO CREATE BOOKING", strlen("FAILED TO CREATE BOOKING"), 0);
                    }
                }
             }
             else if (strncmp(buffer, "CREATE DETAIL BOOKING", strlen("CREATE DETAIL BOOKING")) == 0){
                  if (sscanf(buffer + 22, "%d %s", &booking_id, seat_list) == 2) {
                    printf("Booking ID: %d\n", booking_id);
                    printf("Seat List: %s\n", seat_list);
                     char *seat = strtok(seat_list, ",");
                    while (seat != NULL) {
                        printf("Seat: %s\n", seat);

                        if (insert_booking_detail(booking_id, get_seat_class(seat), seat) == 0) {
                            printf("Inserted seat %s successfully.\n", seat);
                        } else {
                            printf("Failed to insert seat %s.\n", seat);
                        }

                        seat = strtok(NULL, ",");
                    }

                    send(sock, "SUCCESS", strlen("SUCCESS"), 0);
                  }
                  else send(sock, "FAILED TO PARSE BOOKING DETAILS", strlen("FAILED TO PARSE BOOKING DETAILS"), 0);
             }
        }
    }

    close(sock);
    free(client_socket);
    return 0;
}

int main() {
    int server_socket, client_socket, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t thread_id, notification_thread;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 3);
    printf("Waiting for connections...\n");

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size))) {
        new_sock = malloc(sizeof(int)); // <--- SỬA THÀNH sizeof(int)
        if (new_sock == NULL) {
            perror("Failed to allocate memory for new socket");
            continue;
        }
        *new_sock = client_socket;
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }
    }

    close(server_socket);
    return 0;
}
