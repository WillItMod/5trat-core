# 5TRAT Core

5TRAT Core is the open-source validating node and wallet backend for the 5TRAT
network. It is a fresh SHA-256d blockchain with its own genesis block, network
identity, address prefix, difficulty schedule, peer network and transaction
history.

The complete consensus implementation is public so miners, node operators,
wallet developers and future exchange integrators can inspect what every node
enforces.

## Official links

| Resource | Address |
| --- | --- |
| 5TRAT website | [5trat.com](https://5trat.com) |
| 5tratumOS | [5tratum.com](https://5tratum.com) |
| 5tratSmack app | [github.com/WillItMod/5tratSmack](https://github.com/WillItMod/5tratSmack) |
| Public explorer 1 | [explorer1.5trat.net](https://explorer1.5trat.net/explorer/) |
| Public explorer 2 | [explorer2.5trat.net](https://explorer2.5trat.net/explorer/) |
| Core issues | [github.com/WillItMod/5trat-core/issues](https://github.com/WillItMod/5trat-core/issues) |

## Mainnet at a glance

| Parameter | Value |
| --- | --- |
| Currency | 5TRAT |
| Address prefix | `5trat:` |
| Atomic unit | 0.00000001 5TRAT |
| Proof of work | SHA-256d |
| Target block time | 5 minutes from height 80 |
| Difficulty | Per-block ASERT |
| ASERT half-life | 15 minutes from height 80 |
| Minimum difficulty | Approximately 6,985,044.19 from height 80 |
| Initial subsidy | 5.00 5TRAT before proof-jackpot activation |
| Proof rewards | Blue 4.75, Pink 5.25, Gold 6.75 5TRAT across the winning block and following settlement block |
| Long-run expected issuance | 5.00 5TRAT per block before halvings |
| Halving interval | 420,000 blocks |
| Approximate maximum supply | 4.2 million 5TRAT plus fees |
| P2P port | TCP 57555 |
| RPC port | TCP 57576 in 5tratSmack; configurable for standalone nodes |
| Genesis | `af4973599946fbe8c350eae4ff51ba9fbe3fc00fa07e8413b869874ee1be8310` |

Block one uses the compiled launch target. From block two onward, the ASERT
clock is relative to block one's accepted timestamp. Every 144 blocks the node
starts a deterministic ASERT epoch. A long gap can ease future work gradually,
but the target cannot exceed the network's compiled proof-of-work limit.

## One chain, three proof qualities

Blue, Pink and Gold are nested labels for the quality of a valid winning proof:

| Tier | Proof threshold | Reward mechanics from height 280 |
| --- | --- | --- |
| Blue | Network target `T` | 4.75 5TRAT immediately |
| Pink | `T / 4` | 4.75 immediately, then 0.50 to the same claim in the next block |
| Gold | `T / 12` | 4.75 immediately, then 2.00 to the same claim in the next block |

They are not separate chains and do not have independent network difficulties.
Every candidate must first satisfy the same network target. The delayed
settlement prevents a miner selecting its own reward after seeing its hash.

## Price and mining-cost estimates

The protocol contains no GBP, EUR, USD or DGB price oracle. 5TRAT has no
administrator-set price.

The 5tratSmack trade view derives its public DGB reference only from completed
5TRAT/DGB atomic swaps. Open offers do not move the guide. The wallet's
electricity figure is different: it estimates what the displayed coins may have
cost that miner to produce using their hashrate, an assumed 14 J/TH and their
entered electricity tariff. It is a cost estimate, not a market price.

## Peer discovery

Mainnet builds contain two independent DNS bootstrap names:

- `seed1.5trat.net`
- `seed2.5trat.net`

Seeds introduce a new node to peers. They do not provide trusted blocks,
checkpoints or consensus decisions. Every node independently verifies every
header, block and transaction it accepts. See [NETWORK.md](NETWORK.md) for the
public service map and operator guidance.

## Build and run

The supported quick route builds a headless AMD64 or ARM64 container directly
from this checkout:

```bash
docker build --tag 5trat-core .
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

Only P2P port 57555 should be exposed publicly. Keep RPC private. For a
self-contained node, wallet, explorer and solo pool, install 5tratSmack through
5tratumOS instead.

More detail is in [INSTALL.md](INSTALL.md).

## Tests

```bash
docker build --tag 5trat-core-tests -f Dockerfile.test .
```

This builds the unit-test binary and exercises production parameters, proof
tiers, launch timing, jackpot settlement, subsidy halvings and supply bounds.
Consensus changes should include deterministic tests and an explicit activation
rule.

## Code lineage

5TRAT has a fresh chain identity and does not inherit the balances, blocks,
checkpoints, price or peer network of Bitcoin, Bitcoin Cash, BitcoinII, BCH2 or
DigiByte.

The implementation was built from mature Bitcoin-family open-source code.
Upstream copyright notices, internal compatibility identifiers and historical
test names remain where required for attribution and maintainability. Seeing an
upstream name in a source file does not make that project part of the 5TRAT
network. See [CREDITS.md](CREDITS.md).

## Security

There is no administrator key, balance editor, remote checkpoint or hidden
consensus control. Protocol upgrades require reviewed software with an explicit
activation rule that node operators choose to run.

Do not expose RPC or wallet files. Keep encrypted wallet backups and verify
downloads. Report security-sensitive findings using
[SECURITY.md](SECURITY.md), not a public issue.

## Licence

5TRAT Core and its inherited components are distributed under the MIT licence.
See [COPYING](COPYING) and the retained source-level attribution.
