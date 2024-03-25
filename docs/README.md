# 🌶️ Pimento

These are my experiments building an operating system in Rust that runs on a
Raspberry Pi 3 Model B.

## 🏃 Quick start

You will need to install [Nix][nix-installer] before proceeding.

Start the Nix development shell:

```sh
nix develop
```

Build the kernel image and run it with QEMU:

```sh
cd kernel
cargo make qemu
```

## ⚙️ Running on real hardware

This code is designed to run on real hardware using a tool like [Pi Boot
Chain][pi-boot-chain]. Alternatively, it can be installed on an SD card
containing the Raspberry Pi firmware.

## 📚 Resources

This project has been heavily guided by the following resources:

- The [Arm Architecture Reference Manual][arm-manual].
- The [Arm Cortex-A53 Reference Manual][arm-cortex-a53-manual].
- The [Raspberry Pi Processors][raspberrypi-processors] documentation.
- The following tutorials:
  - https://github.com/bztsrc/raspi3-tutorial
  - https://github.com/rust-embedded/rust-raspberrypi-OS-tutorials
  - https://github.com/s-matyukevich/raspberry-pi-os


[arm-cortex-a53-manual]: https://developer.arm.com/documentation/ddi0500/latest/
[arm-manual]: https://developer.arm.com/documentation/ddi0487/latest/
[nix-installer]: https://github.com/DeterminateSystems/nix-installer
[pi-boot-chain]: https://github.com/desheffer/pi-boot-chain
[raspberrypi-processors]: https://www.raspberrypi.com/documentation/computers/processors.html
