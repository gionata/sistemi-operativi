/*
 * Boot code, ram and flash (according to the sections being used)
 * Alessandro Rubini, 2009-2012 GNU GPL2 or later
 */
.global vectors
.global _entry_ram
.global _entry_rom
.global raise
.extern bathos_setup
.extern bathos_main

/*
 * This file includes the initial assembly stuff. It sets the stack
 * pointer and zeroes the BSS. For rom-boot it also copies data to RAM.
 *
 * Different ELF sections are used to differentiate the behaviour
 */

/* The vectors are used when booting from flash */
.section .vectors, "ax"
vectors:
	.rep 8 /* Lazily, "reset" at each interrupt or trap */
	b _entry_rom
	.endr


/* A trivial data section to remember the entry point, used in raise() */
.data
_entry_save:
	.word 0

/* RAM boot: the first kB of RAM is unused, place the stack here */
.section .text.ramboot
_entry_ram:
	mov r0, pc
	sub r0, #12
	mov sp, r0
	/* Save the entry point */
	ldr	r0, =_entry_ram
	ldr	r1, =_entry_save
	str	r0, [r1]

	b	_bss_loop

/* ROM boot: put the stack before data (the lds puts data 1k into RAM) */
.section .text.romboot
_entry_rom:
	/* We run at 15MHz (no PLL setup), so force flash access to 1 cycle */
        ldr     r0, =0xe01fc004
        mov     r1, #1
        str     r1, [r0]

	ldr	r0, =_sdata
	sub	sp, r0, #4 /* stack is before data */
	ldr	r1, =_edata
	ldr	r2, =_erom

	/* copy from r2 to r0, until r1. We know it's aligned at 16b */
10:
	cmp	r0, r1
	bge	2f
	ldmia	r2!, {r4, r5, r6, r7}
	stmia	r0!, {r4, r5, r6, r7}
	b	10b
2:
	/* Save the entry point */
	ldr	r0, =_entry_rom
	ldr	r1, =_entry_save
	str	r0, [r1]

	b	_bss_loop
.ltorg

/* What follows is common to ROM and RAM boot: clear bss and call main */
.section .text.anyboot
_bss_loop:
	ldr	r1, =__bss_start
	ldr	r2, =__bss_end
	mov	r0, #0
0:
	cmp	r1, r2
	bge	1f
	str	r0, [r1]
	add	r1, #4
	b	0b

1:
	bl bathos_setup
	bl bathos_main
	/* Fall through to raise() */

raise: /* raise is called by libgcc on division by 0 */
	ldr	r1, =_entry_save
	ldr	r0, [r1]
	mov	pc, r0
