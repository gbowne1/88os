ENTRY(_start)
OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386:i386)

_kernel_load_address = 0x00100000;
_kernel_virtual_base = 0xC0000000;

SECTIONS
{
	. = 1M;
	start = .;

	. = _kernel_load_address;

	.init32.text ALIGN(4K):
	{ 
		*(.init32.text)
	}

	. += _kernel_virtual_base;
	
	kernel_base = .;
	.text :
	{
		_text = .;
		*(.text);
		_text_end = .;
	}
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.text ALIGN(4K) : AT(ADDR(.text) - _kernel_virtual_base)
  	{
    *(.text)
  	}
	
	.rodata ALIGN(4K): AT(ADDR(.rodata) - _kernel_virtual_base)
  	{
    *(.rodata*)
  	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data ALIGN(4K): AT(ADDR(.data) - _kernel_virtual_base)
  	{
    *(.data)
  	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata)) {
    *(.rodata)
  	}
  	.data ALIGN(4K) : AT(ADDR(.data)) {
    *(.data)
  	}
  	.bss ALIGN(4K) : AT(ADDR(.bss)) {
    *(.bss)
  	}
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		sbss = .;
		*(.bss)
		ebss = .;
		*(.bootstrap_stack)
	}
	.data :
	{
		_data = .;
		*(.bss);
		*(.bss*);
		*(.data);
		*(.rodata*);
		*(COMMON)
		_data_end = .;
	}
	 .bss ALIGN(4K): AT(ADDR(.bss) - _kernel_virtual_base)
  	{
    *(COMMON)
    *(.bss)
  	}

  	.pm ALIGN(4K): AT(ADDR(.pm) - _kernel_virtual_base)
  	{
    *(.pm)
 	}

  	.pd ALIGN(4K): AT(ADDR(.pd) - _kernel_virtual_base)
  	{
    *(.pd)
    _end_of_kernel = (. + 4095) & (~ 4095); /* 4K align */
  	}
	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note*);
		*(.note.gnu.build-id)
		*(.iplt*);
		*(.igot*);
		*(.rel*);
		*(.comment);
	}

	end = ALIGN(4K);
	kernel_image_end = .;
	kernel_end = .;
}