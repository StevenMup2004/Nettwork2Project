#include <gtk/gtk.h>
#include <regex.h>
#include <string.h>
#include <arpa/inet.h>
#include "register.h"
#include "../auth/auth.h"
#include "../login/login.h"
#include "../global/global.h"

GtkWidget *entry_confirm_password;
GtkWidget *entry_username;
GtkWidget *entry_phone;

/* ===== CALLBACKS ===== */

void on_register(GtkWidget *widget, gpointer data) {
    const char *email             = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *password          = gtk_entry_get_text(GTK_ENTRY(entry_password));
    const char *confirm_password  = gtk_entry_get_text(GTK_ENTRY(entry_confirm_password));
    const char *username          = gtk_entry_get_text(GTK_ENTRY(entry_username));
    const char *phone             = gtk_entry_get_text(GTK_ENTRY(entry_phone));

    if (strlen(email) == 0 || strlen(password) == 0 ||
        strlen(confirm_password) == 0 || strlen(username) == 0 ||
        strlen(phone) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "All fields are required.");
        return;
    }

    if (!validate_phone(phone)) {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Phone must be 10–15 digits.");
        return;
    }

    if (!validate_email(email)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Email is invalid.");
        return;
    }

    if (!validate_password(password)) {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Password must be ≥ 8 chars, include uppercase and a number.");
        return;
    }

    if (strcmp(password, confirm_password) != 0) {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Confirm password does not match.");
        return;
    }

    snprintf(buffer, MAX_LENGTH,
             "REGISTER %s:%s:%s:%s", username, phone, email, password);
    send(sock, buffer, sizeof(buffer), 0);
    g_print("Sent to server: %s\n", buffer);
    recv(sock, buffer, sizeof(buffer), 0);
    g_print("Received from server: %s\n", buffer);

    if (strcmp(buffer, "SUCCESS") == 0) {
        GtkWidget *login_content = create_login_window();
        set_content(login_content);
    } else if (strcmp(buffer, "EMAIL_EXISTED") == 0) {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Email already exists.");
        return;
    } else {
        gtk_label_set_text(GTK_LABEL(label_status),
                           "Registration failed, please try again.");
        return;
    }
}

void on_login_link_click(GtkWidget *widget, gpointer data) {
    GtkWidget *login_content = create_login_window();
    set_content(login_content);
}

/* ===== UI ===== */

GtkWidget* create_register_window(void) {
    /* CSS giống style login: gradient + card trắng */
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        provider,
        "window {"
        "   background-image: linear-gradient(135deg, #ff4081, #9c27b0, #303f9f);"
        "   background-size: cover;"
        "   background-position: center;"
        "}\n"

        "#register-box {"
        "   background-color: #FFFFFF;"
        "   border-radius: 12px;"
        "   padding: 40px 48px;"
        "   min-width: 420px;"
        "   box-shadow: 0 12px 35px rgba(0,0,0,0.18);"
        "}\n"

        "#register-title {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 24px;"
        "   font-weight: 700;"
        "   color: #202124;"
        "   margin-bottom: 16px;"
        "}\n"

        "#email-label, #password-label, #username-label,"
        "#phone-label, #confirm-password-label {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   color: #5f6368;"
        "   margin-top: 8px;"
        "   margin-bottom: 4px;"
        "}\n"

        "#email-entry, #password-entry, #username-entry,"
        "#phone-entry, #confirm-password-entry {"
        "   border-radius: 4px;"
        "   padding: 10px 12px;"
        "   border: 1px solid #dadce0;"
        "   background-color: #f1f3f4;"
        "   color: #202124;"
        "}\n"

        "#email-entry:focus, #password-entry:focus, #username-entry:focus,"
        "#phone-entry:focus, #confirm-password-entry:focus {"
        "   border-color: #c2185b;"
        "   background-color: #ffffff;"
        "   box-shadow: 0 0 0 1px rgba(194,24,91,0.35);"
        "}\n"

        "#register-button {"
        "   background-color: #e91e63;"
        "   color: #ffffff;"
        "   font-family: Poppins, Sans-Serif;"
        "   font-weight: 600;"
        "   font-size: 14px;"
        "   border-radius: 4px;"
        "   padding: 12px 0;"
        "   margin-top: 18px;"
        "}\n"

        "#register-button:hover {"
        "   background-color: #d81b60;"
        "}\n"

        "#status-label {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 12px;"
        "   color: #b00020;"
        "   margin-top: 8px;"
        "}\n"

        "#footer-text {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 13px;"
        "   color: #9e9e9e;"
        "}\n"

        "#login-link {"
        "   font-family: Poppins, Sans-Serif;"
        "   font-size: 13px;"
        "   color: #e91e63;"
        "   background-color: transparent;"
        "   border: none;"
        "   box-shadow: none;"
        "   padding: 0;"
        "}\n"

        "#login-link:hover {"
        "   text-decoration: underline;"
        "}\n",
        -1,
        NULL
    );

    /* áp dụng CSS cho toàn app */
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    /* Card đăng ký */
    GtkWidget *register_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(register_box, "register-box");
    gtk_widget_set_halign(register_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(register_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(register_box, 40);
    gtk_widget_set_margin_bottom(register_box, 40);

    GtkStyleContext *box_ctx = gtk_widget_get_style_context(register_box);
    gtk_style_context_add_provider(
        box_ctx,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    /* Title */
    GtkWidget *label_register_title = gtk_label_new("Create an account");
    gtk_widget_set_name(label_register_title, "register-title");
    gtk_widget_set_halign(label_register_title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(register_box), label_register_title,
                       FALSE, FALSE, 0);

    /* Username */
    GtkWidget *label_username = gtk_label_new("Username");
    gtk_widget_set_name(label_username, "username-label");
    gtk_widget_set_halign(label_username, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(register_box), label_username,
                       FALSE, FALSE, 8);

    entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username),
                                   "Enter your username");
    gtk_widget_set_name(entry_username, "username-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_username,
                       FALSE, FALSE, 0);

    /* Phone */
    GtkWidget *label_phone = gtk_label_new("Phone number");
    gtk_widget_set_name(label_phone, "phone-label");
    gtk_widget_set_halign(label_phone, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(register_box), label_phone,
                       FALSE, FALSE, 8);

    entry_phone = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_phone),
                                   "Enter your phone number");
    gtk_widget_set_name(entry_phone, "phone-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_phone,
                       FALSE, FALSE, 0);

    /* Email */
    GtkWidget *label_email = gtk_label_new("Email");
    gtk_widget_set_name(label_email, "email-label");
    gtk_widget_set_halign(label_email, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(register_box), label_email,
                       FALSE, FALSE, 8);

    entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email),
                                   "Enter your email");
    gtk_widget_set_name(entry_email, "email-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_email,
                       FALSE, FALSE, 0);

    /* Password */
    GtkWidget *label_password = gtk_label_new("Password");
    gtk_widget_set_name(label_password, "password-label");
    gtk_widget_set_halign(label_password, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(register_box), label_password,
                       FALSE, FALSE, 8);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password),
                                   "Enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_name(entry_password, "password-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_password,
                       FALSE, FALSE, 0);

    /* Confirm password */
    GtkWidget *label_confirm_password = gtk_label_new("Confirm password");
    gtk_widget_set_name(label_confirm_password, "confirm-password-label");
    gtk_widget_set_halign(label_confirm_password, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(register_box), label_confirm_password,
                       FALSE, FALSE, 8);

    entry_confirm_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_confirm_password),
                                   "Re-enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_confirm_password), FALSE);
    gtk_widget_set_name(entry_confirm_password, "confirm-password-entry");
    gtk_box_pack_start(GTK_BOX(register_box), entry_confirm_password,
                       FALSE, FALSE, 0);

    /* Status label dùng chung */
    label_status = gtk_label_new("");
    gtk_widget_set_name(label_status, "status-label");
    gtk_widget_set_halign(label_status, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(register_box), label_status,
                       FALSE, FALSE, 8);

    /* Nút tạo tài khoản */
    GtkWidget *button_register =
        gtk_button_new_with_label("Create account");
    gtk_widget_set_name(button_register, "register-button");
    gtk_widget_set_hexpand(button_register, TRUE);
    g_signal_connect(button_register, "clicked",
                     G_CALLBACK(on_register), register_box);
    gtk_box_pack_start(GTK_BOX(register_box), button_register,
                       FALSE, FALSE, 0);

    /* Footer: Already have an account? Log in */
    GtkWidget *hbox_footer =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *footer_text =
        gtk_label_new("Already have an account?");
    gtk_widget_set_name(footer_text, "footer-text");
    gtk_box_pack_start(GTK_BOX(hbox_footer), footer_text,
                       FALSE, FALSE, 0);

    GtkWidget *login_button =
        gtk_button_new_with_label("Log in");
    gtk_widget_set_name(login_button, "login-link");
    g_signal_connect(login_button, "clicked",
                     G_CALLBACK(on_login_link_click), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_footer), login_button,
                       FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(register_box), hbox_footer,
                       FALSE, FALSE, 16);

    return register_box;
}
