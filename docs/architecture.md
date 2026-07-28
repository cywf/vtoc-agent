# Architecture

The Heltec runs one combined firmware image only after Meshtastic integration is
approved. Hermes hosts MCP; Heltec and deployment-side VTOC clients use a small,
authenticated, typed device protocol.

Ragnar remains remote and can request named Heltec-native observations. Ragnar
and the lightweight control page consume the same catalog, job state, freshness,
and result. Neither can control undeclared hardware. An absent RTL-SDR or other
external adapter is reported as unavailable.
