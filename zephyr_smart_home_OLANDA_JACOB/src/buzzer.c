
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>

#define BUZZER_PWM_CONTROLLER "PWM_0"
#define BUZZER_PWM_CHANNEL 0

void init_buzzer_gpio(void)
{
    const struct device *buzzer_dev;

    buzzer_dev = device_get_binding("BUZZER_GPIO");
    if (!buzzer_dev) {
        printk("Buzzer GPIO non trouvé.\n");
        return;
    }

    gpio_pin_configure(buzzer_dev, 25, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);
    gpio_pin_set(buzzer_dev, 25, 0);
}

/
