CC = gcc

LIBS_CLIENT = -lssl -lcrypto
LIBS_SERVER = -lpthread -lssl -lcrypto
LIBS_CONNECT = -lmysqlclient

SRC_DIR = .
AUTH_DIR = ./auth
DB_DIR = ./db
FLIGHT_DIR = ./flight
TICKET_DIR = ./ticket
USER_DIR = ./user
BOOKING_DIR = ./booking
DETAIL_DIR = ./bookingDetail
TEST_CONNECT_DIR = ./testConnect
ANNOUNCE_DIR = ./announce
UTILS_DIR = ./utils

SERVER_SRC = $(SRC_DIR)/server.c $(AUTH_DIR)/auth.c $(USER_DIR)/user.c $(DB_DIR)/db.c ${FLIGHT_DIR}/flight.c $(TICKET_DIR)/ticket.c $(ANNOUNCE_DIR)/announce.c $(BOOKING_DIR)/booking.c $(UTILS_DIR)/utils.c $(DETAIL_DIR)/detail.c
CONNECT_SRC = $(TEST_CONNECT_DIR)/connect.c $(DB_DIR)/db.c $(BOOKING_DIR)/booking.c

BINS = server connect

all: $(BINS)

server: $(SERVER_SRC)
	$(CC) $(SERVER_SRC) -o $@ $(LIBS_SERVER) $(LIBS_CONNECT)

connect: $(CONNECT_SRC)
	$(CC) $(CONNECT_SRC) -o $@ $(LIBS_CONNECT)

clean:
	rm -f $(BINS) *.o