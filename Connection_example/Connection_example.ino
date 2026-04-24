#include <IndexSecureConnection.h>
#include "client_cert_der.h"   // generado con: xxd -i client_cert.der > client_cert_der.h
#include "client_key_der.h"    // generado con: xxd -i client_key.der  > client_key_der.h

IndexSecureConnection secure;

// El ssid se asigna independientemente del metodo EAP.
const char *ssid = "ToT_GWNBF1B90";
const char *identity = "admin";

// En caso de trabajar con EAP-PEAP el procedimiento es sencillo y es el siguiente.
const char *password = "zRBmsF6n";


void setup() {
  Serial.begin(115200);
  
  // En caso de trabajar con EAP-TLS, los headers generados por xxd -i exponen:
  //   unsigned char <nombre>[]      -> array con los bytes del DER
  //   unsigned int  <nombre>_len    -> largo en bytes
  // Los nombres dependen del path que le pasaste a xxd, ajustar si difieren.
  //IMPORTANTE, DEBE ESTAR DENTRO DEL SETUP
  const unsigned char *cl_cert     = certs_client_cert_der;
  const size_t         cl_cert_len = certs_client_cert_der_len;
  const unsigned char *cl_key      = certs_client_key_der;
  const size_t         cl_key_len  = certs_client_key_der_len;

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

