BOOT	= out/boot
SRCS	= $(foreach dir,$(DIRS) $(DIRD),$(wildcard $(dir)/*.c))
SRCH	= $(foreach dir,$(DIRS) $(DIRD),$(wildcard $(dir)/*.h))
OBJS	= $(patsubst %.c,out/%.o,$(notdir $(SRCS) $(CSRCS)))
vpath %.c $(dir $(SRCS))

CC      = arm-none-eabi-
CFLAGS  += $(addprefix -I,$(DIRS)) -c -O3 -mcpu=arm926ej-s \
	-ffunction-sections -fdata-sections \
	-MMD -MT$@ -Wall -Wformat=0 -DARM
LFLAGS	+= -lm -Xlinker --gc-sections -Wl,-Map,$(NAME).map -T$(BASE)drv/
FEL	= "$(BASE)tools\sunxi\sunxi-fel"
MKSUNXI	= "$(BASE)tools\sunxi\mksunxi"

.PHONY:	all clean run flash

all:	out $(BOOT).bin  $(NAME).bin
	$(CC)size -G out/*.elf
run:	#all
	$(FEL) -p spl $(BOOT).bin
	$(FEL) -p write 0x80000000 $(NAME).bin
	$(FEL) exec 0x80000000
flash:	#all
	$(FEL) -p spiflash-write 0 $(BOOT).bin
	$(FEL) -p spiflash-write 4096 $(NAME).bin
build:	clean all
	rm -fr out/*.d* out/*.e* out/*.m* out/*.o*
$(NAME).bin: $(NAME).elf
	$(CC)objcopy -O binary $^ $@
$(NAME).elf: $(OBJS)
	$(CC)gcc $^ -o$@ --specs=rdimon.specs $(LFLAGS)link.ld
$(BOOT).bin: $(BOOT).elf
	$(CC)objcopy -O binary $^ $@
	$(MKSUNXI) $@ 1>&0
$(BOOT).elf: $(OBJS)
	$(CC)gcc $^ -o$@ -nostartfiles $(LFLAGS)boot.ld
out/%.o: %.c
	@echo . $(notdir $<)
	$(CC)gcc $(CFLAGS) -o$@ $<
out:
	mkdir $@
clean:
	echo Clean
	rm -fr out

-include $(patsubst %.o,%.d,$(OBJS))
