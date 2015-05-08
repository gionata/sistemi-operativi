/*
 * A trivial task that flashes three leds
 * Alessandro Rubini, 2009 GNU GPL2 or later
 */
#include <bathos/bathos.h>
#include <bathos/gpio.h>
#include <arch/hw.h>

/* use gpio 19, 20, 21 */
static int led_init(void *unused)
{
	gpio_init();
	gpio_dir_af(19, GPIO_DIR_OUT, 0, GPIO_AF_GPIO);
	gpio_dir_af(20, GPIO_DIR_OUT, 0, GPIO_AF_GPIO);
	gpio_dir_af(21, GPIO_DIR_OUT, 0, GPIO_AF_GPIO);
	return 0;
}

static void *led(void *arg)
{
	int i = (int)arg & 7;
	int j, bit;

	for (j = 1, bit = 19; bit <= 21; j <<= 1, bit++)
		gpio_set(bit, (i & j));
	return (void *)++i;
}

static struct bathos_task __task t_led = {
	.name = "leds", .period = HZ / 2,
	.init = led_init, .job = led,
	.release = 10
};
