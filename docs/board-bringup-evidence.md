# Heltec WiFi LoRa 32 V3 bring-up evidence record

Complete one record for each physical-board candidate. Leave a field marked
`unverified` rather than inferring a value. This record does not authorize a
flash by itself.

## Candidate identity

- Huginn firmware version and source commit: `0.1.0-bringup`; source commit recorded
  by the bring-up PR before review.
- Firmware and recovery-image SHA-256: pending a reviewed CI build artifact.
  The CI `huginn-bringup-bundles` job emits complete base and recovery flash bundles,
  per-file checksums, and `release-manifest-input.json`. Binaries remain
  ignored and are never committed. Copy only reviewed artifact checksums into a
  release manifest after physical recovery evidence is complete.
- Date and operator: 2026-07-28; operator identity intentionally omitted.
- Printed board model and revision marking: `WiFi LoRa 32 V3`, physically
  observed. No private board photo is stored in this repository.
- Official schematic or board-definition reference:
  `https://docs.heltec.org/en/node/esp32/wifi_lora_32/index.html`. Heltec's
  V3/V4 comparison confirms that V3 uses an ESP32-S3N8-class 8 MB integrated
  flash design with CP2102, while V4 changes those properties. The printed
  revision still must be compared to the applicable vendor schematic before any
  peripheral pin is enabled.

## Measured board facts

- ESP32-S3 flash and PSRAM configuration: observed ESP32-S3 QFN56, silicon
  revision 0.2, 40 MHz crystal, embedded 8 MB flash, quad flash mode. PSRAM is
  unverified; the read-only flashing tool does not probe PSRAM.
- CP210 serial port, boot log, and reset/boot procedure: observed CP210x USB
  serial bridge. The local port name and device identifier are intentionally
  omitted. A normal RTS reset produced the ESP32-S3 ROM banner and a
  `SPI_FAST_FLASH_BOOT` DIO boot sequence. The corrected base image then
  reported `HUGINN_VERSION=0.1.0-bringup`, `HUGINN_IMAGE_ROLE=base`,
  `HUGINN_TARGET=esp32s3`, `HUGINN_FLASH_BYTES=8388608 status=ESP_OK`,
  `HUGINN_PROVISIONING=ESP_OK`, and `HUGINN_SELF_TEST=PASS`.
- OLED controller and bus address: unverified; no display driver or pin mapping
  is enabled by the bring-up image.
- Display and indicator research record: see
  `docs/heltec-v3-display-indicator-evidence.md`. It documents the official
  V3/V3.1 candidate mapping while preserving the unverified physical revision,
  I2C address, and electrical polarities.
- Bounded display-probe profile: documented in
  `docs/heltec-v3-display-probe.md`. The base role can scan only the documented
  display I2C bus without driving reset, display power, or LED pins; it has not
  been flashed or physically exercised on this board.
- SX1262 routing and regional configuration: unverified; no radio driver or
  regional configuration is enabled by the bring-up image.
- Battery ADC calibration behavior: unverified; no ADC configuration is enabled
  by the bring-up image.
- Flash and RAM report: clean ESP-IDF v5.3.1 container builds select 8 MB flash
  and disable PSRAM. The corrected base image reports PSRAM unavailable as
  expected, 393296 bytes of free heap, and a passing self-test. The verified
  bundle manifest and all component checksums remain CI artifact inputs rather
  than committed binaries or release metadata.

## Recovery exercise

- Initial serial flash result: the base flash bundle was written through the
  ESP-IDF-generated arguments. Bootloader, partition table, and application
  segment hashes were verified by the serial flashing tool. An initial base
  attempt reached application entry but emitted no application log; the
  reproducible cause was the serial console being disabled in the committed
  defaults. The focused console fix produced the successful base boot recorded
  above.
- Version shown on serial and OLED: the serial version and self-test are
  recorded above. OLED remains unverified and unused.
- Deliberately invalid configuration used: not exercised. The documented
  recovery prerequisite requires an invalid configuration, and this session did
  not authorize inventing or writing a destructive invalid state.
- Factory reset or serial recovery procedure: recovery image erases the default
  Huginn NVS provisioning partition before initialization; see
  `docs/serial-bringup.md`.
- Recovery result and post-recovery boot log: pending because the required
  invalid-configuration prerequisite was not safely exercised.
- Separate rollback-image result: pending because recovery was not flashed.

## Review checklist

- [x] Exact printed V3 marking was compared with the official V3 source.
- [x] Firmware and recovery bundle component checksums were independently verified.
- [x] Sanitized base serial boot log is recorded.
- [x] Runtime memory report is recorded.
- [ ] Invalid-configuration recovery passed on this exact board.
- [ ] Rollback image was verified on this exact board.
- [ ] No secrets, private addresses, raw identifiers, or personal locations are attached.

Until every item is verified and reviewed, keep automatic flashing and OTA disabled.
