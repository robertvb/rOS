ENTRY(_start)

SECTIONS {
   
	.text 0x0: {
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