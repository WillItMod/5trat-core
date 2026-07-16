FROM debian:bookworm-slim AS build

# Small 5tratumOS appliances must not compile with every logical CPU while the
# live stack is serving the UI and wallet. Callers can raise this explicitly on
# a dedicated builder, but two jobs is a safe architecture-neutral default.
ARG BUILD_JOBS=2

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    autoconf automake bash build-essential ca-certificates \
    libboost-all-dev libevent-dev libminiupnpc-dev libnatpmp-dev \
    libsqlite3-dev libtool libzmq3-dev pkg-config python3 systemtap-sdt-dev \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN ./autogen.sh \
 && ./configure \
    --without-gui \
    --disable-tests \
    --disable-bench \
    --disable-fuzz-binary \
    --disable-bdb \
 && make -j"${BUILD_JOBS}" \
 && strip src/bitcoincashIId src/bitcoincashII-cli src/bitcoincashII-wallet

FROM debian:bookworm-slim

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    bash ca-certificates libevent-2.1-7 libevent-pthreads-2.1-7 \
    libminiupnpc17 libnatpmp1 libsqlite3-0 libzmq5 tini \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --create-home --uid 1000 fivetrat \
 && install -d -o fivetrat -g fivetrat /data

COPY --from=build /src/src/bitcoincashIId /usr/local/bin/fivetratd
COPY --from=build /src/src/bitcoincashII-cli /usr/local/bin/fivetrat-cli
COPY --from=build /src/src/bitcoincashII-wallet /usr/local/bin/fivetrat-wallet
COPY docker/entrypoint.sh /usr/local/bin/fivetrat-entrypoint

RUN chmod 0755 /usr/local/bin/fivetrat-entrypoint

USER fivetrat
VOLUME ["/data"]
EXPOSE 57555 57576 28342
ENTRYPOINT ["/usr/bin/tini", "--", "/usr/local/bin/fivetrat-entrypoint"]
