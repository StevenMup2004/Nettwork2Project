// homepage_search.h

#ifndef HOMEPAGE_SEARCH_H
#define HOMEPAGE_SEARCH_H

#include <gtk/gtk.h>   // Để sử dụng GTK+ và gchar
#include <glib.h>       // Để sử dụng gch
// Cấu trúc thông tin chuyến bay
typedef struct {
    const gchar *from;
    const gchar *to;
    const gchar *departure_date;
    const gchar *return_date;
    const gchar *traveller_count;
    const gchar *class_selected;
} FlightInfo;

// Khai báo hàm tạo cửa sổ pop-up hiển thị kết quả tìm kiếm
void create_popup_window(FlightInfo *flight_info);

#endif // HOMEPAGE_SEARCH_H
