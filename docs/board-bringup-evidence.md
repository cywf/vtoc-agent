# Heltec WiFi LoRa 32 V3 bring-up evidence record

Complete one record for each physical-board candidate. Leave a field marked
`unverified` rather than inferring a value. This record does not authorize a
flash by itself.

## Candidate identity

- Firmware version and source commit:
- Firmware and recovery-image SHA-256:
- Date and operator:
- Printed board revision and photo reference:
- Official schematic or board-definition reference:

## Measured board facts

- ESP32-S3 flash and PSRAM configuration:
- CP210 serial port, boot log, and reset/boot procedure:
- OLED controller and bus address:
- SX1262 routing and regional configuration:
- Battery ADC calibration behavior:
- Flash and RAM report:

## Recovery exercise

- Initial serial flash result:
- Version shown on serial and OLED:
- Deliberately invalid configuration used:
- Factory reset or serial recovery procedure:
- Recovery result and post-recovery boot log:
- Separate rollback-image result:

## Review checklist

- [ ] Exact board revision was compared with the official source.
- [ ] Firmware and recovery checksums were independently verified.
- [ ] Serial boot log is attached and redacted.
- [ ] Memory report is attached.
- [ ] Invalid-configuration recovery passed on this exact board.
- [ ] Rollback image was verified on this exact board.
- [ ] No secrets, private addresses, raw identifiers, or personal locations are attached.

Until every item is verified and reviewed, keep automatic flashing and OTA disabled.
