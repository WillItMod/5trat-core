# Building 5TRAT Core for Windows

5TRAT does not currently publish or support a native Windows wallet package.
For a Windows development host, use WSL2 with Ubuntu and build the Linux
headless node, or run the supported Docker image.

## WSL2 route

Install Ubuntu through WSL2, keep the checkout inside the Linux filesystem, and
follow [`build-unix.md`](build-unix.md). Do not build from a path under
`/mnt/c`; native Linux paths avoid substantial filesystem overhead and
Autotools failures.

## Docker route

Install Docker Desktop with the WSL2 backend, then follow the container
instructions in [`INSTALL.md`](../INSTALL.md).

The public mainnet P2P port is TCP 57555. Keep RPC private. Back up wallet
material before changing runtimes or storage.

Cross-compiled Windows packages are not part of the current release process.
Any future Windows artifact will be announced and checksummed in this
repository rather than distributed through an unofficial download.
