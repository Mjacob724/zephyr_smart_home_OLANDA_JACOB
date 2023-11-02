
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>

#define PERIOD_INIT 1500

#define BUZZER_NODE DT_ALIAS(buzzer)

#define BEEP_DURATION  K_MSEC(10)
#define PAUSE_DURATION  K_MSEC(1)
#define US_TO_HZ(_us)  (USEC_PER_SEC / (_us))

#define PWM_LABEL DT_LABEL(PWM_NODE)
#define PWM_CHANNEL BUZZER_PIN

const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});


void init_buzzer_gpio();

void init_buzzer_gpio(void)
{
    const struct device *buzzer_dev;

    buzzer_dev = device_get_binding(BUZZER_NODE);
    if (!buzzer_dev)
    {
        printk("Buzzer GPIO non trouvé.\n");
        return;
    }

    else
    {
        const struct device *pwm;

        pwm = device_get_binding(PWM_LABEL);

        for (int i = 0; i < ARRAY_LEN(note_periods); i++)
        {
            period = note_periods[i];
            pwm_pin_set_usec(pwm, PWM_CHANNEL, period, period / 2U, 0);
            k_sleep(BEEP_DURATION);
            pwm_pin_set_usec(pwm, PWM_CHANNEL, period, 0, 0);
            k_sleep(PAUSE_DURATION);
        }
        return 0;
    }
}


