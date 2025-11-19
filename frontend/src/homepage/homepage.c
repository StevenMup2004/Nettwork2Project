#include <gtk/gtk.h>
#include "homepage.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../component/component.h"
#include "../global/global.h"
#include "../server_com/server_com.h"
#include  "../list/list.h"

GtkWidget *homepage_window ;
GtkWidget *input_from, *input_to, *input_departure, *input_return, *combo_box, *input_traveller;
int bytes_received; 
void on_list_link_click(GtkWidget *widget, gpointer data) {
    send(sock, "GET FLIGHTS", strlen("GET FLIGHTS"), 0);
    recv(sock, &flight_count, sizeof(flight_count), 0);
    printf("Number of flights: %d\n", flight_count);
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            printf("Failed to receive flight data.\n");
        }

        buffer[bytes_received] = '\0';
        printf("Received flight: %s\n", buffer);

    parse_flight_data(buffer,flights);
    for (int i = 0; i < flight_count; i++){
        printf("ID of flight: %s\n", flights[i].flight_id);
    }
    const char *traveller = gtk_entry_get_text(GTK_ENTRY(input_traveller));
    const char *selected_class = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
    const char *date_text;
    GList *children = gtk_container_get_children(GTK_CONTAINER(input_departure));
    GtkWidget *entry = GTK_WIDGET(g_list_nth_data(children, 0)); 
    const char *from = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_from));
    const char *to = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_to));
    if (GTK_IS_ENTRY(entry)) {
        date_text = gtk_entry_get_text(GTK_ENTRY(entry));
    } else {
        g_print("No entry found in input_departure\n");
    }

    g_list_free(children);

    // GList *children1 = gtk_container_get_children(GTK_CONTAINER(input_return));
    // GtkWidget *entry1 = GTK_WIDGET(g_list_nth_data(children1, 0)); 

    // if (GTK_IS_ENTRY(entry1)) {
    //     date_return_text = gtk_entry_get_text(GTK_ENTRY(entry1));
    // } else {
    //     g_print("No entry found in input_return\n");
    // }

    // g_list_free(children1);

    g_print("%s %s %s %s %s %s\n", from, to, traveller, selected_class, date_text);
    strcpy(date_tem_flight, date_text);
    strcpy(airport_from, from);
    strcpy(airport_to, to);
    strcpy(class, selected_class);
    number_seat_order = atoi(traveller);
     
    if (strlen(from) == 0 || strlen(to) == 0 || strlen(selected_class) == 0 || strlen(date_text) == 0 || strlen(traveller) == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "All fields are required!");
        return;
    }
    if (!is_number(traveller)){
        gtk_label_set_text(GTK_LABEL(label_status), "Number people must be a number!");
        return;
    }
    if (strcmp(from, to) == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "Departure airport and arrive airport must be different!");
        return;
    }
    
    filter_flights(flights, flight_count, tem_flights, &tem_flight_count, from, to, date_text, selected_class, atoi(traveller));
    for (int i = 0; i < tem_flight_count; i++){
        printf("ID of flight: %s\n", tem_flights[i].flight_id);
    }
    if (tem_flight_count == 0) {
        g_print("No flight\n");
    }
    GtkWidget *list_window = create_list_window();
    if (!list_window) {
        g_warning("Failed to create list window!");
        return;
    }
    set_content(list_window);
}
static void on_window_realize(GtkWidget *widget, gpointer user_data) {
    GtkWidget *calendar = GTK_WIDGET(user_data);
    gtk_widget_hide(calendar);  
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    GdkPixbuf *pixbuf;
    GdkPixbuf *scaled_pixbuf;

    pixbuf = gdk_pixbuf_new_from_file("../assets/images/homepage.png", NULL);
    if (!pixbuf) {
        g_print("Error loading image!\n");
        return FALSE;
    }

    gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);

    gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
    cairo_paint(cr);

    g_object_unref(pixbuf);
    g_object_unref(scaled_pixbuf);

    return FALSE;
}

static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}


static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
    gchar date_str[11];  
    guint year, month, day;
    GtkWidget *input_box = GTK_WIDGET(user_data);  

    
    gtk_calendar_get_date(calendar, &year, &month, &day);
    
    
    g_snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", year, month + 1, day);
    
    
    gtk_entry_set_text(GTK_ENTRY(input_box), date_str);
    
    
    gtk_widget_hide(GTK_WIDGET(calendar));
}

static gboolean on_input_box_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *calendar = GTK_WIDGET(user_data);

    
    gtk_widget_show(calendar);
    return FALSE;
}
static gboolean on_input_class_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *combo_box = GTK_WIDGET(user_data);

    
    gtk_widget_show(combo_box);
    return FALSE;
}


static void on_class_selected(GtkComboBox *combo_box, gpointer user_data) {
    const gchar *selected_class = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
    GtkWidget *input_class = GTK_WIDGET(user_data);

    
    gtk_entry_set_text(GTK_ENTRY(input_class), selected_class);

    
    gtk_widget_hide(GTK_WIDGET(combo_box));
}
static void on_window_realize1(GtkWidget *widget, gpointer user_data) {
    GtkWidget *combo_box = GTK_WIDGET(user_data);
    gtk_widget_hide(combo_box);  
}

GtkWidget* create_input_box(const gchar *placeholder) {
    GtkWidget *input_box = gtk_entry_new();  
    gtk_widget_set_size_request(input_box, 210, 40);  
    gtk_entry_set_placeholder_text(GTK_ENTRY(input_box), placeholder);

    
    GtkCssProvider *input_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(input_provider,
        "entry {"
        "  font-family: 'DM Sans', sans-serif;"
        "  font-size: 16px;"
        "  font-weight: 500;"
        "  letter-spacing: 0.06em;"
        "  text-align: center;"
        "  border-radius: 6px;"
        "  padding: 12px 20px;"
        "  background-color: white;"
        "  border: 1px solid #ccc;"
        "  color: #283841;"
        "}",
        -1, NULL);

    gtk_style_context_add_provider(gtk_widget_get_style_context(input_box), GTK_STYLE_PROVIDER(input_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    return input_box;
}


GtkWidget* create_input_box_with_date_picker(const gchar *placeholder) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);  
    GtkWidget *input_box = create_input_box(placeholder);  
    GtkWidget *calendar = gtk_calendar_new();  

    
    gtk_widget_set_size_request(calendar, 210, 90);  

    
    gtk_box_pack_start(GTK_BOX(box), input_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), calendar, FALSE, FALSE, 0);

    
    g_signal_connect(input_box, "focus-in-event", G_CALLBACK(on_input_box_focus_in), calendar);

    
    g_signal_connect(calendar, "day-selected", G_CALLBACK(on_calendar_day_selected), input_box);
    
    g_signal_connect(gtk_widget_get_toplevel(calendar), "realize", G_CALLBACK(on_window_realize), calendar);
    return box;
}



GtkWidget* create_label(const gchar *text) {
    GtkWidget *label = gtk_label_new(text);
    
    GtkCssProvider *label_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(label_provider,
        "label {"
        "  font-family: 'DM Sans', sans-serif;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  letter-spacing: 0.06em;"
        "  text-align: center;"
        "  color: #283841;"
        "  height: 21px;"
        "  line-height: 20.83px;"
        "}",
        -1, NULL);

    gtk_style_context_add_provider(gtk_widget_get_style_context(label), GTK_STYLE_PROVIDER(label_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    return label;
}

GtkWidget* create_selection_box() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_margin_top(box, 350);   
    gtk_widget_set_margin_start(box, 30);  
    gtk_widget_set_margin_end(box, 30);    
    GtkCssProvider *box_provider = gtk_css_provider_new();
    
    for (int i = 0; i < 5; i++) {
        GtkWidget *section_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
        gtk_widget_set_size_request(section_box, -1, 40); 

        if (i == 0) {
            GtkWidget *label_from = create_label("From");

            input_from = gtk_combo_box_text_new(); 
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_from), "Please choose an airport");
            for (int i = 0; i < airport_count; i++) {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_from), airports[i]);   
            }
            gtk_combo_box_set_active(GTK_COMBO_BOX(input_from), 0); 

            // Tạo và áp dụng CSS cho input_from
            GtkCssProvider *input_from_provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(input_from_provider,
                "combobox {"
                "  font-family: 'DM Sans', sans-serif;"
                "  font-size: 16px;"
                "  font-weight: 500;"
                "  letter-spacing: 0.06em;"
                "  text-align: center;"
                "  border-radius: 6px;"
                "  padding: 12px 20px;"
                "  background-color: white;"
                "  border: 1px solid #ccc;"
                "  color: #283841;"
                "}",          
                -1, NULL);
            gtk_style_context_add_provider(gtk_widget_get_style_context(input_from), GTK_STYLE_PROVIDER(input_from_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            gtk_box_pack_start(GTK_BOX(section_box), label_from, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(section_box), input_from, FALSE, FALSE, 0);

            GtkWidget *label_to = create_label("To");
            input_to = gtk_combo_box_text_new(); 
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_to), "Please choose an airport");
            for (int i = 0; i < airport_count; i++) {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_to), airports[i]);   
            }
            gtk_combo_box_set_active(GTK_COMBO_BOX(input_to), 0); 

            // Tạo và áp dụng CSS cho input_to
            GtkCssProvider *input_to_provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(input_to_provider,
                "combobox {"
                "  font-family: 'DM Sans', sans-serif;"
                "  font-size: 16px;"
                "  font-weight: 500;"
                "  letter-spacing: 0.06em;"
                "  text-align: center;"
                "  border-radius: 6px;"
                "  padding: 12px 20px;"
                "  background-color: white;"
                "  border: 1px solid #ccc;"
                "  color: #283841;"
                "}",          
                -1, NULL);
            gtk_style_context_add_provider(gtk_widget_get_style_context(input_to), GTK_STYLE_PROVIDER(input_to_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            gtk_box_pack_start(GTK_BOX(section_box), label_to, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(section_box), input_to, FALSE, FALSE, 0);
        }        
        else if (i == 1) {
            GtkWidget *label_departure = create_label("Departure");
            input_departure = create_input_box_with_date_picker("Choose Departure Date");

            // GtkWidget *label_return = create_label("Return");
            // input_return = create_input_box_with_date_picker("+ Add Return Date (Optional)");

            
            gtk_box_pack_start(GTK_BOX(section_box), label_departure, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(section_box), input_departure, FALSE, FALSE, 0);
            // gtk_box_pack_start(GTK_BOX(section_box), label_return, FALSE, FALSE, 0);
            // gtk_box_pack_start(GTK_BOX(section_box), input_return, FALSE, FALSE, 0);
        }
        else if (i == 2) {
            GtkWidget *label_traveller = create_label("Number people");
            input_traveller = create_input_box("Choose number people");

            gtk_box_pack_start(GTK_BOX(section_box), label_traveller, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(section_box), input_traveller, FALSE, FALSE, 0);
        }
        else if (i == 3) {
            GtkWidget *label_class = create_label("Class");

            
            combo_box = gtk_combo_box_text_new();
            
            
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Choose Class");
            
            
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Economy");
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Business");
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "First Class");

            
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);  

            
            GtkCssProvider *combo_provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(combo_provider,
                "combobox {"
                "  font-family: 'DM Sans', sans-serif;"
                "  font-size: 16px;"
                "  font-weight: 500;"
                "  letter-spacing: 0.06em;"
                "  text-align: center;"
                "  border-radius: 6px;"
                "  padding: 12px 20px;"
                "  background-color: white;"
                "  border: 1px solid #ccc;"
                "  color: #283841;"
                "}",          
                -1, NULL);

            gtk_style_context_add_provider(gtk_widget_get_style_context(combo_box), GTK_STYLE_PROVIDER(combo_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            
            gtk_box_pack_start(GTK_BOX(section_box), label_class, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(section_box), combo_box, FALSE, FALSE, 0);
        }
        else if (i == 4) {
        
            GtkWidget *search_button = gtk_button_new_with_label("Search Flight");
            gtk_widget_set_name(search_button, "search_button");
        g_signal_connect(search_button, "clicked", G_CALLBACK(on_list_link_click),homepage_window);
            gtk_widget_set_size_request(search_button, 100, 48);

            
            GtkCssProvider *search_provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(search_provider,
                "button {"
                "  font-family: 'Poppins', sans-serif;"
                "  font-size: 16px;"
                "  font-weight: 600;"
                "  letter-spacing: 0.06em;"
                "  color: white;"
                "  background: #223A60;"
                "  border-radius: 8px;"
                "  text-align: center;"
                "  padding: 16px;"
                "}",
                -1, NULL);

            gtk_style_context_add_provider(gtk_widget_get_style_context(search_button), GTK_STYLE_PROVIDER(search_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_widget_set_margin_top(search_button, 35);   
            gtk_box_pack_start(GTK_BOX(section_box), search_button, FALSE, FALSE, 0);

            label_status = gtk_label_new("");
            gtk_box_pack_start(GTK_BOX(section_box), label_status, FALSE, FALSE, 0);
            GtkCssProvider *css_provider_label = gtk_css_provider_new();
            gtk_css_provider_load_from_data(css_provider_label,
                "label {"
                "   color: red;"  // Đặt màu chữ thành đỏ
                "}",
                -1, NULL);

            // Áp dụng CSS cho label
            gtk_style_context_add_provider(gtk_widget_get_style_context(label_status),
                GTK_STYLE_PROVIDER(css_provider_label), GTK_STYLE_PROVIDER_PRIORITY_USER);

        }

        
        gtk_box_pack_start(GTK_BOX(box), section_box, TRUE, TRUE, 0);
    }

    return box;
}


GtkWidget* create_homepage_window() {
    GtkWidget *main_box, *header, *overlay, *darea, *selection_box;

    // Tạo một hộp chính chứa nội dung của trang homepage
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(main_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(main_box, GTK_ALIGN_FILL);

    // Tạo phần header
    GtkWidget *buttons[4];
    header = create_header(buttons, main_box);
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

    // Tạo overlay
    overlay = gtk_overlay_new();
    gtk_box_pack_start(GTK_BOX(main_box), overlay, TRUE, TRUE, 0);
    gtk_widget_set_halign(overlay, GTK_ALIGN_FILL);
    gtk_widget_set_valign(overlay, GTK_ALIGN_FILL);

    // Vùng vẽ
    darea = gtk_drawing_area_new();
    g_signal_connect(darea, "draw", G_CALLBACK(on_draw_event), NULL);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), darea);
    gtk_widget_set_halign(darea, GTK_ALIGN_FILL);
    gtk_widget_set_valign(darea, GTK_ALIGN_FILL);

    // Tạo hộp lựa chọn
    selection_box = create_selection_box();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), selection_box);
    gtk_widget_set_halign(selection_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(selection_box, GTK_ALIGN_FILL);

    return main_box;
}
