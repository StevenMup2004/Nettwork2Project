#ifndef LOGIN_H
#define LOGIN_H

#include <gtk/gtk.h>

GtkWidget *create_login_window();
void on_login_clicked(GtkWidget *widget, gpointer data);
void on_register_link_click(GtkWidget *widget, gpointer data);

#endif 
