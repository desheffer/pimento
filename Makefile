.PHONY: default % run test watch

default: all

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel/build/kernel8.img -serial null -serial stdio

test: all
	qemu-system-aarch64 -M raspi3 -kernel test/build/kernel8.img -serial null -serial stdio

watch:
	find -regex '.*\(\.c\|\.h\|\.ld\|\.S\|Makefile\)' | entr -c make

%:
	$(MAKE) -C shell $@
	$(MAKE) -C kernel $@
	$(MAKE) -C test $@
