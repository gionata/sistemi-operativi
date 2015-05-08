#ifndef __BATHOS_GPIO_H__
#define __BATHOS_GPIO_H__

/* for gpio_dir() and gpio_dir_af() */
#define GPIO_DIR_IN	0
#define GPIO_DIR_OUT	1

/* for gpio_dir_af()  */
#define GPIO_AF_GPIO	0
#define GPIO_AF(x)	x

#ifdef CONFIG_HAS_GPIO
#include <arch/gpio.h>
#else
/*
 * Empty GPIO definitions, to allow libraries to be built
 * (then, clearly, we can't link with those functions, like w1-gpio)
 */

extern void __no_gpio_code_for_this_architecture__(); /* link error */

static inline int gpio_dir_af(int gpio, int output, int value, int afnum)
{
	__no_gpio_code_for_this_architecture__();
	return 0;
}

static inline void gpio_dir(int gpio, int output, int value)
{
	__no_gpio_code_for_this_architecture__();
}

static inline int gpio_get(int gpio)
{
	__no_gpio_code_for_this_architecture__();
	return -1;
}
#define __gpio_get gpio_get

static inline void gpio_set(int gpio, int value)
{
	__no_gpio_code_for_this_architecture__();
}
#define __gpio_set gpio_set

#endif /* CONFIG_HAS_GPIO */
#endif /* __BATHOS_GPIO_H__ */
