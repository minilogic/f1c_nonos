ENTRY(_boot)

SECTIONS
{
	. = 0;
	.text :
	{
		out/boot.o (.text*)
		out/dram.o (.text*)
		*(.text*)
	}
	.data :	{ *(.data) }
	PROVIDE(__spl_size = .);
}
