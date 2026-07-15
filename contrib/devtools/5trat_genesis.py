#!/usr/bin/env python3
"""Calculate a Bitcoin-family genesis block for 5tratum Coin.

This helper only prints candidate values. It does not edit chain parameters.
"""

from __future__ import annotations

import argparse
import hashlib
import struct


GENESIS_PUBKEY = bytes.fromhex(
    "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb"
    "649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"
)


def sha256d(payload: bytes) -> bytes:
    return hashlib.sha256(hashlib.sha256(payload).digest()).digest()


def compact_target(bits: int) -> int:
    exponent = bits >> 24
    mantissa = bits & 0x007FFFFF
    if bits & 0x00800000:
        raise ValueError("negative compact targets are unsupported")
    if exponent <= 3:
        return mantissa >> (8 * (3 - exponent))
    return mantissa << (8 * (exponent - 3))


def compact_size(value: int) -> bytes:
    if value < 253:
        return bytes((value,))
    if value <= 0xFFFF:
        return b"\xfd" + struct.pack("<H", value)
    if value <= 0xFFFFFFFF:
        return b"\xfe" + struct.pack("<I", value)
    return b"\xff" + struct.pack("<Q", value)


def script_num(value: int) -> bytes:
    if value == 0:
        return b""
    encoded = bytearray()
    negative = value < 0
    absolute = -value if negative else value
    while absolute:
        encoded.append(absolute & 0xFF)
        absolute >>= 8
    if encoded[-1] & 0x80:
        encoded.append(0x80 if negative else 0)
    elif negative:
        encoded[-1] |= 0x80
    return bytes(encoded)


def push(data: bytes) -> bytes:
    if len(data) >= 76:
        raise ValueError("this helper only supports short genesis pushes")
    return bytes((len(data),)) + data


def genesis_transaction(timestamp: str, reward: int) -> bytes:
    message = timestamp.encode("utf-8")
    script_sig = push(script_num(486604799)) + push(script_num(4)) + push(message)
    script_pubkey = push(GENESIS_PUBKEY) + b"\xac"  # OP_CHECKSIG
    return b"".join(
        (
            struct.pack("<I", 1),
            compact_size(1),
            bytes(32),
            struct.pack("<I", 0xFFFFFFFF),
            compact_size(len(script_sig)),
            script_sig,
            struct.pack("<I", 0xFFFFFFFF),
            compact_size(1),
            struct.pack("<Q", reward),
            compact_size(len(script_pubkey)),
            script_pubkey,
            struct.pack("<I", 0),
        )
    )


def mine(timestamp: str, block_time: int, bits: int, start_nonce: int) -> None:
    tx = genesis_transaction(timestamp, 50 * 100_000_000)
    merkle_internal = sha256d(tx)
    target = compact_target(bits)
    for nonce in range(start_nonce, 0x1_0000_0000):
        header = b"".join(
            (
                struct.pack("<I", 1),
                bytes(32),
                merkle_internal,
                struct.pack("<I", block_time),
                struct.pack("<I", bits),
                struct.pack("<I", nonce),
            )
        )
        digest = sha256d(header)
        if int.from_bytes(digest, "little") <= target:
            print(f"time={block_time}")
            print(f"nonce={nonce}")
            print(f"bits=0x{bits:08x}")
            print(f"hash={digest[::-1].hex()}")
            print(f"merkle={merkle_internal[::-1].hex()}")
            return
    raise RuntimeError("no valid nonce found")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--timestamp", required=True)
    parser.add_argument("--time", type=int, required=True)
    parser.add_argument("--bits", type=lambda value: int(value, 0), default=0x207FFFFF)
    parser.add_argument("--nonce", type=int, default=0)
    args = parser.parse_args()
    mine(args.timestamp, args.time, args.bits, args.nonce)


if __name__ == "__main__":
    main()
