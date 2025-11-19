TARGET = client


CC = gcc
# Added -Wno-deprecated-declarations to CFLAGS to suppress the gtk_widget_override_color warning for now
# Added -D_XOPEN_SOURCE=700 to define necessary features for strptime
CFLAGS = -D_XOPEN_SOURCE=700 `pkg-config --cflags gtk+-3.0` -lssl -lcrypto -lpthread -lm -lcurl -lmicrohttpd `pkg-config --libs gtk+-3.0` -Wno-deprecated-declarations -Wall -Wextra
LDFLAGS =


# Directories containing source files (excluding the current directory '.')
SRC_DIRS = ./register ./auth ./server_com ./login ./homepage ./ticket_detail ./list ./component ./global ./book_seat ./payment ./booklist ./email


# Find all .c files ONLY in specified subdirectories
SRCS_SUBDIRS = $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
# Explicitly define the main source file
MAIN_SRC = client.c


# Create object file list from subdirectory sources
OBJS_SUBDIRS = $(SRCS_SUBDIRS:.c=.o)
# Explicitly define the main object file
MAIN_OBJ = $(MAIN_SRC:.c=.o)


# Combine all object files needed for linking
OBJS = $(MAIN_OBJ) $(OBJS_SUBDIRS)


all: $(TARGET)


# Linker rule: depends on ALL object files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS)


# Rule to compile any .c file into a .o file (including client.c and those in subdirs)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJS) $(TARGET)


# gcc client.c ./register/register.c ./auth/auth.c ./server_com/server_com.c ./login/login.c ./homepage/homepage.c ticket_detail/ticket_detail.c list/list.c ./component/component.c ./global/global.c ./book_seat/book_seat.c ./payment/payment.c ./booklist/booklist.c ./email/email.c -o client `pkg-config --cflags --libs gtk+-3.0` -lssl -lcrypto -lpthread -lm -lcurl -lmicrohttpd