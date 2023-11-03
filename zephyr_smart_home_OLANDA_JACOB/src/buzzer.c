#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

int main(void)
{
    uint32_t max_period;
    uint32_t period;
    uint8_t dir = 0U;
    int ret;

    printk("PWM-based blinky\n");

    if (!pwm_is_ready_dt(&buzzer_gpio)) {
        printk("Error: PWM device %s is not ready\n",
               buzzer_gpio.dev->name);
        return 0;
    }

    printk("Calibrating for channel %d...\n", buzzer_gpio.channel);
    max_period = MAX_PERIOD;

    while (pwm_set_dt(&buzzer_gpio, max_period, max_period / 2U))
    {
        max_period /= 2U;
        if (max_period < (4U * MIN_PERIOD)) {
            printk("Error: PWM device ",4U * MIN_PERIOD);
            return 0;
        }
    }

    printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
           max_period, MIN_PERIOD);

    period = max_period;
    while (1) {
        ret = pwm_set_dt(&buzzer_gpio, period, period / 2U);
        if (ret) {
            printk("Error %d: failed to set pulse width\n", ret);
            return 0;
        }

        period = dir ? (period * 2U) : (period / 2U);
        if (period > max_period) {
            period = max_period / 2U;
            dir = 0U;
        } else if (period < MIN_PERIOD) {
            period = MIN_PERIOD * 2U;
            dir = 1U;
        }

        k_sleep(K_SECONDS(4U));
    }
    return 0;
}
