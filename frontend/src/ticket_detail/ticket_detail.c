#include <gtk/gtk.h>
#include "ticket_detail.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <arpa/inet.h>
#include "../global/global.h"
#include "../list/list.h"
#include "../book_seat/book_seat.h"
gint screen_width;
gint screen_height;
cairo_text_extents_t cancel_extents;


gboolean on_mouse_click_ticket_detail(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    double cancel_button_x = (screen_width - 936) / 2 + (936 - 344) / 2;
    double cancel_button_y = (screen_height - 588) / 2 + 500;
    double cancel_button_width = 156;
    double cancel_button_height = 56;

    double confirm_button_x = (screen_width - 936) / 2 + (936 - 344) / 2 + 180;
    double confirm_button_y = (screen_height - 588) / 2 + 500;
    double confirm_button_width = 156;
    double confirm_button_height = 56;

    if (event->x >= cancel_button_x &&
        event->x <= cancel_button_x + cancel_button_width &&
        event->y >= cancel_button_y &&
        event->y <= cancel_button_y + cancel_button_height) {
        g_print("Cancel button clicked in ticket detail!\n");
        GtkWidget *list_window = create_list_window();
        set_content(list_window);
    }

    if (event->x >= confirm_button_x &&
        event->x <= confirm_button_x + confirm_button_width &&
        event->y >= confirm_button_y &&
        event->y <= confirm_button_y + confirm_button_height) {
        g_print("Confirm button clicked!\n");
        sprintf(buffer, "GET ORDERED SEATS %s", detail_flight.flight_id);
        send(sock, buffer, strlen(buffer), 0);
        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if (strcmp(buffer, "NOTHING") == 0){
                g_print("No ordered seats.\n");
                GtkWidget *book_seat_window = create_book_seat_window();
                set_content(book_seat_window);
                return TRUE;
            }
            sscanf(buffer, "SEAT COUNT: %d", &seat_count); 
        }
        g_print("Start\n");
        seats_array = malloc(seat_count * sizeof(char *));
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            perror("Error receiving seats data");
            return TRUE;
        }
        buffer[bytes_received] = '\0'; 
        printf("Received seats string: %s\n", buffer);

        char *token = strtok(buffer, ",");
        int i = 0;
        while (token) {
            seats_array[i] = strdup(token);
            if (!seats_array[i]) {
                perror("Error duplicating seat string");
                break;
            }
            token = strtok(NULL, ",");
            i++;
        }

        printf("Parsed seats:\n");
        for (int j = 0; j < seat_count; j++) {
            printf("Seat %d: %s\n", j + 1, seats_array[j]);
        }
        printf("Receive done\n");
        GtkWidget *book_seat_window = create_book_seat_window();
        set_content(book_seat_window);
    }

    return TRUE; 
}

static gboolean on_ticket_detail_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    GdkPixbuf *bg_pixbuf;
    GdkPixbuf *scaled_pixbuf;
    char date[20];
    char time[20];
    split_date_time(detail_flight.departure_time, date, time);

    
    screen_width = gtk_widget_get_allocated_width(widget);
    screen_height = gtk_widget_get_allocated_height(widget);

    
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
    gfloat radius = 24.0;

    
    cairo_move_to(cr, (screen_width - 936) / 2 + radius, (screen_height - 588) / 2); 
    cairo_arc(cr, (screen_width - 936) / 2 + 936 - radius, (screen_height - 588) / 2 + radius, radius, 3 * M_PI / 2, 2 * M_PI); 
    cairo_arc(cr, (screen_width - 936) / 2 + 936 - radius, (screen_height - 588) / 2 + 588 - radius, radius, 0, M_PI / 2); 
    cairo_arc(cr, (screen_width - 936) / 2 + radius, (screen_height - 588) / 2 + 588 - radius, radius, M_PI / 2, M_PI); 
    cairo_arc(cr, (screen_width - 936) / 2 + radius, (screen_height - 588) / 2 + radius, radius, M_PI, 3 * M_PI / 2); 

    cairo_close_path(cr); 

    cairo_set_line_width(cr, 1.0);
    cairo_fill_preserve(cr); 
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); 
    cairo_stroke(cr); 



    
    
    GdkPixbuf *airline_logo = gdk_pixbuf_new_from_file("../assets/images/airline.png", NULL);
    if (airline_logo) {
        GdkPixbuf *scaled_logo = gdk_pixbuf_scale_simple(airline_logo, 328, 80, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_logo, (screen_width - 936) / 2 + (936 - 328) / 2, (screen_height - 588) / 2 + 10);
        cairo_paint(cr);
        g_object_unref(scaled_logo);
        g_object_unref(airline_logo);
    }

    
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2, (screen_height - 588) / 2 + 90);
    cairo_line_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2 + 600, (screen_height - 588) / 2 + 90);
    cairo_stroke(cr);

    
    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    cairo_move_to(cr, (screen_width - 936) / 2 + 200, (screen_height - 588) / 2 + 160);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 20);
    cairo_move_to(cr, (screen_width - 936) / 2 + 200, (screen_height - 588) / 2 + 190);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_move_to(cr, (screen_width - 936) / 2 + 600, (screen_height - 588) / 2 + 160);
    cairo_set_font_size(cr, 24);
    cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

    
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
    cairo_move_to(cr, (screen_width - 936) / 2 + 600, (screen_height - 588) / 2 + 190);
    cairo_set_font_size(cr, 20);
    cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

    
    GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
    if (flight_icon) {
        GdkPixbuf *scaled_flight_icon = gdk_pixbuf_scale_simple(flight_icon, 156, 36, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_flight_icon, (screen_width - 936) / 2 + (936 - 156) / 2, (screen_height - 588) / 2 + 160);
        cairo_paint(cr);
        g_object_unref(scaled_flight_icon);
        g_object_unref(flight_icon);
    }

    
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
    cairo_move_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2, (screen_height - 588) / 2 + 250);
    cairo_line_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2 + 600, (screen_height - 588) / 2 + 250);
    cairo_stroke(cr);

    
    
        cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 20); 

    
    cairo_text_extents_t date_extents, time_extents;
    cairo_text_extents(cr, "Date", &date_extents);
    cairo_text_extents(cr, "Time", &time_extents);

    
    cairo_move_to(cr, (screen_width - 936) / 2 + 270 + (140 + 16) / 2 - date_extents.width / 2, 
                (screen_height - 588) / 2 + 300 - 10); 
    cairo_show_text(cr, "Date");

    
    cairo_move_to(cr, (screen_width - 936) / 2 + 16 + 140 + 270 + (140 + 16) / 2 - time_extents.width / 2,
                (screen_height - 588) / 2 + 300 - 10); 
    cairo_show_text(cr, "Time");

    
    double date_x = (screen_width - 936) / 2 + 320;
    double time_x = date_x + 140 + 16;

    
    
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
    cairo_new_path(cr);

    
    cairo_move_to(cr, date_x + 8, (screen_height - 588) / 2 + 300); 

    
    cairo_line_to(cr, date_x + 140 - 8, (screen_height - 588) / 2 + 300); 
    cairo_arc(cr, date_x + 140 - 8, (screen_height - 588) / 2 + 300 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 

    cairo_line_to(cr, date_x + 140, (screen_height - 588) / 2 + 300 + 46 - 8); 
    cairo_arc(cr, date_x + 140 - 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, 0, M_PI / 2); 

    cairo_line_to(cr, date_x + 8, (screen_height - 588) / 2 + 300 + 46); 
    cairo_arc(cr, date_x + 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, M_PI / 2, M_PI); 

    cairo_line_to(cr, date_x, (screen_height - 588) / 2 + 300 + 8); 
    cairo_arc(cr, date_x + 8, (screen_height - 588) / 2 + 300 + 8, 8, M_PI, 3 * M_PI / 2); 

    cairo_close_path(cr); 

    cairo_fill_preserve(cr); 
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
    cairo_stroke(cr); 

    
    GdkPixbuf *calendar_icon = gdk_pixbuf_new_from_file("../assets/images/calendar.png", NULL);
    if (calendar_icon) {
        GdkPixbuf *scaled_calendar_icon = gdk_pixbuf_scale_simple(calendar_icon, 20, 20, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_calendar_icon, date_x + 8, (screen_height - 588) / 2 + 300 + (46 - 20) / 2);
        cairo_paint(cr);
        g_object_unref(scaled_calendar_icon);
        g_object_unref(calendar_icon);
    }

    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18); 
    
    cairo_text_extents_t date_text_extents;
    cairo_text_extents(cr, date, &date_text_extents);

    double date_text_y = (screen_height - 588) / 2 + 315 + (46 - date_text_extents.height) / 2;
    
    cairo_move_to(cr, date_x + (160 - date_text_extents.width) / 2, date_text_y);
    cairo_show_text(cr, date);
    
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
    cairo_new_path(cr);
    
    cairo_move_to(cr, time_x + 8, (screen_height - 588) / 2 + 300); 

    
    cairo_line_to(cr, time_x + 140 - 8, (screen_height - 588) / 2 + 300); 
    cairo_arc(cr, time_x + 140 - 8, (screen_height - 588) / 2 + 300 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 

    cairo_line_to(cr, time_x + 140, (screen_height - 588) / 2 + 300 + 46 - 8); 
    cairo_arc(cr, time_x + 140 - 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, 0, M_PI / 2); 

    cairo_line_to(cr, time_x + 8, (screen_height - 588) / 2 + 300 + 46); 
    cairo_arc(cr, time_x + 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, M_PI / 2, M_PI); 

    cairo_line_to(cr, time_x, (screen_height - 588) / 2 + 300 + 8); 
    cairo_arc(cr, time_x + 8, (screen_height - 588) / 2 + 300 + 8, 8, M_PI, 3 * M_PI / 2); 

    cairo_close_path(cr); 

    cairo_fill_preserve(cr); 
    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
    cairo_stroke(cr); 


    
    GdkPixbuf *clock_icon = gdk_pixbuf_new_from_file("../assets/images/Clock.png", NULL);
    if (clock_icon) {
        GdkPixbuf *scaled_clock_icon = gdk_pixbuf_scale_simple(clock_icon, 20, 20, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_clock_icon, time_x + 8, (screen_height - 588) / 2 + 300 + (46 - 20) / 2);
        cairo_paint(cr);
        g_object_unref(scaled_clock_icon);
        g_object_unref(clock_icon);
    }
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18); 
    cairo_text_extents_t time_text_extents;
    cairo_text_extents(cr, time, &time_text_extents);  

    
    double time_text_y = (screen_height - 588) / 2 + 315 + (46 - time_text_extents.height) / 2;  

    
    cairo_move_to(cr, time_x + (150 - time_text_extents.width) / 2, time_text_y);

    
    cairo_show_text(cr, time);

    cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
    cairo_move_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2, (screen_height - 588) / 2 + 380);
    cairo_line_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2 + 600, (screen_height - 588) / 2 + 380);
    cairo_stroke(cr);

    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); 
    cairo_set_font_size(cr, 22); 
    cairo_text_extents_t price_text_extents;
    cairo_text_extents(cr, "Price", &price_text_extents); 

    
    double price_x = (screen_width - 936) / 2 + (160 - price_text_extents.width) / 2 + 300;
    double price_y = (screen_height - 588) / 2 + 430;

    
    cairo_move_to(cr, price_x, price_y);
    cairo_show_text(cr, "Price");

    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); 
    cairo_set_font_size(cr, 32); 
    cairo_text_extents_t price_value_extents;
    cairo_text_extents(cr, format_number_with_separator(detail_flight.price, ','), &price_value_extents); 

    
    double price_value_x = price_x + price_text_extents.width + 10; 
    double price_value_y = price_y; 

    
    cairo_move_to(cr, price_value_x, price_value_y);
    cairo_show_text(cr, format_number_with_separator(detail_flight.price, ','));


    
    
    cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
    cairo_new_path(cr);
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 8, (screen_height - 588) / 2 + 500 + 8, 8, M_PI, 3 * M_PI / 2); 
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 156 - 8, (screen_height - 588) / 2 + 500 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 156 - 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, 0, M_PI / 2); 
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, M_PI / 2, M_PI); 
    cairo_close_path(cr);
    cairo_fill_preserve(cr); 
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr); 

    
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL); 
    cairo_set_font_size(cr, 18); 

    
    cancel_extents;
    cairo_text_extents(cr, "Cancel", &cancel_extents);
    cairo_move_to(
        cr,
        (screen_width - 936) / 2 + (936 - 344) / 2 + 156 / 2 - cancel_extents.width / 2,
        (screen_height - 588) / 2 + 500 + 56 / 2 + cancel_extents.height / 2
    );
    cairo_show_text(cr, "Cancel");
    
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
    cairo_new_path(cr);
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 8, (screen_height - 588) / 2 + 500 + 8, 8, M_PI, 3 * M_PI / 2); 
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 156 - 8, (screen_height - 588) / 2 + 500 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 156 - 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, 0, M_PI / 2); 
    cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, M_PI / 2, M_PI); 
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
        (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 156 / 2 - confirm_extents.width / 2,
        (screen_height - 588) / 2 + 500 + 56 / 2 + confirm_extents.height / 2
    );
    cairo_show_text(cr, "Confirm");

        return FALSE;
    }
GtkWidget* create_ticket_detail_window() {
    GtkWidget *drawing_area, *main_box;

    
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    
    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_ticket_detail_draw), NULL);

    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_mouse_click_ticket_detail), NULL);
    
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);

    return main_box;
}

