.PHONY: all %

all:
	$(MAKE) -C libc $@
	$(MAKE) -C kernel $@
	$(MAKE) -C test $@

%:
	$(MAKE) -C libc $@
	$(MAKE) -C kernel $@
	$(MAKE) -C test $@
