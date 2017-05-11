ENTRY(_start)

SECTIONS {

	. = 0x00101000 + SIZEOF_HEADERS;
	.text : {
	   	_start = .;
		*(.text.startup)
		*(.text)
	}
	
	.data : {
		*(.data)
		*(.rodata*)
	}
	
   .bss : {
   }
	
	/DISCARD/ : {
		*(*)
	}
	
}