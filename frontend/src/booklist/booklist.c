#include "booklist.h"
#include <cairo.h>
#include <cairo-pdf.h>
#include <gtk/gtk.h>
#include <math.h>
#include "../global/global.h"
#include "../component/component.h"


typedef struct {         
    Ticket *tickets;
    int *ticket_count;
    int index;
} ConfirmParams;

char date[20];
char time_flight[20];

static void show_contact_dialog(GtkWidget *parent) {
    // Tạo một dialog
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Contact Information", // Tiêu đề
        GTK_WINDOW(gtk_widget_get_toplevel(parent)),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Close", GTK_RESPONSE_CLOSE,
        NULL);

    // Tạo nội dung thông báo
    const char *message = 
        "For assistance, please contact our ticket support team:\n\n"
        "📧 Email: TicketTrail@gmail.com\n"
        "📞 Phone: +1-800-555-1234\n\n"
        "We are available 24/7 to assist you with your queries. Thank you for choosing TicketTrail.";

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(message);

    // Cài đặt font chữ và căn chỉnh
    PangoAttrList *attr_list = pango_attr_list_new();
    PangoAttribute *font_attr = pango_attr_size_new_absolute(18 * PANGO_SCALE); // Font size 18px
    pango_attr_list_insert(attr_list, font_attr);
    gtk_label_set_attributes(GTK_LABEL(label), attr_list);
    pango_attr_list_unref(attr_list);

    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);

    // Thêm label vào dialog
    gtk_box_pack_start(GTK_BOX(content_area), label, TRUE, TRUE, 10);

    // Hiển thị các thành phần của dialog
    gtk_widget_show_all(dialog);

    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
}

static gboolean execute_on_main_thread(gpointer user_data) {
    on_cancel(NULL, user_data);
    return FALSE; 
}

void on_cancel(GtkWidget *widget, gpointer user_data){
    g_print("Cancel button clicked");
    ConfirmParams *params = (ConfirmParams *)user_data;
    g_print("Booking check: %d\n", list_tickets[params->index].booking_id);
    int booking_id = 10;
    snprintf(buffer, sizeof(buffer), "DELETE BOOKING: %d", booking_id);
    send(sock, buffer, strlen(buffer) + 1, 0); 

    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Response from server: %s\n", buffer);
}


static void draw_ticket(cairo_t *cr, double ticket_x, double ticket_y, double ticket_width, double ticket_height, const Ticket *ticket) {
    split_date_time(ticket->departure_time, date, time_flight);
    cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
    cairo_new_path(cr);
    cairo_arc(cr, ticket_x + 24, ticket_y + 24, 24, M_PI, 3 * M_PI / 2);
    cairo_arc(cr, ticket_x + ticket_width - 24, ticket_y + 24, 24, 3 * M_PI / 2, 2 * M_PI);
    cairo_arc(cr, ticket_x + ticket_width - 24, ticket_y + ticket_height - 24, 24, 0, M_PI / 2);
    cairo_arc(cr, ticket_x + 24, ticket_y + ticket_height - 24, 24, M_PI / 2, M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);

    
    double ticket_left_margin = 50;
    double flight_icon_width = 156;
    double flight_icon_x = ticket_x + ticket_left_margin + 120; 
    double flight_icon_y = ticket_y + 50;

    double departure_city_x = ticket_x + ticket_left_margin;
    double departure_city_y = flight_icon_y + 10;
    double arrival_city_x = flight_icon_x + flight_icon_width + 50; 
    double arrival_city_y = departure_city_y;

    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 20);

    
    cairo_move_to(cr, departure_city_x, departure_city_y);
    cairo_show_text(cr,extract_middle_string(ticket->departure_airport));

    
    cairo_move_to(cr, arrival_city_x, arrival_city_y);
    cairo_show_text(cr, extract_middle_string(ticket->arrival_airport));

    
    double departure_airport_y = departure_city_y + 30;
    double arrival_airport_y = arrival_city_y + 30;

    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18);

    cairo_move_to(cr, departure_city_x, departure_airport_y);
    cairo_show_text(cr, extract_middle_string(ticket->departure_airport));

    cairo_move_to(cr, arrival_city_x, arrival_airport_y);
    cairo_show_text(cr, extract_middle_string(ticket->arrival_airport));

    
    GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
    if (flight_icon) {
        GdkPixbuf *scaled_flight_icon = gdk_pixbuf_scale_simple(flight_icon, flight_icon_width, 36, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_flight_icon, flight_icon_x, flight_icon_y);
        cairo_paint(cr);
        g_object_unref(scaled_flight_icon);
        g_object_unref(flight_icon);
    }

    
    double icon_size = 20;
    double date_time_start_x = ticket_x + ticket_left_margin + 70;
    double current_y = ticket_y + 130;

    GdkPixbuf *calendar_icon = gdk_pixbuf_new_from_file("../assets/images/calendar.png", NULL);
    if (calendar_icon) {
        GdkPixbuf *scaled_calendar_icon = gdk_pixbuf_scale_simple(calendar_icon, icon_size, icon_size, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_calendar_icon, date_time_start_x, current_y);
        cairo_paint(cr);
        g_object_unref(scaled_calendar_icon);
        g_object_unref(calendar_icon);
    }

    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, date_time_start_x + 30, current_y + 15);
    cairo_show_text(cr, date);

    GdkPixbuf *clock_icon = gdk_pixbuf_new_from_file("../assets/images/Clock.png", NULL);
    if (clock_icon) {
        GdkPixbuf *scaled_clock_icon = gdk_pixbuf_scale_simple(clock_icon, icon_size, icon_size, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_clock_icon, date_time_start_x + 200, current_y);
        cairo_paint(cr);
        g_object_unref(scaled_clock_icon);
        g_object_unref(clock_icon);
    }

    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, date_time_start_x + 230, current_y + 15);
    cairo_show_text(cr, time_flight);

    if (ticket->list_ticket[0] == '1'){
        strcpy(class, "First class");
    }
    else if (ticket->list_ticket[0] == '2'){
        strcpy(class, "Business");
    }
    else strcpy(class, "Economy");

    const char *flight_labels[4] = {"Flight", "Gate", "Seat", "Class"};
    const char *flight_info[4] = {ticket->flight_id, "22", ticket->list_ticket, class};

    double base_x = ticket_x + 70;
    current_y += 70;
    for (int i = 0; i < 4; i++) {
        cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
        cairo_set_font_size(cr, 16);
        cairo_move_to(cr, base_x + i * 150, current_y);
        cairo_show_text(cr, flight_labels[i]);

        cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
        cairo_set_font_size(cr, 18);
        cairo_move_to(cr, base_x + i * 150, current_y + 20);
        cairo_show_text(cr, flight_info[i]);
    }

    
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
    cairo_set_font_size(cr, 22);
    cairo_move_to(cr, ticket_x + ticket_width - 350, ticket_y + 60);
    cairo_show_text(cr, "Price:");

    cairo_set_font_size(cr, 32);
    cairo_move_to(cr, ticket_x + ticket_width - 280, ticket_y + 60);
    cairo_show_text(cr, format_number_with_separator(ticket->total_price, ','));

    
    double barcode_x = ticket_x + ticket_width - 350;
    double barcode_y = ticket_y + 80;
    double barcode_width = 320;
    double barcode_height = 50;
    int num_bars = 80;

    srand(12345); 
    for (int i = 0; i < num_bars; i++) {
        double bar_x = barcode_x + i * (barcode_width / num_bars);
        double bar_width = (rand() % 2 == 0) ? (barcode_width / num_bars) : (barcode_width / (num_bars * 1.2));
        cairo_set_source_rgb(cr, 0, 0, 0); 
        cairo_rectangle(cr, bar_x, barcode_y, bar_width, barcode_height);
        cairo_fill(cr);
    }
}

static void save_ticket_as_pdf(const char *filename, const Ticket *ticket) {
    double ticket_width = 1000, ticket_height = 280;

    cairo_surface_t *pdf_surface = cairo_pdf_surface_create(filename, ticket_width, ticket_height);
    cairo_t *pdf_cr = cairo_create(pdf_surface);

    draw_ticket(pdf_cr, 0, 0, ticket_width, ticket_height, ticket);

    cairo_destroy(pdf_cr);
    cairo_surface_destroy(pdf_surface);
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->type == GDK_BUTTON_PRESS) {
        Ticket *tickets = (Ticket *)user_data;
        int ticket_counts = ticket_count;

        gint screen_width = gtk_widget_get_allocated_width(widget);
        double ticket_width = 1000, ticket_height = 280;

        for (int i = 0; i < ticket_counts; ++i) {
            double ticket_x = (screen_width - ticket_width) / 2;

            
            double print_button_x = ticket_x + ticket_width - 350;
            double print_button_y = 200 + i * (ticket_height + 50);

            if (event->x >= print_button_x && event->x <= print_button_x + 320 &&
                event->y >= print_button_y && event->y <= print_button_y + 50) {
                    GtkWidget *dialog = gtk_file_chooser_dialog_new(
                    "Save Ticket",
                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                    GTK_FILE_CHOOSER_ACTION_SAVE,
                    "_Cancel", GTK_RESPONSE_CANCEL,
                    "_Save", GTK_RESPONSE_ACCEPT,
                    NULL);

                gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Your_Flight_Ticket.pdf");

                if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
                    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
                    save_ticket_as_pdf(filename, &tickets[i]);
                    g_print("Ticket saved as: %s\n", filename);
                    g_free(filename);
                }

                gtk_widget_destroy(dialog);
                return TRUE;
            }
            double cancel_button_x = ticket_x + ticket_width - 350;
            double cancel_button_y = print_button_y + 70;

            if (event->x >= cancel_button_x && event->x <= cancel_button_x + 150 &&
                event->y >= cancel_button_y && event->y <= cancel_button_y + 50) {
                ConfirmParams *params = g_new(ConfirmParams, 1); 
                params->tickets = tickets;
                params->ticket_count = &ticket_count;
                params->index = i;

                // Gọi trực tiếp hàm on_cancel
                g_print("Cancel button area clicked!\n");
                g_idle_add(execute_on_main_thread, params);

                return TRUE;
            }

             double change_button_x = ticket_x + ticket_width - 200;
            double change_button_y = cancel_button_y;

            if (event->x >= change_button_x && event->x <= change_button_x + 150 &&
                event->y >= change_button_y && event->y <= change_button_y + 50) {
                ConfirmParams *params = g_malloc(sizeof(ConfirmParams));
                params->tickets = tickets;
                params->ticket_count = &ticket_count;
                params->index = i;

                show_contact_dialog(widget);
                return TRUE;
            }
        }
    }
    return FALSE;
}
static gboolean on_booklist_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    gint screen_width = gtk_widget_get_allocated_width(widget);
    gint screen_height = gtk_widget_get_allocated_height(widget);

    
    GdkPixbuf *bg_pixbuf = gdk_pixbuf_new_from_file("../assets/images/bg_login.png", NULL);
    if (bg_pixbuf) {
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(bg_pixbuf, screen_width, screen_height, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
        cairo_paint(cr);
        g_object_unref(scaled_pixbuf);
        g_object_unref(bg_pixbuf);
    }

    if (ticket_count == 0){
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 20);
        cairo_move_to(cr, 50, 100);
        cairo_show_text(cr, "You do not have any tickets");

        return FALSE;
    }

    Ticket *tickets = (Ticket *)user_data; 
    size_t ticket_counts = ticket_count;

    double ticket_width = 1000;
    double ticket_height = 280;
    double ticket_x = (screen_width - ticket_width) / 2;
    double vertical_spacing = 50; 

    
    for (size_t i = 0; i < ticket_counts; ++i) {
        double ticket_y = 50 + i * (ticket_height + vertical_spacing); 

        
        draw_ticket(cr, ticket_x, ticket_y, ticket_width, ticket_height, &tickets[i]);

        
        double button_width = 320;
        double button_height = 50;
        double button_gap = 20; 
        double cancel_change_width = (button_width - button_gap) / 2;

        double button_x = ticket_x + ticket_width - 350;

        
        double print_button_y = ticket_y + 150; 
        cairo_set_source_rgb(cr, 34.0 / 255.0, 58.0 / 255.0, 96.0 / 255.0); 
        cairo_new_path(cr);
        cairo_arc(cr, button_x + 8, print_button_y + 8, 8, M_PI, 3 * M_PI / 2);
        cairo_arc(cr, button_x + button_width - 8, print_button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
        cairo_arc(cr, button_x + button_width - 8, print_button_y + button_height - 8, 8, 0, M_PI / 2);
        cairo_arc(cr, button_x + 8, print_button_y + button_height - 8, 8, M_PI / 2, M_PI);
        cairo_close_path(cr);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
        cairo_stroke(cr);

        
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
        cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 18);
        cairo_text_extents_t print_text_extents;
        cairo_text_extents(cr, "Print ticket out", &print_text_extents);
        cairo_move_to(cr, button_x + (button_width - print_text_extents.width) / 2,
                      print_button_y + (button_height + print_text_extents.height) / 2);
        cairo_show_text(cr, "Print ticket out");

        if (is_valid_date(date) == 0) {
            
            double cancel_button_y = print_button_y + button_height + 20;
            double cancel_button_x = button_x;

            cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
            cairo_new_path(cr);
            cairo_arc(cr, cancel_button_x + 8, cancel_button_y + 8, 8, M_PI, 3 * M_PI / 2);
            cairo_arc(cr, cancel_button_x + cancel_change_width - 8, cancel_button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
            cairo_arc(cr, cancel_button_x + cancel_change_width - 8, cancel_button_y + button_height - 8, 8, 0, M_PI / 2);
            cairo_arc(cr, cancel_button_x + 8, cancel_button_y + button_height - 8, 8, M_PI / 2, M_PI);
            cairo_close_path(cr);
            cairo_fill_preserve(cr);
            cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
            cairo_stroke(cr);

            
            cairo_set_source_rgb(cr, 0.13, 0.23, 0.37);
            cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 18);
            cairo_text_extents_t cancel_text_extents;
            cairo_text_extents(cr, "Cancel", &cancel_text_extents);
            cairo_move_to(cr, cancel_button_x + (cancel_change_width - cancel_text_extents.width) / 2,
                        cancel_button_y + (button_height + cancel_text_extents.height) / 2);
            cairo_show_text(cr, "Cancel");

            
            double change_button_x = cancel_button_x + cancel_change_width + button_gap;

            cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
            cairo_new_path(cr);
            cairo_arc(cr, change_button_x + 8, cancel_button_y + 8, 8, M_PI, 3 * M_PI / 2);
            cairo_arc(cr, change_button_x + cancel_change_width - 8, cancel_button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
            cairo_arc(cr, change_button_x + cancel_change_width - 8, cancel_button_y + button_height - 8, 8, 0, M_PI / 2);
            cairo_arc(cr, change_button_x + 8, cancel_button_y + button_height - 8, 8, M_PI / 2, M_PI);
            cairo_close_path(cr);
            cairo_fill_preserve(cr);
            cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
            cairo_stroke(cr);

            
            cairo_set_source_rgb(cr, 0.13, 0.23, 0.37);
            cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 18);
            cairo_text_extents_t change_text_extents;
            cairo_text_extents(cr, "Change", &change_text_extents);
            cairo_move_to(cr, change_button_x + (cancel_change_width - change_text_extents.width) / 2,
                        cancel_button_y + (button_height + change_text_extents.height) / 2);
            cairo_show_text(cr, "Change");
        }
        
    }

    return FALSE;
}

GtkWidget *create_booklist_window() {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), main_box);

    GtkWidget *buttons[4];
    GtkWidget *header = create_header(buttons, scrolled_window);
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_booklist_draw), list_tickets);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), list_tickets);

    gtk_widget_set_size_request(drawing_area, -1, 3 * (280 + 50 + 50));

    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);

    return scrolled_window;

}