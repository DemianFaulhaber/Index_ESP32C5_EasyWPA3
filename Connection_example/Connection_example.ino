#include <IndexSecureConnection.h>

IndexSecureConnection secure;

const char *ssid = "ToT_GWNBF1B90";
const char *identity = "ToT_GWNBF1B90";
const char *password = "zRBmsF6n";

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
}

void loop() {
}
