#include "email.h"
#include <string.h>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include <time.h>

#define SMTP_SERVER "smtp://smtp.gmail.com"
#define SMTP_PORT "587"
#define SENDER_EMAIL "nguyenthuydung55555@gmail.com"      
#define SENDER_PASSWORD "jehb zesq ydaa dwso"       
#define RECIPIENT_EMAIL "nguyenthithuydung17102003@gmail.com"        
#define EMAIL_SUBJECT "Chúc mừng đặt vé thành công"
#define EMAIL_BODY "Congratulations on successfully booking your ticket!"
#define TICKET_CODE_LENGTH 5
struct upload_status {
    size_t bytes_read;
};

void generate_ticket_code(char *ticket_code, int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        ticket_code[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    ticket_code[length] = '\0'; 
}

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    srand(time(NULL)); 

    char ticket_code[TICKET_CODE_LENGTH + 1];
    generate_ticket_code(ticket_code, TICKET_CODE_LENGTH);

    char data[512]; 

    snprintf(data, sizeof(data), 
             "To: nguyenthithuydung17102003@gmail.com\r\n"
             "From: TicketTrail <ticketrail@gmail.com>\r\n"
             "Subject: Test Email\r\n"
             "\r\n"
             "Chúc mừng đặt vé thành công.\r\n"
             "Mã đặt vé của bạn là: %s\r\n",
             ticket_code);
    size_t len = strlen(data);

    if (upload_ctx->bytes_read < len) {
        size_t to_copy = len - upload_ctx->bytes_read;
        if (to_copy > size * nmemb) {
            to_copy = size * nmemb;
        }

        memcpy(ptr, data + upload_ctx->bytes_read, to_copy);
        upload_ctx->bytes_read += to_copy;
        return to_copy;
    }

    return 0; 
}

int send_email() {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = {0};

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, SENDER_EMAIL);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, SENDER_PASSWORD);
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, SENDER_EMAIL);

        recipients = curl_slist_append(recipients, RECIPIENT_EMAIL);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
    return (res == CURLE_OK) ? 0 : 1;
}

// Xử lý nút Email
static void on_email_button_clicked(GtkWidget *widget, gpointer user_data) {
    if (send_email() == 0) {
        g_print("Email sent successfully!\n");
    } else {
        g_print("Failed to send email.\n");
    }
}

// Hàm vẽ giao diện (tuỳ chọn)
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // Xám nhạt
    cairo_paint(cr);
    return FALSE;
}

// Tạo giao diện email
GtkWidget *create_email_screen() {
    GtkWidget *overlay = gtk_overlay_new();

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), drawing_area);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), NULL);

    GtkWidget *button = gtk_button_new_with_label("Email");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), button);
    gtk_widget_set_size_request(button, 150, 50);
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);

    g_signal_connect(button, "clicked", G_CALLBACK(on_email_button_clicked), NULL);

    return overlay;
}
