# Build stage
FROM ubuntu:22.04 AS builder

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    bison \
    flex \
    libnuma-dev \
    libnuma-dev \
    git \
    python3-dev \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Create build directory and run CMake
# Release build for performance
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# Run smoke test to verify build
RUN chmod +x tests/cli/smoke_test.sh && ./tests/cli/smoke_test.sh

# Run unit tests
RUN cd build && ctest --output-on-failure

# Runtime stage
FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libgomp1 \
    libnuma1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy executable from builder stage
COPY --from=builder /app/build/treewidzard .

# Verify the binary runs (smoke test)
RUN ./treewidzard --help || true

# Default entrypoint
ENTRYPOINT ["./treewidzard"]
