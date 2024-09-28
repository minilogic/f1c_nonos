MK	= $(MAKE) build -s -C
all:
	$(MK) src/audio/mp3player
	$(MK) src/audio/mp3recorder
	$(MK) src/coremark
	$(MK) src/demo/gouraudshade
	$(MK) src/demo/plasma
	$(MK) src/demo/torus
	$(MK) src/demo/voxel
	$(MK) src/demo/wave
	$(MK) src/gui/lvgl7
	$(MK) src/gui/lvgl9
	$(MK) src/irq
	$(MK) src/lwip/httpd
	$(MK) src/slideshow
	$(MK) src/tinygl/md2viewer
	$(MK) src/tinygl/cubetext
	$(MK) src/tinygl/gears
	$(MK) src/tinygl/flag
	$(MK) src/tinygl/wave
	$(MK) src/twi/ns2009
	$(MK) src/twi/gt911
	$(MK) src/twi/rtc
	$(MK) src/usbd/msc
	$(MK) src/usbh/msc
