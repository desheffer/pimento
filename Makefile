.DEFAULT_GOAL := all

.PHONY: all
all: #! Build the kernel (default)
	$(MAKE) -C shell all
	$(MAKE) -C kernel all
	$(MAKE) -C test all

.PHONY: clean
clean: #! Clean generated files
	$(MAKE) -C shell clean
	$(MAKE) -C kernel clean
	$(MAKE) -C test clean

.PHONY: dkr-sh
dkr-sh: #! Start a Docker shell
	docker-compose run --rm app sh

.PHONY: help
help: #! Show this help message
	@echo 'Usage: make [OPTIONS] [TARGET]'
	@echo ''
	@echo 'Targets:'
	@sed -n 's/\(^.*:\).*#!\( .*\$\)/  \1\2/p' $(MAKEFILE_LIST) | column -t -s ':'

.PHONY: run
run: #! Run the kernel
run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel/build/kernel8.img -serial null -serial stdio

.PHONY: test
test: #! Run the tests
test: all
	qemu-system-aarch64 -M raspi3 -kernel test/build/kernel8.img -serial null -serial stdio

.PHONY: watch
watch: #! Build the kernel and watch for changes
	find -regex '.*\(\.c\|\.h\|\.ld\|\.S\|Makefile\)' | entr -c make
