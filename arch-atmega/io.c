#include <bathos/bathos.h>
#include <bathos/init.h>
#include <bathos/io.h>
#include <arch/hw.h>

static int uart_setup(void)
{
	/* This setup function should configure the uart pins and clock */

	/*
	 * Input clock is 16MHz, divided by 8 (using U2X) makes 2MHz.
	 * To get 2% precision we need 50 ticks per bit, so 40k bits,
	 * so let's try a 38400 rate:
	 *   2000000 / 38400 = 52.08
	 */
	regs[REG_UCSRC] = 0;
	regs[REG_UBBRL] = 51;

	/* size is 011 for 8-bit symbols */
	regs[REG_UCSRC] = REG_UCSRC_URSEL | REG_UCSRC_UCSZ1 | REG_UCSRC_UCSZ0;
	regs[REG_UCSRA] = REG_UCSRA_UDRE | REG_UCSRA_U2X;
	regs[REG_UCSRB] = REG_UCSRB_TXEN;
	return 0;
}
rom_initcall(uart_setup);

/* set up the timer: use 256 as a prescaler, and irq */
static int timer_setup(void)
{
	regs[REG_TCCR0] = REG_TCCR0_P256;
	regs[REG_TIMSK] = REG_TIMSK_TOIE0;

	asm("sei"); /* enable interrupts */
	return 0;
}
core_initcall(timer_setup); /* before lpj calculation */


void putc(int c)
{
	if (c == '\n')
		putc('\r');
	while ( !(regs[REG_UCSRA] & REG_UCSRA_UDRE) )
		;
	regs[REG_UDR] = c;
}
