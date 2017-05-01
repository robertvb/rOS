ENTRY(_start)

SECTIONS {
   
	.text 0x00101036: {
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