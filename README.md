# Uso rápido

## Setup

Iniciar "Arduino Ide",
1) Tools->Board->Boards Manager->Esp32 by Espressif Systems (NO EL DE ARDUINO).
2) Sketch->include library->add .ZIP Library->Seleccionar el archivo .zip incluido.


## Cargar cert/key como archivo (DER recomendado)

Para evitar problemas de largo con archivos binarios `.der`, cargalos desde archivo y calcula el `len` con punteros `start/end`.

1) Guarda los archivos en tu sketch (por ejemplo en una carpeta `certs/`):

- `client_cert.der`
- `client_key.der`

2) Genera headers automaticamente a partir de esos archivos (no pegado manual):

```bash
xxd -i certs/client_cert.der > client_cert_der.h
xxd -i certs/client_key.der > client_key_der.h
```

3) Incluye los headers en tu `.ino` y calcula los largos como si tuvieras `start/end`:

```cpp
#include "client_cert_der.h"
#include "client_key_der.h"

const uint8_t *client_cert_start = client_cert_der;
const uint8_t *client_cert_end = client_cert_der + sizeof(client_cert_der);

const uint8_t *client_key_start = client_key_der;
const uint8_t *client_key_end = client_key_der + sizeof(client_key_der);

const char *cl_cert = reinterpret_cast<const char *>(client_cert_start);
const size_t cl_cert_len = static_cast<size_t>(client_cert_end - client_cert_start);

const char *cl_key = reinterpret_cast<const char *>(client_key_start);
const size_t cl_key_len = static_cast<size_t>(client_key_end - client_key_start);
```

4) Pasa cert, key y longitudes en `begin(...)`:

```cpp
secure.begin(ssid, identity, password, cl_cert, cl_key, cl_cert_len, cl_key_len);
```

Notas:

- En `.der` (binario), no uses `strlen` para calcular longitudes.
- En `.pem` (texto), `strlen` puede funcionar, pero se recomienda mantener el esquema con largo explicito.

## Uso

## 1) Incluir la libreria

```cpp
#include <IndexSecureConnection.h>
```

## 2) Conectar

```cpp
IndexSecureConnection secure;
secure.begin("SSID", "EAP_IDENTITY", "EAP_PASSWORD");
```

## 3) Consultar estado

```cpp
auto st = secure.status();
if (st.connected()) {
  Serial.println(st.ip);
}
```

## 4) Hacer requests

```cpp
String getResp = secure.get("https://httpbin.org/get");
String postResp = secure.post("https://httpbin.org/post", "{\"hello\":\"world\"}");
```

### Referirse a "Connection Example" ante cualquier duda.
