MEMORY { ram : org = 0x80000000, len = RAMSIZE }

stack_und = ORIGIN(ram) + LENGTH(ram) - 0x10000;
stack_abt = stack_und - 0x10000;
stack_irq = stack_abt - 0x10000;
stack_fiq = stack_irq - 0x10000;
stack_usr = stack_fiq - 0x10000;
stack_srv = stack_usr - 0x40000;

ENTRY(_vectors)

SECTIONS
{
	.text :
	{
		*(.vectors);
		. = ALIGN(4);
		*(.init);
		. = ALIGN(4);
		*(.text);
		. = ALIGN(4);
		*(.rodata);
		. = ALIGN(4);
		*(.rodata*);
		. = ALIGN(4);
	} > ram
	.data :
	{
		*(.data)
		. = ALIGN(4);
	} > ram
	. = ALIGN(4);
	PROVIDE(image_size = . - ORIGIN(ram));
	.bss :
	{
		PROVIDE(bss_start = .);
		*(.bss)
		*(.bss.**)
		*(COMMON)
		. = ALIGN(4);
		PROVIDE(bss_end = .);
	} > ram
	. = ALIGN(8);
	end = .;
}
