# Output descriptors in 5TRAT Core

5TRAT wallets use output descriptors to describe spendable scripts and derive
addresses.

5TRAT follows the chain's non-witness transaction model. New wallet addresses
therefore use the supported legacy script forms:

- `pk()`
- `pkh()`
- `sh()`
- `multi()`
- `sortedmulti()`
- `combo()`
- `addr()`
- `raw()`

Witness and Taproot descriptors such as `wpkh()`, `wsh()` and `tr()` are not
valid address-generation choices on the 5TRAT chain.

Descriptor-aware RPCs include `getdescriptorinfo`, `deriveaddresses`,
`importdescriptors`, `listdescriptors`, `scantxoutset`, `getaddressinfo` and
`listunspent`.

5TRAT began from its own genesis block. There are no balances or historical
UTXOs to migrate from Bitcoin, Bitcoin Cash or any other chain.
