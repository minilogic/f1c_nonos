MK	= $(MAKE) build -s -C
all:
	$(MK) src/demo/gouraudshade
	$(MK) src/demo/torus
	$(MK) src/demo/voxel
	$(MK) src/demo/wave
	$(MK) src/lvgl
	$(MK) src/slideshow
	$(MK) src/tinygl/cubetext
	$(MK) src/tinygl/flag
	$(MK) src/tinygl/gears
	$(MK) src/tinygl/md2viewer
	$(MK) src/tinygl/wave
	$(MK) src/usbd/msc
	$(MK) src/usbh/msc
	$(MK) src/lwip/httpd
	$(MK) src/coremark
	$(MK) src/audio/mp3player
	$(MK) src/audio/mp3recorder
