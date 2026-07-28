# Huginn

[![CI](https://github.com/cywf/huginn/actions/workflows/ci.yml/badge.svg)](https://github.com/cywf/huginn/actions/workflows/ci.yml)
[![Flasher site](https://img.shields.io/badge/flasher-live-0ea5e9)](https://cywf.github.io/huginn/)
[![License: GPL--3.0--or--later](https://img.shields.io/badge/license-GPL--3.0--or--later-7c3aed)](LICENSE)
[![Foundation prerelease](https://img.shields.io/badge/release-v0.1.0--foundation-f59e0b)](https://github.com/cywf/huginn/releases/tag/v0.1.0-foundation)

Board-aware firmware, recovery tooling, documentation, and browser flasher for
the Heltec WiFi LoRa 32 V3.

## Foundation status

This is a non-flashable foundation. It has no Meshtastic source, radio drivers,
OTA support, or hardware validation. The first board image will be serial-only,
versioned, checksummed, and recovery-tested.

The recovery-safe Huginn serial bring-up implementation is in
`firmware/esp-idf/`. It produces base and recovery artifacts only through the
pinned container contract. It does not authorize a browser or automatic flash;
see `docs/serial-bringup.md`.

## Safety boundary

The Heltec executes only declared board-native capabilities. Remote systems such
as Ragnar consume typed results; they do not receive generic radio, shell, or
adapter control. External hardware appears unavailable unless verified.

## Product components

Huginn owns the device-side product boundary:

1. **Device firmware** - board-aware Rust-first firmware for the Heltec WiFi
   LoRa 32 V3. It owns only verified onboard capabilities, local state,
   recovery, and the narrow protocol boundary.
2. **Browser flasher** - the static GitHub Pages application in
   `web/flasher`. It presents releases, recovery instructions, and a
   user-authorized Web Serial path. It never silently selects or flashes a
   device, and it refuses incomplete release metadata.
3. **Product documentation** - board evidence, recovery policy, installation,
   and the human-facing onboarding contract.

The separate [Muninn](https://github.com/cywf/muninn) repository owns the
optional desktop companion. Muninn may use a declared, verified local transport
to a Huginn device, but USB presence or device proximity alone never grants a
capability.
