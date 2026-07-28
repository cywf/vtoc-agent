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

## Product components

VTOC Agent is one product repository with three cooperating deliverables:

1. **Device firmware** - board-aware Rust-first firmware for the Heltec WiFi
   LoRa 32 V3. It owns only verified onboard capabilities, local state,
   recovery, and the narrow protocol boundary.
2. **Browser flasher** - the static GitHub Pages application in
   `web/flasher`. It presents releases, recovery instructions, and a
   user-authorized Web Serial path. It never silently selects or flashes a
   device, and it refuses incomplete release metadata.
3. **VTOC client** - a small containerized sidecar for an authorized host or
   deployment. It advertises that host's explicitly declared capabilities and
   exchanges typed results with the VTOC bridge. It is not a general remote
   control service.

The client may use a declared, verified local transport such as a supported
USB/UART adapter or authenticated network connection. USB presence or device
proximity alone never grants a capability: the transport, protocol, power
behavior, permissions, and policy must be declared and tested first.
