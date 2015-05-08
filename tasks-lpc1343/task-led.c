/*
 * A trivial task that flashes three leds
 * Alessandro Rubini, 2009 GNU GPL2 or later
 */
#include <bathos/bathos.h>
#include <bathos/gpio.h>
#include <arch/hw.h>

static int led_init(void *unused)
{
	gpio_init();
	/* all bits 1 == led off */
	gpio_dir_af(GPIO_NR(3, 0), GPIO_DIR_OUT, 1, GPIO_AF_GPIO);
	gpio_dir_af(GPIO_NR(3, 1), GPIO_DIR_OUT, 1, GPIO_AF_GPIO);
	gpio_dir_af(GPIO_NR(3, 2), GPIO_DIR_OUT, 1, GPIO_AF_GPIO);
	gpio_dir_af(GPIO_NR(3, 3), GPIO_DIR_OUT, 1, GPIO_AF_GPIO);
	return 0;
}

static void *led(void *arg)
{
	int state = (int)arg;

	if (state < 4)
		gpio_set(GPIO_NR(3, state), 1); /* off */
	state++;
	if (state > 4)
		state = 0;
	if (state > 3)
		return (void *)state;
	gpio_set(GPIO_NR(3, state), 0); /* on */
	return (void *)state;
}

static struct bathos_task __task t_led = {
	.name = "leds", .period = HZ / 5,
	.init = led_init, .job = led,
	.release = 10
};
