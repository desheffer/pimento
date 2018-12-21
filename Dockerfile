FROM muslcc/x86_64:aarch64-linux-musleabi

RUN apk add \
    make \
    qemu-system-aarch64
