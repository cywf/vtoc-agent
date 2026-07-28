# Recovery policy

Never flash an unreviewed image. Each hardware candidate requires a version,
checksum, serial boot fixture, memory report, and rollback image. Do not enable
OTA updates or automatic flashing until serial recovery passes on the exact
board revision.

## Serial recovery contract

The initial contract is represented in `firmware/src/board.rs` and the protocol
fixtures. It is intentionally **not** a flasher or flash authorization. A
candidate can become *reviewable* only when its version, checksum, serial boot
log, memory report, invalid-configuration recovery test, and rollback image
are all recorded for the exact board. Automatic flashing and OTA remain false
regardless of fixture completeness; a separate human review and physical-board
decision are still required.
