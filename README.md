# VTOC Agent

Board-aware companion firmware and companion services for the Heltec WiFi LoRa 32 V3.

## Foundation status

This is a non-flashable foundation. It has no Meshtastic source, radio drivers,
OTA support, or hardware validation. The first board image will be serial-only,
versioned, checksummed, and recovery-tested.

## Safety boundary

The Heltec executes only declared board-native capabilities. Remote systems such
as Ragnar consume typed results; they do not receive generic radio, shell, or
adapter control. External hardware appears unavailable unless verified.
