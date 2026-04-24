#include <IndexSecureConnection.h>

IndexSecureConnection secure;

// El ssid se asigna independientemente del metodo EAP.
const char *ssid = "TU_SSID";

// En caso de trabajar con EAP-PEAP el procedimiento es sencillo y es el siguiente.
const char *identity = "TU_IDENTIDAD_EAP";
const char *password = "TU_PASSWORD_EAP";

// En caso de trabajar con EAP-TLS, usar simbolos start/end para calcular.
// Se puede utilizar strlen() si el archivo es .pem
// consumo desde archivo embebido y calcular largo real (DER seguro).
//
// Reemplaza los nombres de simbolo segun tus archivos reales, por ejemplo:
// client_cert.der -> _binary_client_cert_der_start / _binary_client_cert_der_end
// client_key.der  -> _binary_client_key_der_start  / _binary_client_key_der_end
extern const uint8_t client_cert_start[] asm("_binary_client_cert_der_start");
extern const uint8_t client_cert_end[] asm("_binary_client_cert_der_end");
extern const uint8_t client_key_start[] asm("_binary_client_key_der_start");
extern const uint8_t client_key_end[] asm("_binary_client_key_der_end");



const char *cl_cert = reinterpret_cast<const char *>(client_cert_start);
const size_t cl_cert_len = static_cast<size_t>(client_cert_end - client_cert_start);

const char *cl_key = reinterpret_cast<const char *>(client_key_start);
const size_t cl_key_len = static_cast<size_t>(client_key_end - client_key_start);

void setup() {
  Serial.begin(115200);

  if (!secure.begin(ssid, identity, password, cl_cert, cl_key, cl_cert_len, cl_key_len)) {
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

