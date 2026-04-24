# Core WPA3 192 Addon

Objetivo: crear un core custom para Arduino IDE puro con soporte WPA3-Enterprise 192-bit (Suite-B), separado de la libreria.

Importante:
- No se habilita por API desde el sketch.
- Se habilita en el core (configuracion de compilacion del core).

## Scripts incluidos

- `create_local_custom_core.ps1`: clona el core instalado de Espressif y lo parchea con Suite-B 192.
- `remove_local_custom_core.ps1`: elimina el core custom local.

## Copy/paste (Windows PowerShell)

Desde este directorio ejecutar:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\create_local_custom_core.ps1
```

Eso crea un vendor nuevo en Arduino15:

```text
%LOCALAPPDATA%\Arduino15\packages\indexesp32\
```

y activa:

```ini
CONFIG_ESP_WIFI_SUITE_B_192=y
```

en el paquete de libs para ESP32-C5.

Luego:
1. Reiniciar Arduino IDE.
2. Seleccionar board del vendor custom (indexesp32).
3. Compilar tu sketch con la libreria.

## Revertir

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\remove_local_custom_core.ps1
```

## Nota

Este flujo esta enfocado en ESP32-C5 (el parche se aplica a `esp32c5-libs`).
