# Serial-only bring-up procedure

This procedure applies only to the first observed Heltec WiFi LoRa 32 V3
candidate and is not an automatic flash instruction.

## Preconditions

1. Confirm the printed board revision against the official Heltec source.
2. Run `tools/verify-bringup.ps1` and retain only sanitized command results.
3. Review the base and recovery SHA-256 values generated in the ignored
   `artifacts/` directory.
4. Confirm that the serial device is the observed CP210x port and that the
   selected port belongs to the intended board.
5. Obtain explicit operator confirmation immediately before `write_flash`.

## Manual serial flash

The operator selects the serial port. The base image is written first using the
ESP-IDF-generated flash command for the matching build directory. Do not infer
offsets or use a browser flasher for this session. Capture a sanitized boot log
that includes only version, role, flash report, PSRAM state, provisioning state,
and self-test result.

## Recovery and rollback

The recovery image intentionally erases the default VTOC NVS provisioning
partition before initialization. Exercise it only after the base image has
demonstrated a deliberately invalid configuration state and only with the
recovery artifact and checksum verified. Flashing the base image again is the
rollback action. Record all outcomes in the board bring-up evidence file.
