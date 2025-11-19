#include "cardinfo.h"
#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include "../global/global.h"

#define BANK_COUNT 5
const char *banks[] = {
    "Vietcombank",
    "Techcombank",
    "Vietinbank",
    "BIDV",
    "Sacombank"
};


static void apply_css(GtkWidget *widget, const char *css_data) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    
    gtk_widget_reset_style(widget);
}


static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("../assets/images/bg_login.png", NULL);
    if (pixbuf) {
        gint width = gtk_widget_get_allocated_width(widget);
        gint height = gtk_widget_get_allocated_height(widget);
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
        cairo_paint(cr);
        g_object_unref(scaled_pixbuf);
        g_object_unref(pixbuf);
    }

    
    double widget_width = gtk_widget_get_allocated_width(widget);
    double widget_height = gtk_widget_get_allocated_height(widget);
    double center_x = (widget_width - 800) / 2.0;
    double center_y = (widget_height - 600) / 2.0;
    double corner_radius = 24.0;

    cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
    cairo_new_path(cr);
    cairo_arc(cr, center_x + corner_radius, center_y + corner_radius, corner_radius, M_PI, 3 * M_PI / 2);
    cairo_arc(cr, center_x + 800 - corner_radius, center_y + corner_radius, corner_radius, 3 * M_PI / 2, 2 * M_PI);
    cairo_arc(cr, center_x + 800 - corner_radius, center_y + 600 - corner_radius, corner_radius, 0, M_PI / 2);
    cairo_arc(cr, center_x + corner_radius, center_y + 600 - corner_radius, corner_radius, M_PI / 2, M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);

    return FALSE;
}


static void on_continue_clicked(GtkWidget *widget, gpointer user_data) {
    GtkWidget **fields = (GtkWidget **)user_data;

    
    if (!fields) {
        g_critical("Fields array is NULL.");
        return;
    }

    int all_filled = 1; 

    
    for (int i = 0; i < 4; i++) {
        
        if (!fields[i]) {
            g_critical("Field %d is NULL.", i);
            all_filled = 0;
            continue;
        }

        
        GtkWidget *error_label = g_object_get_data(G_OBJECT(fields[i]), "error_label");
        if (!error_label || !GTK_IS_LABEL(error_label)) {
            g_critical("Error label for field %d is invalid or not set.", i);
            all_filled = 0;
            continue;
        }

        if (i == 0) { 
            if (!GTK_IS_COMBO_BOX(fields[i])) {
                g_critical("Field %d is not a GtkComboBox.", i);
                all_filled = 0;
                continue;
            }

            if (gtk_combo_box_get_active(GTK_COMBO_BOX(fields[i])) == -1) {
                gtk_label_set_text(GTK_LABEL(error_label), "*You need to select a bank.");
                gtk_widget_show(error_label);
                all_filled = 0;
            } else {
                gtk_widget_hide(error_label);
            }
        } else { 
            if (!GTK_IS_ENTRY(fields[i])) {
                g_critical("Field %d is not a GtkEntry.", i);
                all_filled = 0;
                continue;
            }

            const char *text = gtk_entry_get_text(GTK_ENTRY(fields[i]));
            if (!text || strlen(text) == 0) {
                gtk_label_set_text(GTK_LABEL(error_label), "*You need to fill out this field.");
                gtk_widget_show(error_label);
                all_filled = 0;
            } else {
                gtk_widget_hide(error_label);
            }
        }
    }

    
    if (all_filled) {
        g_print("All fields are filled. Proceeding...\n");
        
    } else {
        g_warning("Validation failed. Check fields and error labels.");
    }
}

GtkWidget *create_cardinfo_screen() {
    GtkWidget *overlay = gtk_overlay_new();

    
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), drawing_area);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), NULL);

    GtkWidget *main_box = gtk_fixed_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), main_box);

    
    const char *labels[] = {"Bank Name", "Card Number", "Cardholder Name", "Expiry Date"};
    GtkWidget *fields[4];

    double form_width = 800.0;
    double form_height = 600.0;

    GtkAllocation allocation;
    gtk_widget_get_allocation(drawing_area, &allocation);

    double widget_width = allocation.width;
    double widget_height = allocation.height;

    double base_x = (widget_width + form_width) / 2.0;
    double base_y = (widget_height + form_height) / 2.0 - 60;

    GdkPixbuf *logo_pixbuf = gdk_pixbuf_new_from_file("../assets/images/logovn.png", NULL);
    if (logo_pixbuf) {
        GtkWidget *logo = gtk_image_new_from_pixbuf(gdk_pixbuf_scale_simple(logo_pixbuf, 120, 120, GDK_INTERP_BILINEAR));
        gtk_fixed_put(GTK_FIXED(main_box), logo, base_x + 270, base_y - 140);
        g_object_unref(logo_pixbuf);
    }

    GtkWidget *title_label = gtk_label_new("Fill in your bank card information");
    gtk_fixed_put(GTK_FIXED(main_box), title_label, base_x + 180, base_y + 10);
    apply_css(title_label,
              "label {"
              "  font-size: 24px;"
              "  font-weight: bold;"
              "  color: #223A60;"
              "}");

for (int i = 0; i < 4; i++) {
    GtkWidget *field_label = gtk_label_new(labels[i]);
    gtk_fixed_put(GTK_FIXED(main_box), field_label, base_x + 30, base_y + i * 100 + 60);
    apply_css(field_label,
              "label {"
              "  font-size: 16px;"
              "  font-weight: bold;"
              "  color: #223A60;"
              "}");

        if (i == 0) {
            fields[i] = gtk_combo_box_text_new();
            for (int j = 0; j < BANK_COUNT; j++) {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(fields[i]), banks[j]);
            }

            
            apply_css(fields[i],
                      "combobox, combobox text {"
                      "  background-color: white;"
                    "  border: 1px solid #223A60;"
                      "  padding: 8px;"
                      "  color: #223A60;"
                      "  font-size: 16px;"
                      "  font-family: 'Inter', sans-serif;"
                      "  border-radius: 8px;"
                      "}");
        } else {
        
        fields[i] = gtk_entry_new();

        
        if (i == 1) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(fields[i]), "196318201901");
        } else if (i == 2) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(fields[i]), "NGUYEN VAN A");
        } else if (i == 3) {
            gtk_entry_set_placeholder_text(GTK_ENTRY(fields[i]), "mm/dd/yyyy");
        }
    }

    
    gtk_widget_set_size_request(fields[i], 400, 40);
    gtk_fixed_put(GTK_FIXED(main_box), fields[i], base_x + 230, base_y + i * 100 + 60);

    
    apply_css(fields[i],
              "entry {"
              "  border: 1px solid #223A60;"
              "  border-radius: 8px;"
              "  padding: 8px;"
              "  color: black;"
             "  font-size: 16px;"
              "}"
);
              
    GtkWidget *error_label = gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(main_box), error_label, base_x + 250, base_y + i * 100 + 110);
    apply_css(error_label,
              "label {"
              "  font-size: 12px;"
              "  color: red;"
              "}");
    gtk_widget_hide(error_label);
    g_object_set_data(G_OBJECT(fields[i]), "error_label", error_label);
}


GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
GtkWidget *continue_button = gtk_button_new_with_label("Continue");

gtk_widget_set_size_request(cancel_button, 180, 50);
gtk_widget_set_size_request(continue_button, 180, 50);

gtk_fixed_put(GTK_FIXED(main_box), cancel_button, base_x + 180, base_y + 450);
gtk_fixed_put(GTK_FIXED(main_box), continue_button, base_x + 380, base_y + 450);


apply_css(cancel_button,
          "button {"
          "  background-color: #223A60;"
          "  color: #223A60;"
          "  border-radius: 8px;"
          "  font-weight: bold;"
          "  padding: 10px 20px;"
          "  font-size: 16px;"
          "}");

apply_css(continue_button,
          "button {"
          "  background-color: #223A60;"
          "  color: #223A60;"
          "  border-radius: 8px;"
          "  font-weight: bold;"
          "  padding: 10px 20px;"
          "  font-size: 16px;"
          "}");


    g_signal_connect(continue_button, "clicked", G_CALLBACK(on_continue_clicked), fields);

    return overlay;
}
