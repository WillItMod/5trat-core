#!/usr/bin/env bash
set -euo pipefail

datadir="${FIVETRAT_DATADIR:-/data}"
network="${FIVETRAT_NETWORK:-regtest}"
conf="${datadir}/fivetrat.conf"

mkdir -p "${datadir}"

# This file is generated from container environment on every start so an
# explicitly approved external P2P address can be enabled without touching the
# wallet or chain data volumes.
umask 077
rpc_user="${FIVETRAT_RPC_USER:-fivetrat}"
rpc_password="${FIVETRAT_RPC_PASSWORD:?FIVETRAT_RPC_PASSWORD must be set}"
rpc_allow_ips="${FIVETRAT_RPC_ALLOW_IP:-127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16}"
p2p_port="${FIVETRAT_P2P_PORT:-57555}"
rpc_port="${FIVETRAT_RPC_PORT:-57556}"
zmq_hashblock_port="${FIVETRAT_ZMQ_HASHBLOCK_PORT:-28342}"
advertise_file="${FIVETRAT_P2P_ADVERTISE_FILE:-/p2p-state/external-p2p}"
external_ip="${FIVETRAT_EXTERNAL_IP:-}"
if [[ -z "$external_ip" && -s "$advertise_file" ]]; then
  external_ip="$(tr -d '\r\n[:space:]' <"$advertise_file")"
fi

  {
    echo "server=1"
    echo "listen=1"
    echo "listenonion=0"
    echo "dnsseed=0"
    # Peer-observed discovery plus the host mapper's verified address feeds
    # normal P2P self-announcement. The native container-level mapper stays off
    # because it would target a Docker-only address.
    echo "discover=1"
    echo "upnp=0"
    echo "natpmp=0"
    echo "txindex=1"
    echo "fallbackfee=0.00001000"
    if [[ -n "$external_ip" ]]; then
      echo "externalip=${external_ip}"
    fi
    echo "rpcuser=${rpc_user}"
    echo "rpcpassword=${rpc_password}"
    IFS=',' read -ra rpc_allow_entries <<< "${rpc_allow_ips}"
    for rpc_allow_entry in "${rpc_allow_entries[@]}"; do
      rpc_allow_entry="${rpc_allow_entry//[[:space:]]/}"
      [[ -n "${rpc_allow_entry}" ]] && echo "rpcallowip=${rpc_allow_entry}"
    done
    echo "zmqpubhashblock=tcp://0.0.0.0:${zmq_hashblock_port}"
    if [[ "${network}" == "regtest" ]]; then
      echo "regtest=1"
    elif [[ "${network}" == "testnet" ]]; then
      echo "testnet=1"
    fi
    if [[ "${network}" == "regtest" ]]; then
      echo "[regtest]"
    elif [[ "${network}" == "testnet" ]]; then
      echo "[test]"
    else
      echo "[main]"
    fi
    echo "rpcbind=0.0.0.0:${rpc_port}"
    echo "rpcport=${rpc_port}"
    echo "port=${p2p_port}"
    # Local replica links and public bootstrap links use the same validated P2P
    # protocol.  Bootstrap nodes are discovery/availability aids only: every
    # header, block and transaction they send is still verified locally.
    for peer_list in "${FIVETRAT_ADDNODES:-}" "${FIVETRAT_BOOTSTRAP_NODES:-}"; do
      [[ -z "${peer_list//[[:space:]]/}" ]] && continue
      IFS=',' read -ra peers <<< "${peer_list}"
      for peer in "${peers[@]}"; do
        peer="${peer//[[:space:]]/}"
        [[ -n "${peer}" ]] && echo "addnode=${peer}"
      done
    done
  } > "${conf}"

exec fivetratd -datadir="${datadir}" -conf="${conf}" -printtoconsole=1 "$@"
