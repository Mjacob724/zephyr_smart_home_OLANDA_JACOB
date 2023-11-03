#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});

int main(void)
        {
    k_sleep(K_MSEC(1));
    gpio_pin_configure_dt(&buzzer_gpio, GPIO_OUTPUT_LOW);
    k_sleep(K_MSEC(1));
    gpio_pin_configure_dt(&buzzer_gpio, GPIO_OUTPUT_HIGH);
    k_sleep(K_MSEC(1));
}