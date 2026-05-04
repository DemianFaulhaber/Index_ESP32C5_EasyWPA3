# Custom Core ESP32-C5 — WPA3 Suite-B 192-bit

Core personalizado de Arduino para ESP32-C5 con soporte de **WPA3 Suite-B 192-bit** (EAP-TLS enterprise WiFi).  
El core oficial de Espressif **no incluye** `CONFIG_WPA_SUITE_B_192`, este reemplaza únicamente los archivos de librería compilados del chip `esp32c5` habilitando ese flag en IDF.

---

## ¿Qué contiene esta carpeta?

```
CustomCore-AES256-192BITS/
└── esp32c5/          ← Drop-in replacement para la carpeta esp32c5 del core oficial
    ├── dio_qspi/     ← Headers y libs para Flash Mode DIO
    ├── qio_qspi/     ← Headers y libs para Flash Mode QIO
    ├── lib/          ← Librerías estáticas (.a) compiladas con Suite-B 192 habilitado
    ├── flags/        ← Flags de compilación
    └── ...
```

---

## Requisitos

- **Arduino IDE 2.x**
- Core oficial **espressif:esp32 v3.3.8** instalado vía Boards Manager
- Placa: **ESP32-C5**

---

## Instalación paso a paso

### 1. Instalar el core oficial primero

Si no lo tenés instalado, en Arduino IDE:
- `File → Preferences` → agregar la URL del Board Manager de Espressif:
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- `Tools → Board → Boards Manager` → buscar `esp32` → instalar versión **3.3.8**

---

### 2. Verificar que no haya cores duplicados (¡CRÍTICO!)

Arduino IDE busca cores en **múltiples ubicaciones** y usa el primero que encuentra, ignorando el de Arduino15.  
**Si tenés una carpeta `hardware/espressif/esp32` fuera de Arduino15, va a tomar esa en lugar del core oficial.**

Revisá y eliminá si existen:

| Ruta a revisar | Acción |
|---|---|
| `C:\Users\<tu_usuario>\OneDrive\Documentos\Arduino\hardware\` | ❌ Eliminar si existe |
| `C:\Users\<tu_usuario>\Documents\Arduino\hardware\` | ❌ Eliminar si existe |
| `C:\Users\<tu_usuario>\AppData\Local\Arduino15\packages\espressif\` | ✅ Esta es la correcta, no tocar |

> **Cómo identificar el culpable:** si ves errores raros o el `sdkconfig.h` no tiene `CONFIG_WPA_SUITE_B_192`, cerrá Arduino IDE y buscá carpetas `hardware/espressif/esp32` fuera de Arduino15. Borrá todo lo que no sea Arduino15.

---

### 3. Reemplazar la carpeta `esp32c5`

Localizá la carpeta del core instalado. La ruta típica en Windows es:

```
C:\Users\<tu_usuario>\AppData\Local\Arduino15\packages\espressif\hardware\esp32\3.3.8\tools\esp32-arduino-libs\
```

Dentro de esa carpeta vas a ver subdirectorios por chip: `esp32`, `esp32s2`, `esp32c3`, **`esp32c5`**, etc.

**Reemplazá la carpeta `esp32c5` completa** con la que está en este repositorio:

```
# Opción A: Directamente desde el explorador de Windows
1. Navegá a: ...\esp32-arduino-libs\
2. Renombrá la carpeta esp32c5 original a esp32c5_BACKUP
3. Copiá la carpeta esp32c5 de CustomCore-AES256-192BITS/ en su lugar

# Opción B: PowerShell (ajustá la versión si no es 3.3.8)
$core = "$env:LOCALAPPDATA\Arduino15\packages\espressif\hardware\esp32\3.3.8\tools\esp32-arduino-libs"
Rename-Item "$core\esp32c5" "$core\esp32c5_BACKUP"
Copy-Item -Path "<ruta_a_este_repo>\CustomCore-AES256-192BITS\esp32c5" -Destination "$core\esp32c5" -Recurse
```

---

### 4. Limpiar la caché de compilación

Arduino IDE 2.x guarda compilaciones previas en caché. Si ya compilaste antes con el core estándar, **va a seguir usando el binario cacheado** aunque hayas reemplazado el core.

**Borrá estas carpetas completas:**

```
C:\Users\<tu_usuario>\AppData\Local\Arduino\cores\
C:\Users\<tu_usuario>\AppData\Local\Arduino\sketches\
```

> Podés borrar solo `sketches\` si sabés el hash del sketch, pero borrar `cores\` también es recomendable para estar seguro.

En PowerShell:
```powershell
Remove-Item "$env:LOCALAPPDATA\Arduino\cores" -Recurse -Force
Remove-Item "$env:LOCALAPPDATA\Arduino\sketches" -Recurse -Force
```

> ⚠️ Si algún archivo está bloqueado, **cerrá Arduino IDE y VS Code** antes de ejecutar el comando.

---

### 5. Compilar y verificar

Abrí el sketch de ejemplo en `Connection_example/Connection_example.ino`.

El sketch incluye una verificación automática en `setup()`:

```cpp
#ifdef CONFIG_WPA_SUITE_B_192
  Serial.println("¡ÉXITO! Core personalizado detectado. CONFIG_WPA_SUITE_B_192 está activo.");
#else
  Serial.println("ERROR: No se detecta el flag de 192-bits. Estás usando el core estándar.");
#endif
```

Compilá y subí. En el Serial Monitor (115200 baud) deberías ver:

```
¡ÉXITO! Core personalizado detectado. CONFIG_WPA_SUITE_B_192 está activo.
```

Si ves `ERROR: No se detecta el flag...`, revisá los pasos 2 y 4.

---

## Troubleshooting

**El mensaje de error sigue apareciendo después de reemplazar el core**  
→ No limpiaste la caché. Repetí el paso 4.

**No puedo borrar la caché, dice que el archivo está siendo usado**  
→ Cerrá Arduino IDE y VS Code completamente antes de borrar. El servidor de lenguaje (`clangd`) lockea headers en background.

**¿Cómo sé qué core está usando Arduino IDE realmente?**  
→ Activá verbose output: `File → Preferences → Show verbose output during: compilation`.  
En el log buscá la línea con `-I` que apunta a `sdkconfig.h`. Esa ruta te dice exactamente qué `sdkconfig.h` está usando el compilador.

**El build falla con errores de símbolos faltantes**  
→ Asegurate de que la versión del core oficial instalado sea **3.3.8**. Este custom core fue compilado contra esa versión de IDF (5.5.4).

---

## Notas técnicas

- Compilado con **IDF 5.5.4** (`CONFIG_ARDUINO_IDF_BRANCH = v5.5.4`)
- Flash mode **DIO** y **QIO** soportados (la carpeta correspondiente se elige automáticamente según la configuración de la placa)
- El flag habilitado es `CONFIG_WPA_SUITE_B_192=y` → `CONFIG_ESP_WIFI_SUITE_B_192=y`
- Solo se reemplaza `esp32c5/` — el resto de los chips no se ve afectado
