# pi-os

These are my experiments in building an operating system for Raspberry Pi 3.
This project has been heavily guided by the following resources:

- https://github.com/bztsrc/raspi3-tutorial
- https://github.com/dwelch67/raspberrypi-three
- https://github.com/rsta2/circle64
- https://jsandler18.github.io/
- https://github.com/s-matyukevich/raspberry-pi-os
- https://static.docs.arm.com/ddi0487/ca/DDI0487C_a_armv8_arm.pdf

## Getting Started

This code is designed to run on a Raspberry Pi 3 Model B. Be aware that other
versions of the Pi may experience compatibility issues.

Before you can build the project, you will need the GCC `aarch64-none-elf`
toolchain. You can download it using your package manager or use the
`docker.sh` script in this repository.

Build the project by running `make`. This will generate a file called
`build/kernel8.img`. Copy this file to the SD card of your Pi, replacing the
existing `/boot/kernel8.img` file. You can now boot the Pi and it will run the
new code.

You will quickly become tired of swapping the SD card back and forth every time
a change is made. The process will be much easier if you purchase a USB-to-TTL
serial cable and install [Raspbootin](https://github.com/desheffer/raspbootin).
This setup uses the serial connection to load the newest kernel image every
time the Pi is booted.

Finally, it's worth noting that it is possible to emulate some aspects of the
Pi using QEMU. Unfortunately, there is very limited support for the Raspberry
Pi 3, so your mileage may vary.
