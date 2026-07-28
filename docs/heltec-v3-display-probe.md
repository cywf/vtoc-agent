# Heltec WiFi LoRa 32 V3 bounded display probe

The Huginn base image includes an intentionally bounded I2C acknowledgement
probe for the observed **Heltec WiFi LoRa 32 V3** board profile. It exists to
collect reviewable serial evidence before a display driver is considered. It is
not a display implementation and it does not authorize a hardware flash.

## Candidate wiring

The profile uses only the documented V3 display bus:

| Function | GPIO | Probe behavior |
| --- | --- | --- |
| OLED SDA | 17 | Configured as I2C0 SDA for a 100 kHz acknowledgement scan. |
| OLED SCL | 18 | Configured as I2C0 SCL for a 100 kHz acknowledgement scan. |
| OLED reset | 21 | Recorded in serial diagnostics; never driven. |
| Display power control (`Vext_Ctrl`) | 36 | Recorded in serial diagnostics; never driven. |
| Status LED | 35 | Recorded in serial diagnostics; never driven. |

The mapping comes from the [official Heltec V3 board definition](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/blob/master/variants/heltec_wifi_lora_32_V3/pins_arduino.h),
[V3 Rev 1.1 datasheet](https://s.heltec.cn/download/WiFi_LoRa_32_V3/HTIT-WB32LA_V3%28Rev1.1%29.pdf),
and [V3 schematic](https://resource.heltec.cn/download/WiFi_LoRa_32_V3/HTIT-WB32LA%28F%29_V3_Schematic_Diagram.pdf).

## Deliberate limits

- The probe does not initialize an SSD1306 controller, draw pixels, or infer an
  OLED address.
- It does not drive GPIO21, GPIO36, or GPIO35. OLED address, display-power
  polarity, and LED polarity remain unresolved.
- It does not enable Wi-Fi, Bluetooth, LoRa, Meshtastic, a portal, Telegram, or
  remote control.
- A `NO_DEVICE_ACK` result is valid evidence, not a self-test failure: the
  display may be unpowered or its address remains unknown.

## Expected serial evidence

When the base role is built with `CONFIG_HUGINN_DISPLAY_PROBE=y`, it logs the
board profile, UART diagnostic mode, candidate pins, and the fact that no GPIO
actuation occurred. It then reports either one or more acknowledged I2C
addresses or `HUGINN_DISPLAY_PROBE_RESULT=NO_DEVICE_ACK`.

The recovery role explicitly disables this probe. The bundle verification
workflow checks both generated role configurations after the pinned-container
build and before validating bundle manifests and checksums.
