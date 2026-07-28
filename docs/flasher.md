# GitHub Pages flasher

The static flasher lives in `web/flasher` and is deployed only from `main`.
It displays the versioned release manifest, recovery status, and browser support.
Web Serial requires a user gesture and a user-selected port.

The initial implementation intentionally has no flash action. Enable flashing
only after a signed/versioned manifest includes the exact supported board,
firmware checksum, recovery image, release notes, and proof of a successful
physical-board recovery test. Reject unknown chip or board profiles.

The [installation and onboarding contract](onboarding-contract.md) defines the
complete owner journey after a reviewed release becomes flashable.
