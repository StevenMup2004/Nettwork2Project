#include "./global/global.h"
#include "./homepage/homepage.h"
#include "./server_com/server_com.h"
#include "./register/register.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h> // *** Quan trọng: Đảm bảo bạn đã include stdio.h ***
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

int main(int argc, char *argv[]) {
  printf("Client starting...\n"); // *** Thêm dòng này ***

  printf("Attempting to connect to server at %s:%d...\n", SERVER_IP, PORT); // *** Thêm dòng này ***
  sock = connect_to_server(SERVER_IP, PORT);
  if (sock < 0) { // *** Thêm kiểm tra lỗi kết nối ***
      fprintf(stderr, "Connection failed in client.c. Exiting.\n");
      return 1; // Thoát nếu không kết nối được
  }
  printf("Connected successfully! Socket: %d\n", sock); // *** Thêm dòng này ***

  gtk_init(&argc, &argv);
  printf("GTK Initialized.\n"); // *** Thêm dòng này ***

  GtkWidget *main_window = create_main_window();
  printf("Main window created.\n"); // *** Thêm dòng này ***

  // gtk_widget_show_all(main_window); // Tạm thời comment dòng này để xem các bước sau
  // printf("Showing main window (commented out for now).\n");

  GtkWidget *register_window = create_register_window();
  printf("Register window created.\n"); // *** Thêm dòng này ***

  set_content(register_window);
  printf("Register window content set.\n"); // *** Thêm dòng này ***

  gtk_widget_show_all(main_window); // Hiện cửa sổ chính sau khi đã set content
  printf("Showing main window now.\n"); // *** Thêm dòng này ***

  gtk_main();
  printf("Exiting gtk_main.\n"); // *** Thêm dòng này ***

  close(sock);
  printf("Socket closed.\n"); // *** Thêm dòng này ***
  return 0;
}