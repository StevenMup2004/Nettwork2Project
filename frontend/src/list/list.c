#include <gtk/gtk.h>
#include "list.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../ticket_detail/ticket_detail.h"
#include "../component/component.h"
#include "../global/global.h"


GtkWidget *ticket_list; 
GtkWidget *main_box;    
GtkWidget *list_window;
int selected_day_index = 0; 
char *days[5];
int num_days = 0;


void on_day_button_click(GtkWidget *widget, gpointer data) {
    selected_day_index = GPOINTER_TO_INT(data);
    g_print("Selected day: %s\n", days[selected_day_index]);
    filter_flights(flights, flight_count, tem_flights, &tem_flight_count, airport_from, airport_to, days[selected_day_index], class, number_seat_order);
    
    refresh_ticket_list(ticket_list);
}


GtkWidget* create_day_selector() {

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    generate_next_5_days(date_tem_flight, days, &num_days);
    for (int i = 0; i < num_days; i++) {
        GtkWidget *day_button = gtk_button_new_with_label(days[i]);
        gtk_widget_set_name(day_button, "day_button");

        g_print("Check button: %s\n", days[i]);
        g_signal_connect(day_button, "clicked", G_CALLBACK(on_day_button_click), GINT_TO_POINTER(i));

        gtk_box_pack_start(GTK_BOX(box), day_button, TRUE, TRUE, 5);
    }

    return box;
}

void on_detail_link_click(GtkWidget *widget, gpointer data) {
    char *flight_id = (char *)data;
    g_print("Check id: %s\n", flight_id);
    for (int i = 0; i < tem_flight_count; i++) {
        if (strcmp(tem_flights[i].flight_id, flight_id) == 0) {
            detail_flight = tem_flights[i];
        }
    }

    GtkWidget *detail_window = create_ticket_detail_window();
    set_content(detail_window);
}


GtkWidget* create_ticket_list() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);


    if (tem_flight_count == 0) {
        GtkWidget *no_flight_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(no_flight_label), "<b>No flight match</b>");
        gtk_box_pack_start(GTK_BOX(box), no_flight_label, TRUE, TRUE, 5);
        return box;
    }


    for (int i = 0; i < tem_flight_count; i++) {

        GtkWidget *ticket_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        
        GtkWidget *airline_label = gtk_label_new(tem_flights[i].airplane_name);
        GtkWidget *departure_time_label = gtk_label_new(tem_flights[i].departure_time); 
        GtkWidget *arrival_time_label = gtk_label_new(extract_middle_string(tem_flights[i].departure_airport));
        GtkWidget *class_label = gtk_label_new(extract_middle_string(tem_flights[i].arrival_airport));
        GtkWidget *price_label = gtk_label_new(format_number_with_separator(tem_flights[i].price, ','));
        GtkWidget *duration_label = gtk_label_new(convert_minutes_to_time_format(tem_flights[i].duration_minutes));

        GtkWidget *check_button = gtk_button_new_with_label("Check");
        gtk_widget_set_name(check_button, "check_button");
        g_signal_connect(check_button, "clicked", G_CALLBACK(on_detail_link_click), tem_flights[i].flight_id);

        
        gtk_box_pack_start(GTK_BOX(ticket_box), airline_label, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(ticket_box), departure_time_label, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(ticket_box), arrival_time_label, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(ticket_box), class_label, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(ticket_box), price_label, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(ticket_box), duration_label, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(ticket_box), check_button, FALSE, FALSE, 5);

        
        gtk_box_pack_start(GTK_BOX(box), ticket_box, FALSE, FALSE, 0);

        
        gtk_widget_set_size_request(ticket_box, 800, 40);
        gtk_widget_set_name(ticket_box, "ticket_box");
    }
    
    return box;
}


void refresh_ticket_list(GtkWidget *container) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(ticket_list));
    
    
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    
    GtkWidget *new_ticket_list = create_ticket_list();
    gtk_box_pack_start(GTK_BOX(container), new_ticket_list, TRUE, TRUE, 0);
    gtk_widget_show_all(container); 
}

void sort_flights(gboolean ascending) {
    for (int i = 0; i < tem_flight_count; i++) {
        for (int j = i + 1; j < tem_flight_count; j++) {
            if ((ascending && tem_flights[i].price > tem_flights[j].price) ||
                (!ascending && tem_flights[i].price < tem_flights[j].price)) {
                Flight temp = tem_flights[i];
                tem_flights[i] = tem_flights[j];
                tem_flights[j] = temp;
            }
        }
    }
}

void sort_flights_by_duration(gboolean ascending) {
    for (int i = 0; i < tem_flight_count; i++) {
        for (int j = i + 1; j < tem_flight_count; j++) {
            if ((ascending && tem_flights[i].duration_minutes > tem_flights[j].duration_minutes) ||
                (!ascending && tem_flights[i].duration_minutes < tem_flights[j].duration_minutes)) {
                Flight temp = tem_flights[i];
                tem_flights[i] = tem_flights[j];
                tem_flights[j] = temp;
            }
        }
    }
}


void on_sort_changed(GtkComboBox *combo, gpointer user_data) {
    gint active_index = gtk_combo_box_get_active(combo);
    if (active_index == 0) {
        sort_flights(TRUE); 
    } else if (active_index == 1) {
        sort_flights(FALSE); 
    } else if (active_index == 2) {
        sort_flights_by_duration(TRUE); 
    } else if (active_index == 3) {
        sort_flights_by_duration(FALSE); 
    }

    
    refresh_ticket_list(ticket_list);
}


GtkWidget* create_filter_box() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *filter_label = gtk_label_new("Filter:");
    GtkWidget *combo_box = gtk_combo_box_text_new();

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Price: Low to High");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Price: High to Low");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Duration: Short to Long");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Duration: Long to Short");

    g_signal_connect(combo_box, "changed", G_CALLBACK(on_sort_changed), NULL);

    gtk_box_pack_start(GTK_BOX(box), filter_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), combo_box, FALSE, FALSE, 0);

    return box;
}


GtkWidget* create_list_window() {
    GtkWidget *main_box, *header, *filter_box, *day_selector;

    
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    
    GtkWidget *buttons[4];
    header = create_header(buttons, main_box);
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

     
    day_selector = create_day_selector();
    gtk_box_pack_start(GTK_BOX(main_box), day_selector, FALSE, FALSE, 0);


    
    filter_box = create_filter_box();
    gtk_box_pack_start(GTK_BOX(main_box), filter_box, FALSE, FALSE, 0);

    
    ticket_list = create_ticket_list();
    gtk_box_pack_start(GTK_BOX(main_box), ticket_list, TRUE, TRUE, 0);

    return main_box;
}
