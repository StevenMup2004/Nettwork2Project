#ifndef SERVER_COM_H
#define SERVER_COM_H

#include <stddef.h>
#include "../global/global.h"

int connect_to_server(const char *server_ip, int port);

void send_to_server(int sock, const void *data, size_t size);

void recv_from_server(int sock, void *buffer, size_t size);
void parse_flight_data(char *data, Flight *flights);
int parse_buffer_to_tickets(const char *buffer, Ticket *tickets);
void parse_buffer_to_announces (const char *buffer, Announce *announces);
#endif 
