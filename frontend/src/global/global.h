#ifndef GLOBAL_H
#define GLOBAL_H
#include <gtk/gtk.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <stdbool.h>
#define MAX_LENGTH 4096
#define M_PI 3.14159265358979323846

typedef struct {
    char flight_id[MAX_LENGTH];
    char departure_airport[MAX_LENGTH];
    char arrival_airport[MAX_LENGTH];
    char departure_time[MAX_LENGTH];
    int duration_minutes;
    char airplane_name[MAX_LENGTH];
    int price;
    int capacity;
    int available_economy;
    int available_business;
    int available_first_class;
} Flight;

typedef struct {
    char flight_id[MAX_LENGTH];
    char departure_airport[MAX_LENGTH];
    char arrival_airport[MAX_LENGTH];
    char departure_time[MAX_LENGTH];
    int duration_minutes;
    char airplane_name[MAX_LENGTH];
    int total_price;
    char list_ticket[MAX_LENGTH];
    int booking_id;
} Ticket;

typedef struct {
    int announce_id;
    char flight_id[MAX_LENGTH];
    char content[MAX_LENGTH];
    char updated_at[MAX_LENGTH];
} Announce;

extern GtkWidget *entry_email, *entry_password, *label_status;
extern char buffer[MAX_LENGTH];
extern int sock;
extern GtkWidget *window;
extern GtkWidget *content_area;
extern const char *airports[];
extern const int airport_count;
extern  Flight flights[MAX_LENGTH];
extern  int flight_count;
extern Flight tem_flights[MAX_LENGTH];
extern int tem_flight_count;
extern Flight detail_flight;
extern int seat_count;
extern char **seats_array;
extern int number_seat_order;
extern int price;
extern int user_id;
extern int final_price;
extern char class[30];
extern char **temp_seats;
extern int tem_seats_size;
extern int booking_id;
extern Ticket list_tickets[MAX_LENGTH];
extern Announce list_announces[MAX_LENGTH];
extern int announce_count;
extern Announce filtered_announces[MAX_LENGTH];
extern int filtered_announce_count; 
extern int ticket_count;
extern char email_user[50];
extern char date_tem_flight[50];
extern char airport_from[50];
extern char airport_to[50];
GtkWidget* create_main_window();
void set_content(GtkWidget *new_content);
int is_number(const char *str);
char* format_number_with_separator(int number, char separator);
char* extract_middle_string(const char *input);
void split_date_time(const char *input, char *date, char *time);
bool checkHaveSeat(int available_economy, int available_business, int available_first,const char *type, int number_seat);
void filter_flights(const Flight *flights, int flight_count, Flight *temp_flight, int *temp_count_flight, const char *departure, const char *arrival, const char *date,const char *class,const int number_seat);
int get_seat_position(const char *seat_code, int *i, int *j);
char **add_string_to_array(char **array, int *size, const char *new_string);
char **remove_string_from_array(char **array, int *size, const char *target);
void print_array(char **array, int size);
char *join_strings(char **array, int size, const char *delimiter);
int time_to_seconds(const char *time);
void seconds_to_time(int total_seconds, char *time_str);
char *calculate_end_time(const char *start_time, int duration);
int is_valid_date(const char *date_str);
int get_list_tickets_ordered ();
int filter_announces_by_tickets(Announce *announces, int announce_count, Ticket *tickets, int ticket_count, Announce *filtered_announces);
void vnpay_payment();
void receive_result_from_vnpay();
void generate_next_5_days(const char *input_date, char **days, int *num_days);
char* convert_minutes_to_time_format(int total_minutes);
#endif 