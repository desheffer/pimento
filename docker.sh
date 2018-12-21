#!/bin/sh

cd "$(dirname "$0")"

TAG=pi-os
docker build -t ${TAG} .
docker run -it --rm -v "${PWD}":/app -w /app ${TAG} $@
