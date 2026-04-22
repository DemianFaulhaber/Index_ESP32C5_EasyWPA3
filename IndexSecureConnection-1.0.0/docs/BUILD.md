# Build y Entrega

Este wrapper se entrega como libreria Arduino en formato zip.

## Requisitos para validar compilacion local

- arduino-cli instalado
- Core ESP32 instalado

## Comandos de validacion

```bash
arduino-cli core install esp32:esp32
arduino-cli compile --fqbn esp32:esp32:esp32c5 examples/BasicConnection
```

## Empaquetado de entrega

Desde la carpeta del componente:

```bash
./package_release.sh
```

El zip final queda en `dist/`.
