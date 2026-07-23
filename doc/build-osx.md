# Building 5TRAT Core on macOS

macOS is supported as a developer build host. Public node deployments should
use the container route in [`INSTALL.md`](../INSTALL.md).

## Dependencies

Install Xcode Command Line Tools and Homebrew, then:

```bash
xcode-select --install
brew install automake libtool boost pkg-config libevent miniupnpc libnatpmp zeromq sqlite
```

## Build

```bash
git clone https://github.com/WillItMod/5trat-core.git
cd 5trat-core

./autogen.sh
./configure \
  --without-gui \
  --disable-tests \
  --disable-bench \
  --disable-fuzz-binary \
  --disable-bdb
make -j"$(sysctl -n hw.ncpu)"
```

The repository helper can create release copies with the public 5TRAT
executable names:

```bash
./build.sh release
```

The macOS build is intended for development and validation. Keep persistent
public nodes on a supported Linux host, keep RPC private, and expose only TCP
57555 for mainnet P2P.
