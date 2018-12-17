#!/bin/sh

cd "$(dirname "$0")"

TAG=desheffer/aarch64-toolchain:2018.11-1
docker pull $TAG || exit
docker run -it --rm -v "$PWD":/app -w /app $TAG $@
