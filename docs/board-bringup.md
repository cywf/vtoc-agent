# Board bring-up

No board is approved for flashing from this repository yet. The first physical
session verifies board revision, CP210 serial, boot/reset behavior, OLED
controller/address, and flash/RAM configuration. The first candidate requires a
serial boot log, checksum, memory report, and a successful invalid-configuration
recovery test before Wi-Fi, Meshtastic, or remote execution is added.

The recovery-safe first Huginn image is serial-only. It targets only an observed
ESP32-S3 with 8 MB quad flash and intentionally omits Wi-Fi provisioning,
display, LoRa, ADC, and PSRAM-dependent behavior until physical-board evidence
proves the associated configuration. Build and manual serial instructions are
in `docs/serial-bringup.md`.
