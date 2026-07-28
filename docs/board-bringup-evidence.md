# Heltec WiFi LoRa 32 V3 bring-up evidence record

Complete one record for each physical-board candidate. Leave a field marked
`unverified` rather than inferring a value. This record does not authorize a
flash by itself.

## Candidate identity

- Firmware version and source commit: `0.1.0-bringup`; source commit recorded
  by the bring-up PR before review.
- Firmware and recovery-image SHA-256: pending a reviewed CI build artifact.
  The CI `bringup-images` job emits complete base and recovery flash bundles,
  per-file checksums, and `release-manifest-input.json`. Binaries remain
  ignored and are never committed. Copy only reviewed artifact checksums into a
  release manifest after physical recovery evidence is complete.
- Date and operator: 2026-07-27; operator identity intentionally omitted.
- Printed board revision and photo reference: unverified. A physical visual
  inspection is still required; no board photo is stored in this repository.
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
  `SPI_FAST_FLASH_BOOT` DIO boot sequence. No application output arrived in a
  passive three-second serial sample.
- OLED controller and bus address: unverified; no display driver or pin mapping
  is enabled by the bring-up image.
- SX1262 routing and regional configuration: unverified; no radio driver or
  regional configuration is enabled by the bring-up image.
- Battery ADC calibration behavior: unverified; no ADC configuration is enabled
  by the bring-up image.
- Flash and RAM report: clean ESP-IDF v5.3.1 container builds select 8 MB flash
  and disable PSRAM. Base image size is 0x2DC60 and recovery image size is
  0x2DE10, each leaving 82% of the 1 MB application partition free. Runtime
  memory report remains pending first authorized serial flash.

## Recovery exercise

- Initial serial flash result: not attempted. Explicit operator confirmation is
  required immediately before a persistent flash operation.
- Version shown on serial and OLED: pending flash. OLED remains unverified.
- Deliberately invalid configuration used: pending; it will be documented only
  after the base image and checksum are reviewed.
- Factory reset or serial recovery procedure: recovery image erases the default
  VTOC NVS provisioning partition before initialization; see
  `docs/serial-bringup.md`.
- Recovery result and post-recovery boot log: pending.
- Separate rollback-image result: pending.

## Review checklist

- [ ] Exact board revision was compared with the official source.
- [ ] Firmware and recovery checksums were independently verified.
- [ ] Serial boot log is attached and redacted.
- [ ] Memory report is attached.
- [ ] Invalid-configuration recovery passed on this exact board.
- [ ] Rollback image was verified on this exact board.
- [ ] No secrets, private addresses, raw identifiers, or personal locations are attached.

Until every item is verified and reviewed, keep automatic flashing and OTA disabled.
