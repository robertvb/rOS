

/****************************************************************************************************************/
/* CÓDIGO EJEMPLO OBTENIDO DEL DOCUMENTO DE "ARM LINUX BOOTLOADER"  Por fin algo BIEN documentado :)!   	*/
/* LINK: http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html#appendix_tag_reference 		*/
/****************************************************************************************************************/

/* example.c
 * example ARM Linux bootloader code
 * this example is distributed under the BSD licence
 */

#ifndef ATAGS_H
#define ATAGS_H

#include <stdint.h>

#define	ATAG_NONE	0
#define	ATAG_CORE	0x54410001
#define	ATAG_MEM	0x54410002
#define	ATAG_VIDEOTEXT	0x54410003
#define	ATAG_RAMDISK	0x54410004
#define	ATAG_INITRD2	0x54420005
#define	ATAG_SERIAL	0x54410006
#define	ATAG_REVISION	0x54410007
#define	ATAG_VIDEOLFB	0x54410008
#define	ATAG_CMDLINE	0x54410009

struct atag_header
{
	uint32_t size;	/* Size in words of this tag */
	uint32_t tag;	/* Tag value */
};

/* ATAG_NONE ends the list of ATAGs */
struct atag_none
{
	struct atag_header header;
	/* No further data in this ATAG */
};

/* ATAG_CORE begins the list of ATAGs */
struct atag_core
{
	struct atag_header	header;
	/* Optional entries below */
	uint32_t flags;	/* Bit 0 - read only. Others unused */
	uint32_t pagesize;	/* Page size */
	uint32_t rootdevice;	/* Root device number */
};

/* ATAG_MEM defines a physical memory region */
struct atag_mem
{
	struct atag_header header;
	uint32_t size;		/* Size of region */
	uint32_t address;	/* Address of start of region */
};

/* ATAG_VIDEOTEXT defines a VGA text screen. Not relevant to a Raspberry Pi  */

/* ATAG_RAMDISK defines an initial ramdisk - floppy images only? */
struct atag_ramdisk
{
	struct atag_header header;
	unsigned int flags;	/* Bit 0 = load, bit 1 = prompt */
	unsigned int size;	/* Decompressed size in KB */
	unsigned int start;	/* Start block of ram disk */
};

/* ATAG_INITRD2 - define physical location of ramdisk image */
struct atag_initrd2
{
	struct atag_header header;
	uint32_t address;	/* Address of ramdisk image */
	uint32_t size;		/* Size of compressed(?) image */
};

/* ATAG_SERIAL has the 64-bit serial number */
struct atag_serial
{
	struct atag_header header;
	uint32_t low;
	uint32_t high;
};

/* ATAG_REVISION - board revision number */
struct atag_revision
{
	struct atag_header header;
	uint32_t revision;
};

/* ATAG_VIDEOLFB - describes a framebuffer */
struct atag_videolfb
{
	struct atag_header header;
	unsigned short int width;	/* Width of buffer */
	unsigned short int height;	/* Height */
	unsigned short int depth;	/* Bits/pixel */
	unsigned short int linelength;	// ?
	unsigned int address;		/* Base address of buffer */
	unsigned int size;		/* Size of buffer */
	unsigned char redsize;		/* Number of red bits in each pixel */
	unsigned char redpos;		/* Position of red bits in pixel */
	unsigned char greensize;
	unsigned char greenpos;
	unsigned char bluesize;
	unsigned char bluepos;
	unsigned char reservedsize;	/* Number of reserved bits/pixel */
	unsigned char reservedpos;	/* Position of reserved bits */
};

/* ATAG_CMDLINE - kernel command line */
struct atag_cmdline
{
	struct atag_header header;
	char cmdline;		/* Multiple characters from here */
};

#endif
