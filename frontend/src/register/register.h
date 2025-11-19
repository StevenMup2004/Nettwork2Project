#ifndef REGISTER_H
#define REGISTER_H

#include <gtk/gtk.h>

void on_register(GtkWidget *widget, gpointer data);
GtkWidget* create_register_window();
void on_login_link_click (GtkWidget *widget, gpointer data);

#endif 
