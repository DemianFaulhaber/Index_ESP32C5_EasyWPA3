#include "HTTP_MANAGER.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"

#define READ_CHUNK_SIZE 1024
static const char *TAG = "HTTP_MANAGER";

// Lee la respuesta completa dinámicamente
static char *http_read_all(esp_http_client_handle_t client, int *out_len) {
    int content_length = esp_http_client_fetch_headers(client);
    int total_size = (content_length > 0) ? content_length + 1 : READ_CHUNK_SIZE;
    char *buffer = malloc(total_size);
    if (!buffer) { *out_len = 0; return NULL; }

    int total_read = 0;
    int read_len;
    while ((read_len = esp_http_client_read(client, buffer + total_read, total_size - total_read - 1)) > 0) {
        total_read += read_len;
        if (total_read >= total_size - 1) {
            total_size *= 2;
            char *tmp = realloc(buffer, total_size);
            if (!tmp) { free(buffer); *out_len = 0; return NULL; }
            buffer = tmp;
        }
    }
    buffer[total_read] = '\0';
    *out_len = total_read;
    return buffer;
}

void http_get_request(const char *url, cJSON **response) {
    *response = NULL;

    esp_http_client_config_t config = {
        .url = url,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t http_inst = esp_http_client_init(&config);
    esp_http_client_set_method(http_inst, HTTP_METHOD_GET);

    esp_err_t err = esp_http_client_open(http_inst, 0);
    if (err == ESP_OK) {
        int read_len = 0;
        char *buffer = http_read_all(http_inst, &read_len);
        if (buffer && read_len > 0) {
            *response = cJSON_Parse(buffer);
            if (!*response) {
                ESP_LOGE(TAG, "Error parseando JSON de GET %s (len=%d)", url, read_len);
            }
        }
        free(buffer);
    } else {
        ESP_LOGE(TAG, "Error conexion GET: %s", esp_err_to_name(err));
    }
    esp_http_client_close(http_inst);
    esp_http_client_cleanup(http_inst);
}

void http_post_request(const char *url, const cJSON *body, cJSON **response) {
    *response = NULL;
    char *post_data = cJSON_PrintUnformatted(body);
    if (!post_data) return;

    esp_http_client_config_t config = {
        .url = url,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t http_inst = esp_http_client_init(&config);
    esp_http_client_set_method(http_inst, HTTP_METHOD_POST);
    esp_http_client_set_header(http_inst, "Content-Type", "application/json");

    int data_len = strlen(post_data);
    esp_err_t err = esp_http_client_open(http_inst, data_len);
    if (err == ESP_OK) {
        esp_http_client_write(http_inst, post_data, data_len);
        int read_len = 0;
        char *buffer = http_read_all(http_inst, &read_len);
        if (buffer && read_len > 0) {
            *response = cJSON_Parse(buffer);
            if (!*response) {
                ESP_LOGE(TAG, "Error parseando JSON de POST %s", url);
            }
        }
        free(buffer);
    } else {
        ESP_LOGE(TAG, "Error conexion POST: %s", esp_err_to_name(err));
    }
    esp_http_client_close(http_inst);
    esp_http_client_cleanup(http_inst);
    free(post_data);
}

void http_keep_alive_request(const char *url, cJSON **response) {
    *response = NULL;

    esp_http_client_config_t config = {
        .url = url,
        .keep_alive_enable = true,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t http_inst = esp_http_client_init(&config);
    esp_http_client_set_method(http_inst, HTTP_METHOD_GET);

    esp_err_t err = esp_http_client_open(http_inst, 0);
    if (err == ESP_OK) {
        int read_len = 0;
        char *buffer = http_read_all(http_inst, &read_len);
        if (buffer && read_len > 0) {
            *response = cJSON_Parse(buffer);
            if (!*response) {
                ESP_LOGE(TAG, "Error parseando JSON de keep-alive %s", url);
            }
        }
        free(buffer);
    } else {
        ESP_LOGE(TAG, "Error conexion keep-alive: %s", esp_err_to_name(err));
    }
    // No cleanup to keep connection alive
}

