#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_eap_client.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "WIFI_SETUP.h"

static const char *TAG = "WIFI_C5_SECURE";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGE(TAG, "[DBG] STA iniciada, conectando...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGE(TAG, "[DBG] Desconectado, reason=%d, reintentando...", disc->reason);
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "IP obtenida: " IPSTR, IP2STR(&event->ip_info.ip));
        esp_ip4_addr_t *ip_out = (esp_ip4_addr_t *)arg;
        if (ip_out) {
            *ip_out = event->ip_info.ip;
        }
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}



esp_err_t WIFI_SETUP_init(credentials *creds, esp_ip4_addr_t *ip_out) {
    s_wifi_event_group = xEventGroupCreate();
    esp_netif_create_default_wifi_sta();
    ESP_LOGI(TAG, "Iniciando configuración de Wi-Fi 6 (AX) y Enterprise...");
    // 1. Configuración de protocolos: Activamos AX en 2.4G y AN en 5G
    wifi_protocols_t protocols = {
        .ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX,
        .ghz_5g = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N,
    };

    // 2. Configuración de ancho de banda: Forzamos 20MHz para modo HE (Wi-Fi 6)
    wifi_bandwidths_t bw = {
        .ghz_2g = WIFI_BW20,
        .ghz_5g = WIFI_BW20,
    };


    // Inicialización del driver
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // Seteamos el modo estación
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Aplicamos los protocolos y el ancho de banda (Lo que no estaba en el menuconfig)
    ESP_ERROR_CHECK(esp_wifi_set_protocols(WIFI_IF_STA, &protocols));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidths(WIFI_IF_STA, &bw));

    // 3. Configuración Enterprise
    wifi_config_t wifi_config = {
        .sta = {
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.authmode = WIFI_AUTH_WPA3_ENTERPRISE,
            .pmf_cfg = {
                .capable = true,
                .required = true,
            },
        },
    };
    memcpy(wifi_config.sta.ssid, creds->WIFI_SSID, strlen(creds->WIFI_SSID));
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Configuración de identidad para el servidor RADIUS
    if(creds->cl_cert == NULL && creds->cl_key == NULL){
      ESP_ERROR_CHECK(esp_eap_client_set_eap_methods(ESP_EAP_TYPE_ALL));
      ESP_ERROR_CHECK(esp_eap_client_set_identity((uint8_t *)creds->EAP_IDENTITY, strlen(creds->EAP_IDENTITY)));
      ESP_ERROR_CHECK(esp_eap_client_set_username((uint8_t *)creds->EAP_IDENTITY, strlen(creds->EAP_IDENTITY)));
      ESP_ERROR_CHECK(esp_eap_client_set_password((uint8_t *)creds->EAP_PASSWORD, strlen(creds->EAP_PASSWORD)));
    }
    else{
        ESP_ERROR_CHECK(esp_eap_client_set_eap_methods(ESP_EAP_TYPE_TLS));
        
        ESP_ERROR_CHECK(esp_eap_client_set_certificate_and_key(
            (const unsigned char *)creds->cl_cert, creds->cl_cert_len,
            (const unsigned char *)creds->cl_key, creds->cl_key_len,
            NULL, 0
            )
        );

    }
    ESP_ERROR_CHECK(esp_eap_client_set_disable_time_check(true));
    // Registramos el event handler para conectar y obtener IP
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, ip_out));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, ip_out));

    // Habilitamos el stack de enterprise y arrancamos
    ESP_ERROR_CHECK(esp_wifi_sta_enterprise_enable()); 
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi 6 (AX) y Enterprise configurados en C5.");

    // Esperamos hasta 60s a obtener IP (EAP + 4-way handshake + DHCP pueden tardar)
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE, pdFALSE, pdMS_TO_TICKS(60000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Conectado exitosamente.");
    } else {
        ESP_LOGE(TAG, "Timeout esperando conexión.");
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}
