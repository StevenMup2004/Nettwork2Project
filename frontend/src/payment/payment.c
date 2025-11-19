#include "../payment/payment.h"
#include "../server_com/server_com.h"
#include "../book_seat/book_seat.h"
#include <cairo.h>
#include <math.h>
#include "../global/global.h"
#include "../booklist/booklist.h"
const char *gate_code= "22";

int selected_voucher = -1;
double discounted_price = 0;
char error_message[256] = "";
time_t error_start_time = 0;

double button_y_payment, cancel_button_x, confirm_button_x;

// Function to calculate discounted price
double calculate_discounted_price(int voucher_index) {
    if (voucher_index == -1) return price;

    switch (voucher_index) {
        case 0: return price - 200000; // Welcome Voucher
        case 1: return price * 0.75;  // 25% OFF
        case 2: {
            // Group Travel Discount: Requires at least 3 seats
            if (tem_seats_size < 3)
               return price;
            return price * 0.8; // 20% OFF
        }
        case 3: return price * 0.9; // 10% OFF
        default: return price;
    }
}


// Draw error popup
double popup_close_button_x, popup_close_button_y, popup_close_button_size = 24;

void draw_error_popup(cairo_t *cr, gint screen_width, gint screen_height) {
    if (error_message[0] && difftime(time(NULL), error_start_time) < 20) {
        double popup_width = 400;
        double popup_height = 120;
        double popup_x = (screen_width - popup_width) / 2;
        double popup_y = (screen_height - popup_height) / 2;

        // Background
        cairo_set_source_rgb(cr, 34.0 / 255.0, 58.0 / 255.0, 96.0 / 255.0);
        cairo_new_path(cr);
        cairo_arc(cr, popup_x + 24, popup_y + 24, 24, M_PI, 3 * M_PI / 2);
        cairo_arc(cr, popup_x + popup_width - 24, popup_y + 24, 24, 3 * M_PI / 2, 2 * M_PI);
        cairo_arc(cr, popup_x + popup_width - 24, popup_y + popup_height - 24, 24, 0, M_PI / 2);
        cairo_arc(cr, popup_x + 24, popup_y + popup_height - 24, 24, M_PI / 2, M_PI);
        cairo_close_path(cr);
        cairo_fill(cr);

        // Text
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 18);

        // First line
        cairo_text_extents_t text_extents;
        cairo_text_extents(cr, "You must purchase", &text_extents);
        cairo_move_to(cr, popup_x + (popup_width - text_extents.width) / 2, popup_y + 40);
        cairo_show_text(cr, "You must purchase");

        // Second line
        cairo_text_extents(cr, "at least 3 tickets", &text_extents);
        cairo_move_to(cr, popup_x + (popup_width - text_extents.width) / 2, popup_y + 65);
        cairo_show_text(cr, "at least 3 tickets");

        // Third line
        cairo_text_extents(cr, "to apply this voucher.", &text_extents);
        cairo_move_to(cr, popup_x + (popup_width - text_extents.width) / 2, popup_y + 90);
        cairo_show_text(cr, "to apply this voucher.");

        // Close button
        double close_circle_radius = 12;
        popup_close_button_x = popup_x + popup_width - 20 - close_circle_radius * 2;
        popup_close_button_y = popup_y + 20;

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_arc(cr, popup_close_button_x + close_circle_radius, popup_close_button_y + close_circle_radius, close_circle_radius, 0, 2 * M_PI);
        cairo_fill(cr);

        double x_thickness = 3.0, x_length = 10.0;
        cairo_set_source_rgb(cr, 34.0 / 255.0, 58.0 / 255.0, 96.0 / 255.0);
        cairo_set_line_width(cr, x_thickness);

        // X lines
        cairo_move_to(cr, popup_close_button_x + close_circle_radius - x_length / 2, popup_close_button_y + close_circle_radius - x_length / 2);
        cairo_line_to(cr, popup_close_button_x + close_circle_radius + x_length / 2, popup_close_button_y + close_circle_radius + x_length / 2);
        cairo_move_to(cr, popup_close_button_x + close_circle_radius + x_length / 2, popup_close_button_y + close_circle_radius - x_length / 2);
        cairo_line_to(cr, popup_close_button_x + close_circle_radius - x_length / 2, popup_close_button_y + close_circle_radius + x_length / 2);
        cairo_stroke(cr);
    }
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    gint screen_width = gtk_widget_get_allocated_width(widget);
    gint screen_height = gtk_widget_get_allocated_height(widget);
    if (event->x >= cancel_button_x && event->x <= cancel_button_x + 156 &&
        event->y >= button_y_payment && event->y <= button_y_payment + 56) {
        printf("Cancel button clicked!\n");
        GtkWidget *book_seat_window = create_book_seat_window();
        set_content(book_seat_window);
        return true;
    }

    if (event->x >= confirm_button_x && event->x <= confirm_button_x + 156 &&
        event->y >= button_y_payment && event->y <= button_y_payment + 56) {
        // printf("Confirm button clicked!\n");
        // int result = get_list_tickets_ordered();
        // if (result == -1){
        //     printf("Error when fetching tickets\n");
        //     return false;
        // }
        final_price = discounted_price;

        vnpay_payment();
        receive_result_from_vnpay();
        return true;
        }

    if (error_message[0] &&
        event->x >= popup_close_button_x &&
        event->x <= popup_close_button_x + popup_close_button_size &&
        event->y >= popup_close_button_y &&
        event->y <= popup_close_button_y + popup_close_button_size) {
        error_message[0] = '\0';
        selected_voucher = -1;
        discounted_price = price;
        gtk_widget_queue_draw(widget);
        return TRUE;
    }

    double voucher_x = screen_width / 2 - 500 + 50;
    double voucher_y = screen_height / 2 - 300 + 100;

    for (int i = 0; i < 4; i++) {
        if (event->x >= voucher_x && event->x <= voucher_x + 400 &&
            event->y >= voucher_y && event->y <= voucher_y + 104) {
            selected_voucher = i;
            discounted_price = calculate_discounted_price(i);

            if (selected_voucher != 2 || discounted_price != price) {
                error_message[0] = '\0';
            }

            gtk_widget_queue_draw(widget);
            return TRUE;
        }
        voucher_y += 120;
    }

    return FALSE;
}

static gboolean on_payment_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    char time[20];
    char date[20];
    split_date_time(detail_flight.departure_time, date, time);
    GdkPixbuf *bg_pixbuf;
    GdkPixbuf *scaled_pixbuf;

    gint screen_width = gtk_widget_get_allocated_width(widget);
    gint screen_height = gtk_widget_get_allocated_height(widget);

    
    bg_pixbuf = gdk_pixbuf_new_from_file("../assets/images/bg_login.png", NULL);
    if (!bg_pixbuf) {
        g_print("Error loading image!\n");
        return FALSE;
    }

    scaled_pixbuf = gdk_pixbuf_scale_simple(bg_pixbuf, screen_width, screen_height, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
    cairo_paint(cr);
    g_object_unref(scaled_pixbuf);
    g_object_unref(bg_pixbuf);

    
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_new_path(cr);
    cairo_arc(cr, screen_width / 2 - 500 + 24, screen_height / 2 - 300 + 24, 24, M_PI, 3 * M_PI / 2);
    cairo_arc(cr, screen_width / 2 + 500 - 24, screen_height / 2 - 300 + 24, 24, 3 * M_PI / 2, 2 * M_PI);
    cairo_arc(cr, screen_width / 2 + 500 - 24, screen_height / 2 + 300 - 24, 24, 0, M_PI / 2);
    cairo_arc(cr, screen_width / 2 - 500 + 24, screen_height / 2 + 300 - 24, 24, M_PI / 2, M_PI);
    cairo_close_path(cr);
    cairo_fill_preserve(cr);

    
    cairo_set_source_rgb(cr, 34.0 / 255.0, 58.0 / 255.0, 96.0 / 255.0); 
    cairo_new_path(cr);

    
    cairo_arc(cr, screen_width / 2 - 500 + 24, screen_height / 2 - 300 + 24, 24, M_PI, 3 * M_PI / 2);

    
    cairo_line_to(cr, screen_width / 2, screen_height / 2 - 300);

    
    cairo_line_to(cr, screen_width / 2, screen_height / 2 + 300);

    
    cairo_arc(cr, screen_width / 2 - 500 + 24, screen_height / 2 + 300 - 24, 24, M_PI / 2, M_PI);

    
    cairo_line_to(cr, screen_width / 2 - 500, screen_height / 2 - 300 + 24);

    cairo_close_path(cr); 
    cairo_fill(cr); 


    
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 30);
    cairo_text_extents_t vouchers_label_extents;
    cairo_text_extents(cr, "MY VOUCHERS", &vouchers_label_extents);
    cairo_move_to(cr, screen_width / 2 - 500 + (500 - vouchers_label_extents.width) / 2, screen_height / 2 - 300 + 61);
    cairo_show_text(cr, "MY VOUCHERS");

    
    double voucher_x = (screen_width - 1000) / 2 + (500 - 400) / 2;
    double voucher_y = screen_height / 2 - 300 + 100; 
        for (int i = 0; i < 4; i++) {
        // Highlight selected voucher
        if (i == selected_voucher) {
            cairo_set_source_rgb(cr, 0.55, 0.93, 0.93); // Highlight color #8DECEC
        } else {
            cairo_set_source_rgb(cr, 0.98, 0.98, 0.98); // Default color #FBFBFB
        }

        // Vẽ nền voucher
        cairo_new_path(cr);
        cairo_arc(cr, voucher_x + 8, voucher_y + 8, 8, M_PI, 3 * M_PI / 2);
        cairo_line_to(cr, voucher_x + 400 - 8, voucher_y);
        cairo_arc(cr, voucher_x + 400 - 8, voucher_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
        cairo_line_to(cr, voucher_x + 400, voucher_y + 104 - 8);
        cairo_arc(cr, voucher_x + 400 - 8, voucher_y + 104 - 8, 8, 0, M_PI / 2);
        cairo_line_to(cr, voucher_x + 8, voucher_y + 104);
        cairo_arc(cr, voucher_x + 8, voucher_y + 104 - 8, 8, M_PI / 2, M_PI);
        cairo_close_path(cr); // Kết thúc đường vẽ

        // Tô nền voucher
        cairo_fill_preserve(cr);

        
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.25); 
        cairo_stroke(cr);

        // Vẽ đường kẻ dọc nét đứt
        cairo_set_dash(cr, (double[]){6.0, 4.0}, 2, 0.0);
        cairo_move_to(cr, voucher_x + 124, voucher_y + 8.55);
        cairo_line_to(cr, voucher_x + 124, voucher_y + 95.45);
        cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 0.2); 
        cairo_stroke(cr);
        cairo_set_dash(cr, NULL, 0, 0);

        
        char voucher_image[256];
        snprintf(voucher_image, sizeof(voucher_image), "../assets/images/voucher%d.png", i + 1);
        GdkPixbuf *voucher_icon = gdk_pixbuf_new_from_file(voucher_image, NULL);
        if (voucher_icon) {
            GdkPixbuf *scaled_voucher_icon = gdk_pixbuf_scale_simple(voucher_icon, 55, 55, GDK_INTERP_BILINEAR);
            gdk_cairo_set_source_pixbuf(cr, scaled_voucher_icon, voucher_x + 34.5, voucher_y + 24.5);
            cairo_paint(cr);
            g_object_unref(scaled_voucher_icon);
            g_object_unref(voucher_icon);
        }

        
        const char *voucher_texts[4][3] = {
            {"200.000 VND", "Welcome Voucher", "Valid until 01 February 2025"},
            {"25% OFF", "VNPAY Payment Voucher", "Valid until 03 March 2025"},
            {"20% OFF", "Group Travel Discount", "Valid until 05 March 2025"},
            {"10% OFF", "Over 1 Million Discount", "Valid until 09 March 2025"}};

        
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); 
        cairo_select_font_face(cr, "Montserrat", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 24);
        cairo_move_to(cr, voucher_x + 140, voucher_y + 35);
        cairo_show_text(cr, voucher_texts[i][0]);

        
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); 
        cairo_select_font_face(cr, "Montserrat", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 16);
        cairo_move_to(cr, voucher_x + 140, voucher_y + 65);
        cairo_show_text(cr, voucher_texts[i][1]);

        
        cairo_set_source_rgb(cr, 0.33, 0.33, 0.33); 
        cairo_select_font_face(cr, "Montserrat", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 10);
        cairo_move_to(cr, voucher_x + 140, voucher_y + 90);
        cairo_show_text(cr, voucher_texts[i][2]);

        voucher_y += 120; 
    }
    
    GdkPixbuf *logo_pixbuf = gdk_pixbuf_new_from_file("../assets/images/logovn.png", NULL);
    if (logo_pixbuf) {
        GdkPixbuf *scaled_logo_pixbuf = gdk_pixbuf_scale_simple(logo_pixbuf, 120, 120, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_logo_pixbuf, screen_width / 2 + 250 - 60, screen_height / 2 - 300 + 20);
        cairo_paint(cr);
        g_object_unref(scaled_logo_pixbuf);
        g_object_unref(logo_pixbuf);
    }

const char *flight_labels[4] = {"Flight", "Gate", "Seat", "Class"};
const char *flight_info[4] = {detail_flight.flight_id, gate_code, join_strings(temp_seats, tem_seats_size, ", "), class};


int label_font_size = 18;
int info_font_size = 18;


double column_spacing = 110;


double right_half_start_x = screen_width / 2; 
double right_half_width = 500; 
double content_width = 3 * column_spacing; 
double base_x = right_half_start_x + (right_half_width - content_width) / 2;


double base_y = screen_height / 2 - 300 + 350; 
double row_spacing = 30; 


for (int i = 0; i < 4; i++) {
    
    double column_x = base_x + i * column_spacing;

    
    cairo_text_extents_t label_extents, info_extents;

    
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, label_font_size);
    cairo_text_extents(cr, flight_labels[i], &label_extents);

    
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, info_font_size);
    cairo_text_extents(cr, flight_info[i], &info_extents);

    
    double label_y = base_y;
    double info_y = base_y + row_spacing + (label_extents.height - info_extents.height) / 2;

    
    cairo_set_source_rgb(cr, 0.33, 0.33, 0.33); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, label_font_size);
    cairo_move_to(cr, column_x - label_extents.width / 2, label_y);
    cairo_show_text(cr, flight_labels[i]);

    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, info_font_size);
    cairo_move_to(cr, column_x - info_extents.width / 2, info_y);
    cairo_show_text(cr, flight_info[i]);
}



double right_half_center_x = screen_width / 2 + 500 / 2; 
double flight_icon_y = screen_height / 2 - 300 + 180;    



cairo_text_extents_t departure_city_extents, arrival_city_extents;
cairo_text_extents(cr, extract_middle_string(detail_flight.departure_airport), &departure_city_extents);
cairo_text_extents(cr,  extract_middle_string(detail_flight.arrival_airport), &arrival_city_extents);


double flight_icon_x = right_half_center_x - 156 / 2; 
double city_spacing = 20; 


double departure_city_x = flight_icon_x - city_spacing - departure_city_extents.width + 5;
double departure_city_y = flight_icon_y;


double arrival_city_x = flight_icon_x + 156 + city_spacing -5;
double arrival_city_y = flight_icon_y;


cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
cairo_set_font_size(cr, 20);
cairo_move_to(cr, departure_city_x, departure_city_y);
if (detail_flight.departure_airport) cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));


cairo_move_to(cr, arrival_city_x, arrival_city_y);
if (detail_flight.arrival_airport) cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));


cairo_text_extents_t departure_airport_extents, arrival_airport_extents;
cairo_text_extents(cr, extract_middle_string(detail_flight.departure_airport), &departure_airport_extents);
cairo_text_extents(cr, extract_middle_string(detail_flight.arrival_airport), &arrival_airport_extents);

double departure_airport_y = departure_city_y + (departure_city_extents.height + 20); 
double arrival_airport_y = arrival_city_y + (arrival_city_extents.height + 20); 


cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
cairo_set_font_size(cr, 18);
cairo_move_to(cr, departure_city_x + departure_city_extents.width / 2 - departure_airport_extents.width / 2, departure_airport_y);
if (detail_flight.departure_airport) cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));


cairo_move_to(cr, arrival_city_x + arrival_city_extents.width / 2 - arrival_airport_extents.width / 2, arrival_airport_y);
if (detail_flight.arrival_airport) cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));


GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
if (flight_icon) {
    GdkPixbuf *scaled_flight_icon = gdk_pixbuf_scale_simple(flight_icon, 156, 36, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, scaled_flight_icon, flight_icon_x, flight_icon_y - 18);
    cairo_paint(cr);
    g_object_unref(scaled_flight_icon);
    g_object_unref(flight_icon);
}



double date_x = screen_width / 2 + (500 - 400) / 2 + 50;


GdkPixbuf *calendar_icon = gdk_pixbuf_new_from_file("../assets/images/Calendar.png", NULL);
if (calendar_icon) {
    GdkPixbuf *scaled_calendar_icon = gdk_pixbuf_scale_simple(calendar_icon, 20, 20, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, scaled_calendar_icon, date_x + 8, screen_height / 2 - 300 + 257 + (46 - 20) / 2);
    cairo_paint(cr);
    g_object_unref(scaled_calendar_icon);
    g_object_unref(calendar_icon);
}


cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
cairo_set_font_size(cr, 18);
cairo_text_extents_t date_text_extents;
cairo_text_extents(cr, date, &date_text_extents);
double date_text_y = screen_height / 2 - 300 + 270 + (46 - date_text_extents.height) / 2;
cairo_move_to(cr, date_x + 20 + 8 + 10, date_text_y);
cairo_show_text(cr,date);


double time_x = screen_width / 2 + (500 - 400) / 2 + 230;


GdkPixbuf *clock_icon = gdk_pixbuf_new_from_file("../assets/images/Clock.png", NULL);
if (clock_icon) {
    GdkPixbuf *scaled_clock_icon = gdk_pixbuf_scale_simple(clock_icon, 20, 20, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, scaled_clock_icon, time_x + 8, screen_height / 2 - 300 + 257 + (46 - 20) / 2);
    cairo_paint(cr);
    g_object_unref(scaled_clock_icon);
    g_object_unref(clock_icon);
}


cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
cairo_set_font_size(cr, 18);
cairo_text_extents_t time_text_extents;
cairo_text_extents(cr, time, &time_text_extents);
double time_text_y = screen_height / 2 - 300 + 270 + (46 - time_text_extents.height) / 2;
cairo_move_to(cr, time_x + 20 + 8 + 10, time_text_y);
cairo_show_text(cr, time);


    
    

// Define the text for Price and its value
const char *price_label = "Price";

// Calculate the total width of the Price label and the old price group
cairo_text_extents_t price_label_extents, price_value_extents, new_price_extents;
cairo_set_font_size(cr, 18);
cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
cairo_text_extents(cr, price_label, &price_label_extents);

cairo_set_font_size(cr, 24);
cairo_text_extents(cr, format_number_with_separator(price, ','), &price_value_extents);

// Calculate the width for "Price" label + old price
double total_width = price_label_extents.width + 10 + price_value_extents.width; // 10px gap between label and value

// Center alignment for the group (Price label + old price) in the right half
double base1_x = screen_width / 2 + 500 - (500 / 2) - (total_width / 2);
double base1_y = base_y + 100;// Vertical alignment

// Draw "Price" label
cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); // Black color
cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
cairo_set_font_size(cr, 18);
cairo_move_to(cr, base1_x, base1_y);
cairo_show_text(cr, price_label);

// Draw old price
double old_price_x = base1_x + price_label_extents.width + 10;
cairo_set_font_size(cr, 24); // Font size for old price
if (selected_voucher == -1 || (selected_voucher == 2 && error_message[0])) {
    // Display only the original price without a strikethrough
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); // Black color
    cairo_move_to(cr, old_price_x, base1_y);
    cairo_show_text(cr, format_number_with_separator(price, ','));
} else {
    // Display old price with a strikethrough
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6); // Gray color
    cairo_move_to(cr, old_price_x, base1_y);
    cairo_show_text(cr, format_number_with_separator(price, ','));

    // Draw strikethrough line for old price
    cairo_text_extents(cr, format_number_with_separator(price, ','), &price_value_extents);
    double strikethrough_start_x = old_price_x;
    double strikethrough_end_x = old_price_x + price_value_extents.width;
    double strikethrough_y = base1_y - price_value_extents.height / 2;
    cairo_move_to(cr, strikethrough_start_x, strikethrough_y);
    cairo_line_to(cr, strikethrough_end_x, strikethrough_y);
    cairo_stroke(cr);

    // Draw new price below the old price
    char new_price[64];
    snprintf(new_price, sizeof(new_price), "%s VND", format_number_with_separator(discounted_price, ','));
    cairo_text_extents(cr, new_price, &new_price_extents);

    double new_price_x = old_price_x + (price_value_extents.width - new_price_extents.width) / 2;
    double new_price_y = base1_y + 40; // 40px below the old price
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); // Black color for new price
    cairo_move_to(cr, new_price_x, new_price_y);
    cairo_show_text(cr, new_price);
}

    // Draw error popup if necessary
    draw_error_popup(cr, screen_width, screen_height);


    
    button_y_payment = screen_height / 2 + 300 - 80;
    cancel_button_x = screen_width / 2 + 250 - 180 - 10;
    confirm_button_x = screen_width / 2 + 250 + 10;

    
    cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
    cairo_new_path(cr);
    cairo_arc(cr, cancel_button_x + 8, button_y_payment + 8, 8, M_PI, 3 * M_PI / 2); 
    cairo_arc(cr, cancel_button_x + 156 - 8, button_y_payment + 8, 8, 3 * M_PI / 2, 2 * M_PI); 
    cairo_arc(cr, cancel_button_x + 156 - 8, button_y_payment + 56 - 8, 8, 0, M_PI / 2); 
    cairo_arc(cr, cancel_button_x + 8, button_y_payment + 56 - 8, 8, M_PI / 2, M_PI); 
    cairo_close_path(cr);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
    cairo_stroke(cr);

    
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18);
    cairo_text_extents_t cancel_extents;
    cairo_text_extents(cr, "Cancel", &cancel_extents);
    cairo_move_to(
        cr,
        cancel_button_x + 156 / 2 - cancel_extents.width / 2,
        button_y_payment + 56 / 2 + cancel_extents.height / 2
    );
    cairo_show_text(cr, "Cancel");

    
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
    cairo_new_path(cr);
    cairo_arc(cr, confirm_button_x + 8, button_y_payment + 8, 8, M_PI, 3 * M_PI / 2); 
    cairo_arc(cr, confirm_button_x + 156 - 8, button_y_payment + 8, 8, 3 * M_PI / 2, 2 * M_PI); 
    cairo_arc(cr, confirm_button_x + 156 - 8, button_y_payment + 56 - 8, 8, 0, M_PI / 2); 
    cairo_arc(cr, confirm_button_x + 8, button_y_payment + 56 - 8, 8, M_PI / 2, M_PI); 
    cairo_close_path(cr);
    cairo_fill_preserve(cr);
    cairo_stroke(cr);

    
    cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18);
    cairo_text_extents_t confirm_extents;
    cairo_text_extents(cr, "Confirm", &confirm_extents);
    cairo_move_to(
        cr,
        confirm_button_x + 156 / 2 - confirm_extents.width / 2,
        button_y_payment + 56 / 2 + confirm_extents.height / 2
    );
    cairo_show_text(cr, "Confirm");

    return FALSE;
}

GtkWidget* create_payment_window() {
    GtkWidget *main_box, *drawing_area;

    // --- PHẦN SỬA LỖI: Khởi tạo giá trị ban đầu ---
    // Đặt lại lựa chọn voucher về mặc định (không chọn)
    selected_voucher = -1;     
    
    // Quan trọng: Gán giá tiền thanh toán bằng giá gốc ngay khi mở cửa sổ
    // Điều này ngăn chặn việc gửi giá trị 0 sang VNPAY nếu người dùng không chọn voucher
    discounted_price = price;  
    // ----------------------------------------------

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    // gtk_window_set_title(GTK_WINDOW(main_box), "Payment");
    // gtk_window_fullscreen(GTK_WINDOW(main_box));
    // gtk_window_set_resizable(GTK_WINDOW(main_box), FALSE);

    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_payment_draw), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), NULL);
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);

    return main_box;
}