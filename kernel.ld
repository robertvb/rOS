

SECTIONS {
	/*
	 * 0x8000 ver el ARManual. *bootloader!
	*/
	.text 0x8000 : {
		*(.text.startup)
		*(.text)
	}
	
	.data : {
		*(.data)
		*(.rodata*)
	}

	
	/DISCARD/ : {
		*(*)
	}
	*/
	
}
