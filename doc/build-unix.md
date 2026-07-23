# Building 5TRAT Core on Linux

The supported public deployment is the container build in
[`INSTALL.md`](../INSTALL.md). These instructions are for developers who need a
native headless build.

## Dependencies

On Debian 12 or Ubuntu 24.04:

```bash
sudo apt update
sudo apt install -y \
  autoconf automake bash bsdextrautils build-essential ca-certificates \
  libboost-all-dev libevent-dev libminiupnpc-dev libnatpmp-dev \
  libsqlite3-dev libtool libzmq3-dev pkg-config python3 systemtap-sdt-dev
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
make -j"$(nproc)"
```

Or use the repository helper:

```bash
./build.sh configure \
  --without-gui \
  --disable-tests \
  --disable-bench \
  --disable-fuzz-binary \
  --disable-bdb
./build.sh release
```

The release helper publishes the executables under the 5TRAT names
`fivetratd`, `fivetrat-cli`, `fivetrat-tx`, `fivetrat-wallet`,
`fivetrat-util` and `fivetrat-seeder`.

## Test

The same compile and smoke suite used by this repository's CI can be run with:

```bash
docker build --target test-runner -f Dockerfile.test .
```

Do not expose RPC to the internet. The public mainnet P2P port is TCP 57555.
