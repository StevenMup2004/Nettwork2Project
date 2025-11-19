// homepage_search.c
#include <gtk/gtk.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include "homepage_search.h"

// Định nghĩa hàm tạo cửa sổ pop-up hiển thị kết quả tìm kiếm
void create_popup_window(FlightInfo *flight_info) {
    GtkWidget *popup_window, *popup_box, *label, *flight_details;

    // Tạo cửa sổ mới cho popup
    popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "Flight Search Results");

    // Tạo box chứa nội dung popup
    popup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(popup_window), popup_box);

    // Hiển thị thông tin chuyến bay
    flight_details = gtk_label_new(NULL);
    gchar *flight_details_text = g_strdup_printf(
        "From: %s\nTo: %s\nDeparture Date: %s\nReturn Date: %s\nTravellers: %s\nClass: %s",
        flight_info->from, flight_info->to, flight_info->departure_date, flight_info->return_date, flight_info->traveller_count, flight_info->class_selected
    );
    gtk_label_set_text(GTK_LABEL(flight_details), flight_details_text);
    gtk_box_pack_start(GTK_BOX(popup_box), flight_details, TRUE, TRUE, 0);

    // Tạo nút đóng cửa sổ
    label = gtk_button_new_with_label("Close");
    g_signal_connect(label, "clicked", G_CALLBACK(gtk_widget_destroy), popup_window);
    gtk_box_pack_start(GTK_BOX(popup_box), label, FALSE, FALSE, 0);

    // Hiển thị cửa sổ popup
    gtk_widget_show_all(popup_window);
}
