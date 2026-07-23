# 5TRAT public network

Every 5TRAT node validates the chain locally. The public services below improve
discovery and availability but are not trusted consensus authorities.

## Bootstrap and P2P

| Service | Address |
| --- | --- |
| DNS seed 1 | `seed1.5trat.net` |
| DNS seed 2 | `seed2.5trat.net` |
| P2P | TCP 57555 |

DNS seeds return candidate peers to a fresh node. Peer-provided blocks are still
checked against the genesis identity, proof of work, difficulty schedule,
transaction rules and checkpoints compiled into that node.

## Public read services

| Service | Primary | Secondary |
| --- | --- | --- |
| Explorer | [explorer1.5trat.net](https://explorer1.5trat.net/explorer/) | [explorer2.5trat.net](https://explorer2.5trat.net/explorer/) |
| Completed trade tape | `market1.5trat.net` | `market2.5trat.net` |
| Electrum endpoint | `electrum1.5trat.net` | `electrum2.5trat.net` |
| DEX bootstrap | `dex1.5trat.net` | `dex2.5trat.net` |

The completed trade tape carries public, anonymous settlement facts. It does not
publish wallet passwords, private keys or a buyer/seller identity. Consumers
merge and deduplicate both relays.

## Running a public node

1. Synchronize fully before advertising the node.
2. Forward inbound TCP 57555 to the node, or use a verified automatic mapping.
3. Expose no RPC, wallet, application or management port.
4. Confirm external reachability from outside the local network.
5. Keep the node updated and monitor disk, peers and chain tip.

Two installations behind one public IPv4 address cannot both receive the same
external TCP port. Give each public listener a distinct external port or expose
only one of them. Outbound-only nodes still validate and strengthen their own
wallet and mining operation.

## Canonical identity

| Item | Value |
| --- | --- |
| Genesis block | `af4973599946fbe8c350eae4ff51ba9fbe3fc00fa07e8413b869874ee1be8310` |
| Mainnet P2P port | 57555 |
| Address prefix | `5trat:` |
| Proof of work | SHA-256d |

If a node reports a different genesis identity or fails a compiled canonical
checkpoint, it is not on the current 5TRAT mainnet and must not serve mining
work until repaired.
