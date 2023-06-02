NAME	= out/httpd
BASE	= ../../../
DIRS	= . ./arch $(BASE)drv $(BASE)drv/usb \
	$(BASE)lib/lwip/core $(BASE)lib/lwip/core/ipv4 \
	$(BASE)lib/lwip/netif $(BASE)lib/lwip/include \
	$(BASE)lib/lwip/apps/http $(BASE)lib/lwip/apps/lwiperf
CFLAGS	= -DUSBH_NET
LFLAGS	= --specs=nano.specs
include $(BASE)common.mk
out/fs.o: fsdata
fsdata:	$(wildcard web/*.*) makefsdata.exe
	makefsdata.exe web -f:fsdata
makefsdata.exe: arch/cc.h arch/lwipopts.h
	gcc -o$@ $(BASE)lib/lwip/apps/http/makefsdata/makefsdata.c -s -O3 $(addprefix -I,$(DIRS))
