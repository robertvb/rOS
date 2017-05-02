ENTRY(_start)

SECTIONS {
   
   	_start = .;
	.text 0x00101036: {
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