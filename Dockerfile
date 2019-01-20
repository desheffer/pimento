FROM muslcc/x86_64:aarch64-linux-musleabi

RUN apk add \
    curl \
    entr \
    make \
    qemu-system-aarch64 \
    util-linux
