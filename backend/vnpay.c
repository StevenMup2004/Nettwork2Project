#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <curl/curl.h>

// Hàm mã hóa URL
void url_encode(const char *src, char *dest, size_t dest_len) {
    size_t i, j = 0;
    for (i = 0; src[i] != '\0' && j < dest_len - 1; i++) {
        if (isalnum((unsigned char)src[i]) || src[i] == '-' || src[i] == '_' || src[i] == '.' || src[i] == '~') {
            dest[j++] = src[i];
        } else {
            if (j + 3 >= dest_len) break;
            snprintf(&dest[j], 4, "%%%02X", (unsigned char)src[i]);
            j += 3;
        }
    }
    dest[j] = '\0';
}

// Hàm tính toán HMAC-SHA256
char *hmac_sha512(const char *key, const char *data) {
    unsigned char *result;
    unsigned int len = 64;  // HMAC-SHA512 trả về 64 byte
    result = (unsigned char *)malloc(len);
    HMAC(EVP_sha512(), key, strlen(key), (unsigned char *)data, strlen(data), result, &len);

    char *hex_result = (char *)malloc(len * 2 + 1);
    for (unsigned int i = 0; i < len; i++) {
        sprintf(hex_result + (i * 2), "%02x", result[i]);
    }
    hex_result[len * 2] = '\0';
    free(result);
    return hex_result;
}

// Hàm lấy thời gian hiện tại
void get_current_time(char *buffer, size_t buffer_size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, buffer_size, "%Y%m%d%H%M%S", tm_info);
}

// Hàm gửi yêu cầu HTTP POST đến VNPAY
void send_payment_request(const char *payment_url, const char *data) {
    CURL *curl;
    CURLcode res;

    // Khởi tạo libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Đặt URL API VNPAY
        curl_easy_setopt(curl, CURLOPT_URL, payment_url);
        
        // Đặt kiểu request POST và gửi dữ liệu
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        // Thêm header Content-Type là application/x-www-form-urlencoded
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Gửi yêu cầu và nhận kết quả
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Payment request sent successfully.\n");
        }

        // Dọn dẹp
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main() {
    // Các tham số cấu hình
    const char *vnp_TmnCode = "MZHS6NEZ";  // Thay bằng mã Merchant được cung cấp
    const char *vnp_HashSecret = "OLMEGIFJE0ODTO3SZ0PETBRGUFW2H3FW"; // Thay bằng mã bí mật được cung cấp
    const char *vnp_Url = "https://sandbox.vnpayment.vn/paymentv2/vpcpay.html"; // URL thanh toán của VNPAY
    
    // Dữ liệu giao dịch
    const char *vnp_TxnRef = "12"; 
    const char *vnp_Amount = "10000000"; // Số tiền (10000000 VND)
    const char *vnp_CreateDate = "20241230203800"; // Thời gian tạo giao dịch
    const char *vnp_CurrCode = "VND"; // Loại tiền tệ
    const char *vnp_IpAddr = "127.0.0.1"; // Địa chỉ IP
    const char *vnp_Locale = "vn"; // Ngôn ngữ
    const char *vnp_OrderInfo = "Thanhtoan"; // Thông tin đơn hàng
    const char *vnp_OrderType = "other"; // Loại hàng hóa
    const char *vnp_ReturnUrl = "http://localhost:8888/vnpay_return"; // URL trả về
    const char *vnp_BankCode = "NCB"; 

    // Mã hóa các tham số cần thiết
    char encoded_OrderInfo[512];
    char encoded_ReturnUrl[512];
    url_encode(vnp_OrderInfo, encoded_OrderInfo, sizeof(encoded_OrderInfo));
    url_encode(vnp_ReturnUrl, encoded_ReturnUrl, sizeof(encoded_ReturnUrl));

    // Tạo chuỗi tham số
    char params[2048];
    snprintf(params, sizeof(params),
        "vnp_Amount=%s&vnp_BankCode=%s&vnp_Command=pay&vnp_CreateDate=%s&vnp_CurrCode=%s"
        "&vnp_IpAddr=%s&vnp_Locale=%s&vnp_OrderInfo=%s&vnp_OrderType=%s&vnp_ReturnUrl=%s&vnp_TmnCode=%s"
        "&vnp_TxnRef=%s&vnp_Version=2.1.0",
        vnp_Amount, vnp_BankCode, vnp_CreateDate, vnp_CurrCode, vnp_IpAddr, vnp_Locale,
        encoded_OrderInfo, vnp_OrderType, encoded_ReturnUrl, vnp_TmnCode, vnp_TxnRef);

    // Tính toán chữ ký
    char *secure_hash = hmac_sha512(vnp_HashSecret, params);

    // Tạo URL thanh toán
    char payment_url[4096];
    snprintf(payment_url, sizeof(payment_url), "%s?%s&vnp_SecureHash=%s", vnp_Url, params, secure_hash);

    // In URL thanh toán
    printf("Payment URL: %s\n", payment_url);

    // Gửi yêu cầu thanh toán
    send_payment_request(payment_url, params);

    // Giải phóng bộ nhớ
    free(secure_hash);

    return 0;
}
