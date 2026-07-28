# Huginn serial-only bring-up procedure

This procedure applies only to the first observed Huginn Heltec WiFi LoRa 32 V3
candidate and is not an automatic flash instruction.

## Preconditions

1. Confirm the printed board revision against the official Heltec source.
2. Run `tools/verify-bringup.ps1` and retain only sanitized command results.
3. Review the CI-produced base and recovery flash bundles. Each bundle contains
   the bootloader, partition table, application image, ESP-IDF-generated flash
   arguments, ESP-IDF-generated offset metadata, and per-file SHA-256 values.
   The `release-manifest-input.json` index names both bundle manifests.
4. Confirm that the serial device is the observed CP210x port and that the
   selected port belongs to the intended board.
5. Obtain explicit operator confirmation immediately before `write_flash`.

## Manual serial flash

The operator selects the serial port. The base bundle is written first using its
included ESP-IDF-generated `flash_args.generated` and
`flasher_args.generated.json`; do not infer offsets or substitute an
application-only binary. Do not use a browser flasher for this session. Capture
a sanitized boot log that includes only version, role, flash report, PSRAM
state, provisioning state, and self-test result.

## Recovery and rollback

The recovery image intentionally erases the default Huginn NVS provisioning
partition before initialization. Exercise it only after the base image has
demonstrated a deliberately invalid configuration state and only with the
recovery artifact and checksum verified. Flashing the base image again is the
rollback action. Record all outcomes in the board bring-up evidence file.
