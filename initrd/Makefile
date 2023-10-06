.DEFAULT_GOAL := all

IMG := initrd

BUILD := build

BIN := cat help ls sh uname

.PHONY: all
all: $(BUILD)/$(IMG).tar

.PHONY: clean
clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).tar: $(patsubst %, $(BUILD)/rootfs/bin/%, $(BIN)) $(BUILD)/rootfs/etc
	tar -cvf - -C $(BUILD)/rootfs bin etc > $@

$(BUILD)/rootfs/bin/sh:
	@mkdir -p $(@D)
	curl -fSL -o $@ https://github.com/desheffer/pimento/raw/5af73b29650960667e8d591bb8a6b911aef789f9/dash

$(BUILD)/rootfs/bin/toybox:
	@mkdir -p $(@D)
	curl -fSL -o $@ https://github.com/desheffer/pimento/raw/5af73b29650960667e8d591bb8a6b911aef789f9/toybox

$(BUILD)/rootfs/bin/%: $(BUILD)/rootfs/bin/toybox
	cp $< $@

$(BUILD)/rootfs/etc: etc
	cp -R $< $@
