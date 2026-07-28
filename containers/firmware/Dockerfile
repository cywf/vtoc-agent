FROM rust:1.78.0-bookworm

RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates git && rm -rf /var/lib/apt/lists/*
RUN rustup component add rustfmt clippy
WORKDIR /workspace

# Lock an immutable base-image digest before the first release branch.
# Add ESP-IDF only after exact version and board evidence are recorded.
CMD ["bash"]
