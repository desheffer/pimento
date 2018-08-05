#!/bin/sh

cd "$(dirname "$0")"

TAG=desheffer/gcc-aarch64-none-elf:7
docker pull $TAG || exit
docker run -it --rm -v "$PWD":/app -w /app $TAG $@
