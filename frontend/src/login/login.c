#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "login.h"
#include "../auth/auth.h"
#include "../homepage/homepage.h"
#include "../register/register.h"
#include "../global/global.h"

// Giả sử mấy biến này đã được khai báo extern trong login.h / global.h:
// extern GtkWidget *entry_email;
// extern GtkWidget *entry_password;
// extern GtkWidget *label_status;
// extern int sock;
// extern char buffer[MAX_LENGTH];
// extern char email_user[MAX_LENGTH];
// extern int user_id;

void on_login_clicked(GtkWidget *widget, gpointer data) {
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));

    if (strlen(email) == 0 || strlen(password) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Please fill in all fields!");
        return;
    }

    if (!validate_email(email)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Email is not valid!");
        return;
    }

    strcpy(email_user, email);
    snprintf(buffer, MAX_LENGTH, "LOGIN %s:%s", email, password);
    send(sock, buffer, sizeof(buffer), 0);
    g_print("Sent to server: %s\n", buffer);
    recv(sock, buffer, sizeof(buffer), 0);
    g_print("Received from server: %s\n", buffer);

    if (strncmp(buffer, "SUCCESS:", 8) == 0) {
        sscanf(buffer + 8, "%d", &user_id);
        printf("User ID: %d\n", user_id);

        GtkWidget *homepage_widget = create_homepage_window();
        set_content(homepage_widget);
    } else if (strcmp(buffer, "EMAIL_NOT_FOUND") == 0) {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Email not found, please register email");
        return;
    } else {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Login failed, please try again");
        return;
    }
}

void on_register_link_click(GtkWidget *widget, gpointer data) {
    GtkWidget *register_content = create_register_window();
    set_content(register_content);
}

GtkWidget *create_login_window(void) {
    GtkWidget *login_box, *button_login;

    /* ===== CSS giống hình mẫu ===== */
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        provider,
        /* Gradient background + card + controls */
        "window {"
        "   background-image: linear-gradient(135deg, #ff4081, #9c27b0, #303f9f);"
        "   background-size: cover;"
        "   background-position: center;"
        "}\n"

        "#login-box {"
        "   background-color: #FFFFFF;"
        "   border-radius: 12px;"
        "   padding: 40px 48px;"
        "   min-width: 380px;"
        "   box-shadow: 0 12px 35px rgba(0,0,0,0.18);"
        "}\n"

        "#login-title {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 26px;"
        "   font-weight: 700;"
        "   color: #202124;"
        "   margin-bottom: 24px;"
        "}\n"

        "#email-label, #password-label {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   color: #5f6368;"
        "   margin-top: 8px;"
        "   margin-bottom: 4px;"
        "}\n"

        "#email-entry, #password-entry {"
        "   border-radius: 4px;"
        "   padding: 10px 12px;"
        "   border: 1px solid #dadce0;"
        "   background-color: #f1f3f4;"
        "   color: #202124;"
        "}\n"

        "#email-entry:focus, #password-entry:focus {"
        "   border-color: #c2185b;"
        "   background-color: #ffffff;"
        "   box-shadow: 0 0 0 1px rgba(194,24,91,0.35);"
        "}\n"

        "#remember-label {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 13px;"
        "   color: #5f6368;"
        "}\n"

        "#login-button {"
        "   background-color: #e91e63;"
        "   color: #ffffff;"
        "   font-family: Poppins, Sans-Serif;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "   border-radius: 4px;"
        "   padding: 12px 0;"
        "   margin-top: 16px;"
        "}\n"

        "#login-button:hover {"
        "   background-color: #d81b60;"
        "}\n"

        "#status-label {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 12px;"
        "   color: #b00020;"
        "   margin-top: 8px;"
        "}\n"

        "#footer-box {"
        "   margin-top: 24px;"
        "}\n"

        "#footer-text {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 13px;"
        "   color: #9e9e9e;"
        "}\n"

        "#sign-up {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 13px;"
        "   color: #e91e63;"
        "   background-color: transparent;"
        "   border: none;"
        "   box-shadow: none;"
        "   padding: 0;"
        "}\n"

        "#sign-up:hover {"
        "   text-decoration: underline;"
        "}\n",
        -1,
        NULL
    );

    /* Áp CSS cho toàn bộ screen */
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    /* ===== Layout card login ===== */
    login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(login_box, "login-box");
    gtk_widget_set_halign(login_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(login_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(login_box, 40);
    gtk_widget_set_margin_bottom(login_box, 40);

    GtkStyleContext *login_box_context =
        gtk_widget_get_style_context(login_box);
    gtk_style_context_add_provider(
        login_box_context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    /* Title: LOGIN */
    GtkWidget *label_login_title = gtk_label_new("LOGIN");
    gtk_widget_set_name(label_login_title, "login-title");
    gtk_widget_set_halign(label_login_title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(login_box), label_login_title,
                       FALSE, FALSE, 0);

    /* Email */
    GtkWidget *label_email = gtk_label_new("Email");
    gtk_widget_set_name(label_email, "email-label");
    gtk_widget_set_halign(label_email, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(login_box), label_email,
                       FALSE, FALSE, 8);

    entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_widget_set_name(entry_email, "email-entry");
    gtk_box_pack_start(GTK_BOX(login_box), entry_email,
                       FALSE, FALSE, 0);

    /* Password */
    GtkWidget *label_password = gtk_label_new("Password");
    gtk_widget_set_name(label_password, "password-label");
    gtk_widget_set_halign(label_password, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(login_box), label_password,
                       FALSE, FALSE, 8);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_name(entry_password, "password-entry");
    gtk_box_pack_start(GTK_BOX(login_box), entry_password,
                       FALSE, FALSE, 0);

    /* Remember me */
    GtkWidget *remember_box =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *remember_check = gtk_check_button_new();
    GtkWidget *remember_label =
        gtk_label_new("Remember me");
    gtk_widget_set_name(remember_label, "remember-label");

    gtk_box_pack_start(GTK_BOX(remember_box), remember_check,
                       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(remember_box), remember_label,
                       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(login_box), remember_box,
                       FALSE, FALSE, 8);
    /* (hiện tại checkbox chỉ mang tính UI, chưa dùng logic) */

    /* Status label (error message) */
    label_status = gtk_label_new("");
    gtk_widget_set_name(label_status, "status-label");
    gtk_widget_set_halign(label_status, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(login_box), label_status,
                       FALSE, FALSE, 0);

    /* Nút LOGIN */
    button_login = gtk_button_new_with_label("LOGIN");
    gtk_widget_set_name(button_login, "login-button");
    gtk_widget_set_hexpand(button_login, TRUE);
    g_signal_connect(button_login, "clicked",
                     G_CALLBACK(on_login_clicked), login_box);
    gtk_box_pack_start(GTK_BOX(login_box), button_login,
                       FALSE, FALSE, 0);

    /* Footer: Not a member? Sign up now */
    GtkWidget *footer_box =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_name(footer_box, "footer-box");
    gtk_widget_set_halign(footer_box, GTK_ALIGN_CENTER);

    GtkWidget *footer_text =
        gtk_label_new("Not a member?");
    gtk_widget_set_name(footer_text, "footer-text");
    gtk_box_pack_start(GTK_BOX(footer_box), footer_text,
                       FALSE, FALSE, 0);

    GtkWidget *sign_up =
        gtk_button_new_with_label("Sign up now");
    gtk_widget_set_name(sign_up, "sign-up");
    g_signal_connect(sign_up, "clicked",
                     G_CALLBACK(on_register_link_click), login_box);
    gtk_box_pack_start(GTK_BOX(footer_box), sign_up,
                       FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(login_box), footer_box,
                       FALSE, FALSE, 0);

    return login_box;
}
