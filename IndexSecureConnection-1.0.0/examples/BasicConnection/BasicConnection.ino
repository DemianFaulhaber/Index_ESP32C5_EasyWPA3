#include <IndexSecureConnection.h>

IndexSecureConnection secure;

const char *ssid = "TU_SSID";
const char *identity = "TU_IDENTIDAD_EAP";
const char *password = "TU_PASSWORD_EAP";

void setup() {
  Serial.begin(115200);

  if (!secure.begin(ssid, identity, password)) {
    auto st = secure.status();
    Serial.print("Error conectando: ");
    Serial.println(st.error);
    return;
  }

  auto st = secure.status();
  Serial.print("Conectado. IP: ");
  Serial.println(st.ip);

  String response = secure.get("https://httpbin.org/get");
  Serial.println(response);
}

void loop() {
}
