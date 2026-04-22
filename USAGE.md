# Uso Rapido

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
