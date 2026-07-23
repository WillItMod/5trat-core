#!/bin/bash
# 5TRAT Core Build Script
# Usage: ./build.sh [command]
#   configure  - Run autogen.sh and configure (first time setup)
#   build      - Incremental build (default)
#   release    - Build and update release binaries
#   clean      - Clean build artifacts
#   full       - Clean + configure + build + release

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
RELEASE_DIR="$SCRIPT_DIR/release"
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# The source tree still uses inherited executable filenames internally. Release
# artifacts are renamed here so users only interact with the 5TRAT names.
RELEASE_BINARIES=(
    "bitcoincashIId:fivetratd"
    "bitcoincashII-cli:fivetrat-cli"
    "bitcoincashII-tx:fivetrat-tx"
    "bitcoincashII-wallet:fivetrat-wallet"
    "bitcoincashII-util:fivetrat-util"
    "bch2-seeder:fivetrat-seeder"
)

cmd_configure() {
    echo "==> Running autogen.sh..."
    cd "$SCRIPT_DIR"
    ./autogen.sh

    echo "==> Creating build directory..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    echo "==> Running configure..."
    "$SCRIPT_DIR/configure" "$@"

    echo "==> Configuration complete!"
}

cmd_build() {
    if [ ! -f "$BUILD_DIR/Makefile" ]; then
        echo "Error: Build not configured. Run './build.sh configure' first."
        exit 1
    fi

    echo "==> Building with $NPROC parallel jobs..."
    cd "$BUILD_DIR"
    make -j"$NPROC"

    echo "==> Build complete!"
}

cmd_release() {
    cmd_build

    echo "==> Updating release binaries..."
    mkdir -p "$RELEASE_DIR"

    for mapping in "${RELEASE_BINARIES[@]}"; do
        source_bin="${mapping%%:*}"
        release_bin="${mapping#*:}"
        if [ -f "$BUILD_DIR/src/$source_bin" ]; then
            cp "$BUILD_DIR/src/$source_bin" "$RELEASE_DIR/$release_bin"
            strip "$RELEASE_DIR/$release_bin"
            echo "    $release_bin"
        fi
    done

    echo "==> Release binaries updated in $RELEASE_DIR"
    find "$RELEASE_DIR" -maxdepth 1 -type f -name 'fivetrat*' -exec ls -lh {} + \
        | awk '{print "    "$9": "$5}'
}

cmd_clean() {
    echo "==> Cleaning build..."
    if [ -d "$BUILD_DIR" ]; then
        cd "$BUILD_DIR"
        make clean 2>/dev/null || true
    fi
    echo "==> Clean complete!"
}

cmd_full() {
    cmd_clean
    shift 2>/dev/null || true
    cmd_configure "$@"
    cmd_release
}

# Main
case "${1:-build}" in
    configure)
        shift
        cmd_configure "$@"
        ;;
    build)
        cmd_build
        ;;
    release)
        cmd_release
        ;;
    clean)
        cmd_clean
        ;;
    full)
        shift
        cmd_full "$@"
        ;;
    *)
        echo "5TRAT Core Build Script"
        echo ""
        echo "Usage: $0 [command] [configure options]"
        echo ""
        echo "Commands:"
        echo "  configure  - First-time setup (autogen + configure)"
        echo "  build      - Incremental build (default)"
        echo "  release    - Build and update release/ binaries"
        echo "  clean      - Clean build artifacts"
        echo "  full       - Clean + configure + build + release"
        echo ""
        echo "Examples:"
        echo "  ./build.sh                           # Quick rebuild"
        echo "  ./build.sh release                   # Rebuild + update release/"
        echo "  ./build.sh configure --disable-wallet"
        echo "  ./build.sh full --without-gui"
        ;;
esac
