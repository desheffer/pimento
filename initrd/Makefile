.DEFAULT_GOAL := all

IMG := initrd

BUILD := build

.PHONY: all
all: $(BUILD)/$(IMG).tar

.PHONY: clean
clean:
	rm -rf $(BUILD)
	$(MAKE) -C hello clean

$(BUILD)/$(IMG).tar: $(BUILD)/rootfs/bin/hello
	tar -cvf - -C $(BUILD)/rootfs bin > $@

$(BUILD)/rootfs/bin/hello: hello/build/hello.img
	@mkdir -p $(@D)
	cp $< $@

hello/build/hello.img:
	$(MAKE) -C hello all
