#ifndef TICKET_DETAIL_H
#define TICKET_DETAIL_H

#include <gtk/gtk.h>

GtkWidget* create_ticket_detail_window();
gboolean on_cancel_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

#endif // TICKET_DETAIL_H
