#include <IndexSecureConnection.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

IndexSecureConnection secure;

const char *ssid = "ToT_GWNBF1B90";
const char *identity = "admin";
const char *password = "zRBmsF6n";

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("\n\n=== BOOT ===");

  Serial.println("[1] NVS init...");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  Serial.println("[1] OK");

  Serial.println("[2] netif init...");
  esp_netif_init();
  Serial.println("[2] OK");

  Serial.println("[3] event loop...");
  esp_err_t loop_err = esp_event_loop_create_default();
  if (loop_err != ESP_OK && loop_err != ESP_ERR_INVALID_STATE) {
    ESP_ERROR_CHECK(loop_err);
  }
  Serial.println("[3] OK");

  Serial.println("[4] Conectando WiFi...");
  if (!secure.begin(ssid, identity, password)) {
    auto st = secure.status();
    Serial.print("Error conectando: ");
    Serial.println(st.error);
    return;
  }

  auto st = secure.status();
  Serial.print("Conectado. IP: ");
  Serial.println(st.ip);
}

void loop() {

}
