ENTRY(reset)

SECTIONS
{
    . = 0x10000; /* Linux kernel starting address */

    .startup . :
    {
        *(.startup)
    }

    .text : 
    {
        *(.text)
    }

    .data : 
    { 
        *(.data) 
    }

    .bss : 
    { 
        *(.bss COMMON) 
    }
	
    . = ALIGN(8);
    _heap_start = .;
    . = . + 0x10000; /* 64K stack size */
    _heap_end = .;
	topOfStack = .;
}