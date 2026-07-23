# Credits and code lineage

5TRAT is a new blockchain with its own genesis block, network identity, peer
network, balances and consensus schedule. It is not a fork of another project's
live ledger.

Its implementation stands on mature open-source work from:

- [Bitcoin Core](https://github.com/bitcoin/bitcoin)
- [Bitcoin Cash Node](https://gitlab.com/bitcoin-cash-node/bitcoin-cash-node)
- BitcoinII and Bitcoin Cash II contributors whose code formed part of the
  development lineage
- Satoshi Nakamoto's original Bitcoin design and implementation

The source therefore retains upstream copyright notices, internal compatibility
identifiers and historical test names. These are attribution and engineering
lineage. They do not import another chain's blocks, balances, checkpoints,
seeds, price or governance into 5TRAT.

5TRAT-specific consensus work includes:

- the fresh genesis and address/network identity
- the five-minute mainnet schedule
- per-block ASERT parameters and bounded launch behaviour
- Blue, Pink and Gold proof-quality classification
- delayed proof-jackpot settlement
- the 420,000-block halving schedule
- canonical checkpoint recovery and public peer bootstrap configuration

See the Git history for individual contributors and [COPYING](COPYING) for the
MIT licence.
