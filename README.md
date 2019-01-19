# pi-os

These are my experiments in building an operating system for the Raspberry Pi 3
Model B.

For simplicity, I am using [musl libc](https://www.musl-libc.org/) and trying
to match the [Linux system call
definitions](https://thog.github.io/syscalls-table-aarch64/latest.html).

This project has been heavily guided by the following resources:

- https://github.com/bztsrc/raspi3-tutorial
- https://github.com/dwelch67/raspberrypi-three
- https://github.com/rsta2/circle64
- https://jsandler18.github.io/
- https://github.com/s-matyukevich/raspberry-pi-os
- https://static.docs.arm.com/ddi0487/ca/DDI0487C_a_armv8_arm.pdf
- https://www.raspberrypi.org/documentation/hardware/raspberrypi/

## Getting Started

This code is designed to run in QEMU and on real hardware.

You will need to install [Docker](https://docs.docker.com/install/) before
proceeding. A Docker image is provided to help with compiling and running the
kernel.

### QEMU

Launch the Docker environment by running `make dkr-sh`. Next, run `make run`
inside the Docker container. This will compile the kernel and start QEMU.

That's it! The kernel is now running.

### Raspberry Pi 3 Model B

Please note the specific model of Pi that I am using. This code will likely
experience compatibility issues wth older versions of the Pi.

Launch the Docker environment by running `make dkr-sh`. Next, run `make` inside
the Docker container. This will compile the kernel and generate a kernel image,
`kernel/build/kernel8.img`. Copy this image to the SD card for your Pi,
overwriting the existing `/boot/kernel8.img` file. Boot the Pi and the kernel
should be running.

You will quickly become tired of swapping the SD card back and forth every time
a change is made. The process can be made easier by purchasing a USB-to-TTL
serial cable and installing
[Raspbootin](https://github.com/desheffer/raspbootin). This method loads the
newest kernel image over the serial connection every time the Pi is booted.
