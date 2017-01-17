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
	
  _bss_start = .;	
   .bss : {
    *(.dynbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    /* Align here to ensure that the .bss section occupies space up to
       _end.  Align after .bss to ensure correct alignment even if the
       .bss section disappears because there are no input sections.
       FIXME: Why do we need it? When there is no .bss section, we don't
       pad the .data section.  */
    . = ALIGN(. != 0 ? 32 / 8 : 1);
   }
  _bss_end = .;
  _heap_start = .;
	
	/DISCARD/ : {
		*(*)
	}
	
}
