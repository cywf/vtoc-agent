# Heltec WiFi LoRa 32 V3

This is the only intended 0.x board profile. Pin assignments remain absent
until the printed revision and official schematic are verified against the
physical board. Record the OLED controller/address, SX1262 routing, CP210 serial
behavior, flash/RAM configuration, and battery ADC calibration before enabling
the corresponding peripheral.

## Contract

`firmware/src/board.rs` represents this board only by stable identity and
safety constraints. It intentionally makes no electrical, USB, display, radio,
or memory claims. The `heltec-v3-board-profile.json` fixture remains
`unverified` until an operator records the exact printed board revision and
checks it against the official schematic.
