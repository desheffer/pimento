FROM muslcc/x86_64:aarch64-linux-musl

RUN apk add \
        curl \
        entr \
        make \
        qemu-system-aarch64 \
        util-linux
