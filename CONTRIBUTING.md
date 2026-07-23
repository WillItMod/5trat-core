# Contributing to 5TRAT Core

Review, testing and narrowly scoped pull requests are welcome.

## Before changing code

1. Read [README.md](README.md), [INSTALL.md](INSTALL.md) and
   [SECURITY.md](SECURITY.md).
2. Search existing issues and pull requests.
3. For a consensus or network-format proposal, open a design issue before
   writing a large patch.
4. Report security-sensitive findings privately.

## Pull requests

- Create a focused branch.
- Keep unrelated formatting out of behavioural changes.
- Explain what changes, why it is needed and whether it affects consensus,
  wallet compatibility, P2P behaviour or stored data.
- Add deterministic tests for consensus and serialization changes.
- State the commands used to validate the patch.
- Preserve upstream copyright and licence notices.

Useful title prefixes include `consensus:`, `net:`, `wallet:`, `rpc:`, `build:`,
`test:` and `doc:`.

## Tests

Build the targeted test image:

```bash
docker build --tag 5trat-core-tests -f Dockerfile.test .
```

For a normal runtime build:

```bash
docker build --tag 5trat-core .
```

Consensus changes require an explicit activation rule. Do not silently alter
rules already enforced by the live chain.

## Source lineage

The codebase retains upstream internal names in some paths, binaries, tests and
comments. Do not mechanically remove those from consensus code merely for
branding. Public documentation and user-facing strings should describe 5TRAT
accurately, while attribution remains intact.
