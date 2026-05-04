#include "EVENT_HANDLER.h"
#include "esp_log.h"

static STATUS_MACHINE status_machine = { .status = 'D' };

void start_connection_process(credentials *creds, bool AES) {
    esp_ip4_addr_t ip_out = {0};
    esp_err_t err = WIFI_SETUP_init(creds, &ip_out, AES);

    status_machine.ip_address[0] = '\0';
    status_machine.error_message[0] = '\0';

    if (err == ESP_OK) {
        status_machine.status = 'C';
        snprintf(status_machine.ip_address, sizeof(status_machine.ip_address), IPSTR, IP2STR(&ip_out));
        ESP_LOGI("EVENT_HANDLER", "Conexion exitosa, IP: %s", status_machine.ip_address);
    } else {
        status_machine.status = 'E';
        snprintf(status_machine.error_message, sizeof(status_machine.error_message), "Error: %s", esp_err_to_name(err));
        ESP_LOGE("EVENT_HANDLER", "%s", status_machine.error_message);
    }
}

STATUS_MACHINE get_full_status(void) {
    return status_machine;
}

char get_status(void) {
    return status_machine.status;
}

int get_ip_address(char *buffer, size_t len) {
    if (status_machine.status == 'C') {
        strncpy(buffer, status_machine.ip_address, len);
        return 0;
    }
    return -1;
}


