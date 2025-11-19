#ifndef LIST_H
#define LIST_H

#include <gtk/gtk.h>


// Khai báo các hàm
GtkWidget* create_ticket_list();
GtkWidget* create_filter_box();
void on_detail_link_click(GtkWidget *widget, gpointer data);
void sort_flights(gboolean ascending);  
void on_sort_changed(GtkComboBox *combo, gpointer user_data); 
void refresh_ticket_list(GtkWidget *container);
GtkWidget* create_list_window();
#endif