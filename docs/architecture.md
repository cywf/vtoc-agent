# Architecture

The Heltec runs one combined firmware image only after Meshtastic integration is
approved. Hermes hosts MCP; Heltec and deployment-side VTOC clients use a small,
authenticated, typed device protocol.

Ragnar remains remote and can request named Heltec-native observations. Ragnar
and the lightweight control page consume the same catalog, job state, freshness,
and result. Neither can control undeclared hardware. An absent RTL-SDR or other
external adapter is reported as unavailable.

The OLED is a status surface, not a remote desktop. It shows a distinct card
for standalone, Sentinel, Ragnar, Intercept, and TAK modes. Each card reports
only local link state, queued-work count, and whether its latest result is
current. Board bring-up supplies the physical display driver after the pin map
is verified; the firmware core supplies the mode-specific content.
