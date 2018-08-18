#!/bin/sh

cd "$(dirname "$0")"

TAG=desheffer/aarch64-toolchain
docker pull $TAG || exit
docker run -it --rm -v "$PWD":/app -w /app $TAG $@
