.PHONY: default % run test

default: all

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel/build/kernel8.img -serial null -serial stdio

test: all
	qemu-system-aarch64 -M raspi3 -kernel test/build/kernel8.img -serial null -serial stdio

%:
	$(MAKE) -C libc $@
	$(MAKE) -C shell $@
	$(MAKE) -C kernel $@
	$(MAKE) -C test $@
