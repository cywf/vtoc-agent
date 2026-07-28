# Hermes integration

MCP runs only in Hermes and its VTOC bridge. The Heltec uses an outbound,
authenticated device protocol. Initial capabilities are health and board-native
observations. Unsupported names are denied; absent declared hardware is
unavailable. The bridge enforces schemas, audit metadata, timeouts, and rate
limits.

`protocol/v1/capability-request.schema.json` is the bridge-to-Heltec contract.
It permits only the declared Heltec V3 target and the five named capabilities.
Wi-Fi and BLE observations may request a bounded result count; every other
initial capability has an empty parameter object. The schema intentionally has
no command, shell, pin, driver, arbitrary adapter, or raw-capture parameter.
