# Security policy

## Reporting a vulnerability

Do not open a public issue for a finding that could expose private keys, permit
unauthorized spending, split consensus, crash public nodes at scale or bypass
proof-of-work validation.

Use either of these private routes:

1. Open a private security advisory in the
   [5trat-core repository](https://github.com/WillItMod/5trat-core/security/advisories/new).
2. Email `axebench.app@gmail.com` with the subject `5TRAT Core security`.

Include:

- the affected commit, release or container tag
- architecture and operating system
- exact reproduction steps
- expected and observed behaviour
- logs with wallet secrets, credentials and public IPs removed
- your assessment of impact

Please allow time to reproduce and coordinate a fix before public disclosure.

## Operator rules

- Expose P2P TCP 57555 only when you intend to accept public peers.
- Keep RPC, wallet files and backup material off the public internet.
- Use a long random RPC password.
- Verify release checksums and container digests.
- Keep at least one tested encrypted wallet backup offline.
- Never paste seed phrases, private keys or wallet passwords into an issue.

## Consensus changes

5TRAT has no remote administrator key or checkpoint authority. A consensus
change requires new software, deterministic tests, a documented activation rule
and adoption by node operators. Bootstrap nodes can introduce peers but cannot
override local validation.
