FROM muslcc/x86_64:aarch64-linux-musleabi

RUN apk add \
    entr \
    make \
    qemu-system-aarch64
