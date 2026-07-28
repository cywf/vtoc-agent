# Heltec WiFi LoRa 32 V3 display and indicator evidence

This is an evidence-only record for a future display-enabled Huginn image. It
does not authorize a display probe, GPIO test, radio feature, or firmware flash.

## Attached-board identity

Read-only serial evidence identifies the attached candidate as an ESP32-S3
QFN56 with 8 MB embedded quad flash through a CP210x bridge. A normal reset
reaches `SPI_FAST_FLASH_BOOT`, and the current Huginn base image reports a
passing serial self-test.

This excludes the documented V4 hardware profile: Heltec's V4 change log says
V4 moves to ESP32-S3R2, 16 MB external flash, 2 MB PSRAM, and removes CP2102.
The printed model and revision marking on this physical board have not yet been
visually read, so this record must not claim V3.1 or V3.2 as an exact observed
revision. Before implementation, inspect and record the board-side marking and
select the matching Heltec schematic.

## Official V3 reference set

- [Heltec WiFi LoRa 32 V3 Rev 1.1 datasheet](https://s.heltec.cn/download/WiFi_LoRa_32_V3/HTIT-WB32LA_V3%28Rev1.1%29.pdf)
- [Heltec V3.1 schematic](https://resource.heltec.cn/download/WiFi_LoRa_32_V3/HTIT-WB32LA%28F%29_V3.1_Schematic_Diagram.pdf)
- [Heltec V3 board definition](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/blob/master/variants/heltec_wifi_lora_32_V3/pins_arduino.h)
- [Heltec WiFi LoRa 32 hardware update log](https://docs.heltec.cn/en/node/esp32/wifi_lora_32/hardware_update_log.html)

The Rev 1.1 datasheet documents an ESP32-S3FN8, CP2102, and a 0.96-inch
128-by-64 OLED. The V3 board definition and V3.1 schematic agree on the
candidate GPIO mapping below. Heltec's V3 product support identifies SSD1306
as the display mode for this product family.

## Conditional mapping for a physically confirmed V3/V3.1 candidate

| Function | Candidate mapping | Evidence |
| --- | --- | --- |
| OLED controller family | SSD1306-compatible, 128 by 64 | Heltec V3 product support and Rev 1.1 datasheet |
| Display bus | Dedicated I2C bus | Heltec V3 board definition and V3.1 schematic net names |
| OLED SDA | GPIO17 | Heltec V3 board definition |
| OLED SCL | GPIO18 | Heltec V3 board definition |
| OLED reset | GPIO21 | Heltec V3 board definition and Rev 1.1 datasheet |
| Display power control | GPIO36 (`Vext_Ctrl`) | Heltec V3 board definition, Rev 1.1 datasheet, and V3.1 schematic |
| Status LED control | GPIO35 (`LED`) | Heltec V3 board definition and Rev 1.1 datasheet |

## Facts deliberately not assumed

- **Exact printed revision:** not visually observed. Do not reuse the V3.1
  mapping for a V3.2-marked board until the V3.2 schematic is compared.
- **OLED I2C address:** not documented in the official V3 sources above. Do
  not hard-code `0x3C` or another address from community examples.
- **Vext active level:** the schematic identifies GPIO36 as the power-control
  net, but this record does not claim a polarity without a verified board test.
- **LED polarity, colour, and electrical relationship to charge LEDs:** GPIO35
  is the documented logical control, but these physical characteristics are not
  established here.

## Safe next physical probe

After the printed revision is confirmed, build a reviewed serial-only probe
image that does only the following: logs its revision/profile, drives GPIO36
only after the matching schematic establishes its polarity, releases and resets
GPIO21, scans only the dedicated GPIO17/GPIO18 I2C bus, and prints discovered
addresses. Do not add network, LoRa, Bluetooth, or external-device logic. A
separate reviewed indicator test may then pulse GPIO35 with bounded timing
while an operator observes the board.

## Current firmware observation

The corrected base image emits `HUGINN_VERSION=0.1.0-bringup`, base image role,
ESP32-S3 target, successful flash and provisioning status, PSRAM disabled, and
a passing self-test over serial. The OLED is blank and no status LED behavior
is expected because the current image intentionally contains no display or
indicator driver.
