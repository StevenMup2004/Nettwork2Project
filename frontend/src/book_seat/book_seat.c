#include <gtk/gtk.h>
#include "book_seat.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <arpa/inet.h>
#include "../global/global.h"
#include "../payment/payment.h"

#define ROWS 30
#define SEATS_PER_ROW 10
int actual_rows = 20;
int i_code, j_code;
int ordered[ROWS][SEATS_PER_ROW];
int temp_ordered[ROWS][SEATS_PER_ROW];
double button_x, button_y, button_width = 120, button_height = 40;
typedef struct {
    int row;
    int seat;
    gboolean selected;
    char label[4];
} Seat;
Seat seats[ROWS][SEATS_PER_ROW];

char selected_seat_label[4] = "";  

void initialize_seats() {

    actual_rows = detail_flight.capacity/10;
    const char *columns = "ABCDEFGHIJ";
    for (int i = 0; i < actual_rows; i++) {
        for (int j = 0; j < SEATS_PER_ROW; j++) {
            seats[i][j].row = i + 1;
            seats[i][j].seat = j + 1;
            seats[i][j].selected = FALSE;
            snprintf(seats[i][j].label, sizeof(seats[i][j].label), "%d%c", i + 1, columns[j]);
        }
    }

    memset(ordered, 0, sizeof(ordered));
    memset(temp_ordered, 0, sizeof(temp_ordered));
    for (int i = 0; i < seat_count; i++){
        if (get_seat_position(seats_array[i], &i_code, &j_code) == 0){
            ordered[i_code][j_code] = 1;
            seats[i_code][j_code].selected = TRUE;
        }
    }
    for (int i = 0; i < tem_seats_size; i++){
        if (get_seat_position(temp_seats[i], &i_code, &j_code) == 0){
            temp_ordered[i_code][j_code] = 1;
            seats[i_code][j_code].selected = TRUE;
        }
    }
}


static gboolean on_book_seat_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    char date[20];
    char time[20];
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
gfloat rect_width_left = 600.0;  
gfloat rect_height_left = 588.0; 
gfloat padding_left = 50.0;     
gfloat x_position_left = padding_left;                    
gfloat y_position_left = (screen_height - rect_height_left) / 2; 

    gfloat rect_width = 500.0;  
    gfloat rect_height = 588;
    gfloat radius = 24.0;       
    gfloat padding_right = 50.0; 
    gfloat x_position = screen_width - rect_width - padding_right; 
    gfloat y_position = (screen_height - rect_height) / 2; 

    cairo_move_to(cr, x_position + radius, y_position);
cairo_arc(cr, x_position + rect_width - radius, y_position + radius, radius, 3 * M_PI / 2, 2 * M_PI);  
cairo_arc(cr, x_position + rect_width - radius, y_position + rect_height - radius, radius, 0, M_PI / 2);  
cairo_arc(cr, x_position + radius, y_position + rect_height - radius, radius, M_PI / 2, M_PI);  
cairo_arc(cr, x_position + radius, y_position + radius, radius, M_PI, 3 * M_PI / 2);  

    cairo_close_path(cr);  

    cairo_set_line_width(cr, 1.0);
    cairo_fill_preserve(cr);  
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);  
    cairo_stroke(cr);  


cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);  
cairo_move_to(cr, x_position_left + radius, y_position_left);
cairo_arc(cr, x_position_left + rect_width_left - radius, y_position_left + radius, radius, 3 * M_PI / 2, 2 * M_PI); 
cairo_arc(cr, x_position_left + rect_width_left - radius, y_position_left + rect_height_left - radius, radius, 0, M_PI / 2); 
cairo_arc(cr, x_position_left + radius, y_position_left + rect_height_left - radius, radius, M_PI / 2, M_PI); 
cairo_arc(cr, x_position_left + radius, y_position_left + radius, radius, M_PI, 3 * M_PI / 2); 
cairo_close_path(cr);
cairo_fill_preserve(cr); 
cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);  
cairo_stroke(cr);  

double seat_width = 30, seat_height = 30;
    double margin = 10;

    for (int i = 0; i < actual_rows; i++) {
        for (int j = 0; j < SEATS_PER_ROW; j++) {
            double x = 150 + margin + j * (seat_width + margin);
            double y = 150 + margin + i * (seat_height + margin);

            if (i == 0) {
                cairo_set_source_rgb(cr, 1.0, 0.75, 0.8);  
            } else if (i == 1 || i == 2) {
                cairo_set_source_rgb(cr, 0.7, 0.9, 1.0);  
            } else {
                cairo_set_source_rgb(cr, 0.7, 0.5, 1.0);  
            }

             
            if (seats[i][j].selected) {
                cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);  
            }
            if (ordered[i][j] ==  1){
                cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);  
            }
            // if (temp_ordered[i][j] ==  1){
            //     cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);  
            // }
            cairo_rectangle(cr, x, y, seat_width, seat_height);
            cairo_fill(cr);

             
            cairo_set_source_rgb(cr, 0, 0, 0);  
            cairo_move_to(cr, x + seat_width / 4, y + seat_height / 1.5);
            cairo_show_text(cr, seats[i][j].label);
        }
    }


     
    GdkPixbuf *airline_logo = gdk_pixbuf_new_from_file("../assets/images/airline.png", NULL);
    if (airline_logo) {
        GdkPixbuf *scaled_logo = gdk_pixbuf_scale_simple(airline_logo, 219, 53, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_logo,  x_position + radius + 120, y_position + 10);
        cairo_paint(cr);
        g_object_unref(scaled_logo);
        g_object_unref(airline_logo);
    }

     
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91);  
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, x_position + radius, y_position + 61);
    cairo_line_to(cr, x_position + radius + 450, y_position + 61);
    cairo_stroke(cr);

     
     
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, (screen_width - 936) / 2 + 610, (screen_height - 588) / 2 + 100);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

     
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, (screen_width - 936) / 2 + 610, (screen_height - 588) / 2 + 115);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

     
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_move_to(cr, (screen_width - 936) / 2 + 770, (screen_height - 588) / 2 + 100);
    cairo_set_font_size(cr, 14);
    cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

     
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to(cr, (screen_width - 936) / 2 + 770, (screen_height - 588) / 2 + 115);
    cairo_set_font_size(cr, 12);
    cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

     
    GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
    if (flight_icon) {
        GdkPixbuf *scaled_flight_icon = gdk_pixbuf_scale_simple(flight_icon, 59, 14, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_flight_icon, (screen_width - 936) / 2 + 710, (screen_height - 588) / 2 + 88);
        cairo_paint(cr);
        g_object_unref(scaled_flight_icon);
        g_object_unref(flight_icon);
    }

     
cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
cairo_set_line_width(cr, 1.0);

 
double x_pos = (screen_width - 936) / 2 + 850;  
double y_start = (screen_height - 588) / 2 + 65;  
double y_end = y_start + 75;  

 
cairo_move_to(cr, x_pos, y_start);  
cairo_line_to(cr, x_pos, y_end);   
cairo_stroke(cr);


     
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91);  
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, x_position + radius, y_position + 150);
    cairo_line_to(cr, x_position + radius + 450, y_position + 150);
    cairo_stroke(cr);

      
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, (screen_width - 936) / 2 + 870, (screen_height - 588) / 2 + 100);
    cairo_show_text(cr, date);

     
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, (screen_width - 936) / 2 + 870, (screen_height - 588) / 2 + 115);
    cairo_show_text(cr, time);

     
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_move_to(cr, (screen_width - 936) / 2 + 970, (screen_height - 588) / 2 + 100);
    cairo_set_font_size(cr, 14);
    cairo_show_text(cr, date);

       
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to(cr, (screen_width - 936) / 2 + 940, (screen_height - 588) / 2 + 100);
    cairo_set_font_size(cr, 14);
    cairo_show_text(cr, "--");

     
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  
    cairo_move_to(cr, (screen_width - 936) / 2 + 970, (screen_height - 588) / 2 + 115);
    cairo_set_font_size(cr, 12);
    cairo_show_text(cr,calculate_end_time(time, detail_flight.duration_minutes * 60));  


     
      GdkPixbuf *economy_logo = gdk_pixbuf_new_from_file("../assets/images/Economy.png", NULL);
    if (economy_logo) {
        GdkPixbuf *scaled_economy = gdk_pixbuf_scale_simple(economy_logo, 213, 120, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_economy,  x_position + radius - 30, y_position + 200);
        cairo_paint(cr);
        g_object_unref(scaled_economy);
        g_object_unref(economy_logo);
    }

      
    cairo_set_source_rgb(cr, 0.7, 0.5, 1.0); 
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, (screen_width - 936) / 2 + 650, (screen_height - 588) / 2 + 330);
    cairo_show_text(cr, "Economy Class");

      GdkPixbuf *business_logo = gdk_pixbuf_new_from_file("../assets/images/Business.png", NULL);
    if (business_logo) {
        GdkPixbuf *scaled_business = gdk_pixbuf_scale_simple(business_logo, 213, 120, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_business,  x_position + radius + 230, y_position + 200);
        cairo_paint(cr);
        g_object_unref(scaled_business);
        g_object_unref(business_logo);
    }

     
    cairo_set_source_rgb(cr, 0.7, 0.9, 1.0); 
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, (screen_width - 936) / 2 + 870, (screen_height - 588) / 2 + 330);
    cairo_show_text(cr, "Business Class");

     GdkPixbuf *first_logo = gdk_pixbuf_new_from_file("../assets/images/First.png", NULL);
    if (first_logo) {
        GdkPixbuf *scaled_first = gdk_pixbuf_scale_simple(first_logo, 213, 120, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_first,  x_position + radius + 100, y_position + 350);
        cairo_paint(cr);
        g_object_unref(scaled_first);
        g_object_unref(first_logo);
    }

     
    cairo_set_source_rgb(cr, 1.0, 0.75, 0.8); 
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, (screen_width - 936) / 2 + 750, (screen_height - 588) / 2 + 480);
    cairo_show_text(cr, "First Class");


       
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91);  
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, x_position + radius, y_position + 500);
    cairo_line_to(cr, x_position + radius + 450, y_position + 500);
    cairo_stroke(cr);  

     
   cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, (screen_width - 936) / 2 + 650, (screen_height - 588) / 2 + 550);
    cairo_show_text(cr, "Seat number: ");
    cairo_show_text(cr, join_strings(temp_seats, tem_seats_size, ", ")); 

    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);  
    cairo_select_font_face(cr, "Poppins", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);

    char price_text[32];
    snprintf(price_text, sizeof(price_text), "Price: %s VND", format_number_with_separator(price, ','));

    cairo_move_to(cr, (screen_width - 936) / 2 + 650, (screen_height - 588) / 2 + 570);  
    cairo_show_text(cr, price_text);

    button_x = screen_width - 70 - button_width;
    button_y = (screen_height - 588) / 2 + 530;

    // Vẽ nút Confirm
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37);
    cairo_new_path(cr);
    cairo_arc(cr, button_x + 8, button_y + 8, 8, M_PI, 3 * M_PI / 2);
    cairo_arc(cr, button_x + button_width - 8, button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
    cairo_arc(cr, button_x + button_width - 8, button_y + button_height - 8, 8, 0, M_PI / 2);
    cairo_arc(cr, button_x + 8, button_y + button_height - 8, 8, M_PI / 2, M_PI);
    cairo_close_path(cr);
    cairo_fill_preserve(cr);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0.92, 0.94, 0.94);
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 16);

    cairo_text_extents_t confirm_extents;
    cairo_text_extents(cr, "Confirm", &confirm_extents);
    cairo_move_to(cr,
                  button_x + button_width / 2 - confirm_extents.width / 2,
                  button_y + button_height / 2 + confirm_extents.height / 2);
    cairo_show_text(cr, "Confirm");

        return FALSE;
    }

  gboolean on_mouse_click_book_seat(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    double seat_width = 30, seat_height = 30;
    double margin = 10;
    double offset_x = 150;  

     if (event->x >= button_x && event->x <= button_x + button_width &&
        event->y >= button_y && event->y <= button_y + button_height) {
        g_print("Confirm button clicked!\n");
        if (tem_seats_size == 0){
            gtk_label_set_text(GTK_LABEL(label_status), "Choose a least 1 seat");
            return TRUE;
        }
        GtkWidget *payment_window = create_payment_window();
        set_content(payment_window);
        return TRUE;
    }


    for (int i = 0; i < actual_rows; i++) {
        for (int j = 0; j < SEATS_PER_ROW; j++) {
             
            double x = offset_x + margin + j * (seat_width + margin);
            double y = 150 + margin + i * (seat_height + margin);   

             
            if (event->x >= x && event->x <= x + seat_width && event->y >= y && event->y <= y + seat_height) {
                 
                 
                seats[i][j].selected = !seats[i][j].selected;
                gtk_widget_queue_draw(widget);   
                if (ordered[i][j] == 1){
                     gtk_label_set_text(GTK_LABEL(label_status), "Please choose another, this seat is unavailable");
                     return TRUE;
                }

                if (seats[i][j].selected) {
                    strncpy(selected_seat_label, seats[i][j].label, sizeof(selected_seat_label));
                    g_print("Seat selected: %s\n", selected_seat_label);
                    temp_seats = add_string_to_array(temp_seats, &tem_seats_size, selected_seat_label);
                    if (i == 0){
                        price += detail_flight.price * 10;
                    }
                    else if (i == 1 || i == 2){
                        price += detail_flight.price * 5;
                    }
                    else price += detail_flight.price;
                } else {
                    g_print("Seat deselected: %s\n", seats[i][j].label);
                    temp_seats = remove_string_from_array(temp_seats, &tem_seats_size, selected_seat_label);
                    if (i == 0){
                        price -= detail_flight.price * 10;
                    }
                    else if (i == 1 || i == 2){
                        price -= detail_flight.price * 5;
                    }
                    else price -= detail_flight.price;
                }
                return TRUE;
            }
        }
    }

    return FALSE;
}

  GtkWidget* create_book_seat_window() {
    GtkWidget *drawing_area, *main_box;

     
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

     initialize_seats(); 

     
    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_book_seat_draw), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(on_mouse_click_book_seat), NULL);

    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);

    label_status = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(main_box), label_status, FALSE, FALSE, 0);
    GtkCssProvider *css_provider_label = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider_label,
        "label {"
        "   color: red;"  // Đặt màu chữ thành đỏ
        "}",
        -1, NULL);

    // Áp dụng CSS cho label
    gtk_style_context_add_provider(gtk_widget_get_style_context(label_status),
    GTK_STYLE_PROVIDER(css_provider_label), GTK_STYLE_PROVIDER_PRIORITY_USER);


    return main_box;
}