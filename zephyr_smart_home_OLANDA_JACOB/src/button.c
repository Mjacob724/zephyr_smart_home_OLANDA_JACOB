//
// Created by seatech on 02/11/23.
//

#include <zephyr.h>
#include <device.h>
#include <gpio.h>

#define BUTTON1_PIN 16
#define BUTTON2_PIN 27

void button_callback(struct device *dev, struct gpio_callback *cb)
{
    if (button_0)
    {
        printk("Bouton 1 appuyé\n");
    }
    if (button_1)
    {
        printk("Bouton 2 appuyé\n");
    }
}

void main(void)
{
    struct device *gpio_dev;
    struct gpio_callback button_cb;

    gpio_dev = device_get_binding(DT_LABEL(DT_NODELABEL(gpio0)));

    if (!gpio_dev) {
        printk("Impossible de récupérer le périphérique GPIO\n");
        return;
    }

    // Configurez les broches
    gpio_pin_configure(gpio_dev, button_0, GPIO_DIR_IN | GPIO_INT | GPIO_PULL_UP | GPIO_INT_EDGE);
    gpio_pin_configure(gpio_dev, button_1, GPIO_DIR_IN | GPIO_INT | GPIO_PULL_UP | GPIO_INT_EDGE);

    // Configurez la gestion des interruptions
    gpio_init_callback(&button_cb, button_callback, BIT(button_0) | BIT(button_1));
    gpio_add_callback(gpio_dev, &button_cb);

    // Activez les interruptions
    gpio_pin_interrupt_configure(gpio_dev, button_0, GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW);
    gpio_pin_interrupt_configure(gpio_dev, button_1, GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW);

    while (1)
    {
        if (gpio_pin_get(gpio_dev, button_0)) {
            printk("Bouton 1 relâché\n");
        } else {
            printk("Bouton 1 appuyé\n");
        }

        if (gpio_pin_get(gpio_dev, button_1)) {
            printk("Bouton 2 relâché\n");
        } else {
            printk("Bouton 2 appuyé\n");
        }

        k_sleep(K_MSEC(100));
    }

    }


