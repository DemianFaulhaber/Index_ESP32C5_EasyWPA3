#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H
#include "esp_err.h"
#include "esp_netif_ip_addr.h"

typedef struct {
    char WIFI_SSID[64];
    char EAP_IDENTITY[64];
    char EAP_PASSWORD[246];
    const unsigned char *cl_cert;
    int cl_cert_len;
    const unsigned char *cl_key;
    int cl_key_len;
} credentials;

// Prototipo de la función de inicialización

esp_err_t WIFI_SETUP_init(credentials *creds, esp_ip4_addr_t *ip_out, bool AES);

#endif
