# Installing 5TRAT Core

For most users, the supported route is
[5tratumOS](https://5tratum.com/install) with the 5tratSmack app. It bundles the
node, wallet, solo pool, explorer, updates and recovery checks.

This document is for standalone node operators and developers.

## Docker build

Requirements:

- Docker Engine 24 or newer
- 64-bit AMD64 or ARM64 Linux
- persistent storage for `/data`
- outbound internet access

Build from this checkout:

```bash
docker build --tag 5trat-core .
```

Run a mainnet node:

```bash
docker volume create 5trat-core-data

docker run -d \
  --name 5trat-core \
  --restart unless-stopped \
  -e FIVETRAT_NETWORK=main \
  -e FIVETRAT_RPC_PASSWORD='replace-with-a-long-random-secret' \
  -p 57555:57555 \
  -v 5trat-core-data:/data \
  5trat-core
```

The public port is TCP 57555. Do not publish the RPC port to the internet.

## Runtime settings

| Variable | Purpose | Default |
| --- | --- | --- |
| `FIVETRAT_NETWORK` | `main`, `testnet` or `regtest` | `regtest` |
| `FIVETRAT_DATADIR` | Node data directory | `/data` |
| `FIVETRAT_P2P_PORT` | P2P listener | `57555` |
| `FIVETRAT_RPC_PORT` | RPC listener | `57556` |
| `FIVETRAT_RPC_USER` | RPC username | `fivetrat` |
| `FIVETRAT_RPC_PASSWORD` | Required RPC password | none |
| `FIVETRAT_RPC_ALLOW_IP` | Comma-separated private RPC networks | private ranges |
| `FIVETRAT_ADDNODES` | Permanent comma-separated peer connections | none |
| `FIVETRAT_BOOTSTRAP_NODES` | Optional one-shot peer introduction nodes | none |
| `FIVETRAT_EXTERNAL_IP` | Verified public P2P address to advertise | discovered |

Mainnet also contains the compiled DNS seeds `seed1.5trat.net` and
`seed2.5trat.net`, so manual peer entries are normally unnecessary.

## Check synchronization

```bash
docker exec 5trat-core \
  fivetrat-cli -datadir=/data getblockchaininfo
```

Confirm that `initialblockdownload` is false and that `blocks` equals `headers`
before treating the node as synchronized.

## Native developer build

The detailed upstream platform notes remain under [`doc/`](doc/). The headless
Linux build used by the container is:

```bash
./autogen.sh
./configure \
  --without-gui \
  --disable-tests \
  --disable-bench \
  --disable-fuzz-binary \
  --disable-bdb
make -j"$(nproc)"
```

The resulting internal binary names are copied into the published runtime as
`fivetratd`, `fivetrat-cli` and `fivetrat-wallet`.

## Upgrade safety

Back up wallet material before replacing software. Do not delete `/data` to
solve a display problem. Chain data can be downloaded again; private keys
cannot.

Consensus releases must remain on the same canonical chain. Check release
notes, verify checksums and allow the node to finish any required reindex or
recovery before opening mining traffic.
