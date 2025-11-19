#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <time.h>
#include <signal.h>

#define PORT 8888
#define RUNTIME_SECONDS 900 

volatile sig_atomic_t stop_server = 0;

void handle_signal(int signal) {
    stop_server = 1;
}

int iterate_querystring(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
    int *condition_met = (int *)cls; 
    if (key && value) {
        printf("Key: %s, Value: %s\n", key, value);
        if (strcmp(key, "vnp_ResponseCode") == 0 && strcmp(value, "00") == 0) {
            printf("Condition met: vnp_ResponseCode == 00\n");
            *condition_met = 1; 
        }
    }
    return MHD_YES; 
}

int handle_request(void *cls, struct MHD_Connection *connection, 
                   const char *url, const char *method, const char *version, 
                   const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") != 0) {
        return MHD_NO; 
    }

    printf("URL: %s\n", url);

    int condition_met = 0; 
    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, &iterate_querystring, &condition_met);

    const char *response;
    if (condition_met) {
        printf("Stopping server due to condition met: vnp_ResponseCode == 00\n");
        response = "Please return to your app to continue"; 
        stop_server = 1; 
    } else {
        response = "Please return to your app to continue";
    }

    struct MHD_Response *http_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
    MHD_destroy_response(http_response);

    return ret;
}

int main() {
    struct MHD_Daemon *daemon;

    signal(SIGINT, handle_signal);

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &handle_request, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start HTTP server.\n");
        return 1;
    }

    printf("HTTP server running on port %d...\n", PORT);

    time_t start_time = time(NULL); 

    while (!stop_server) {
        if (time(NULL) - start_time >= RUNTIME_SECONDS) { 
            printf("Server ran for %d seconds. Stopping...\n", RUNTIME_SECONDS);
            break;
        }
        sleep(1); 
    }

    MHD_stop_daemon(daemon);
    printf("HTTP server stopped.\n");
    return 0;
}
