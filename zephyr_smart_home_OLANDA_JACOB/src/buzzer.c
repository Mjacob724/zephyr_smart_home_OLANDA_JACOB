
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

/*
void init_buzzer_pwm(void)
{
    const struct device *pwm_dev;

    pwm_dev = device_get_binding(BUZZER_PWM_CONTROLLER);
    if (!pwm_dev)
    {
        printk("PWM non trouvé.\n");
        return;
    }

    pwm_pin_set_cycles(pwm_dev, BUZZER_PWM_CHANNEL, 0, 0, 0);
    pwm_pin_set_period(pwm_dev, BUZZER_PWM_CHANNEL, 0, 200, 0);
    pwm_pin_set_duty_cycle(pwm_dev, BUZZER_PWM_CHANNEL, 0, 0 ,0);
    pwm_pin_start(pwm_dev, BUZZER_PWM_CHANNEL);
}


K_THREAD_STACK_DEFINE(buzzer_thread_stack, CONFIG_BEEPER_THREAD_STACK_SIZE);
struct k_thread buzzer_thread;

void buzzer_thread_fn(void *p1, void *p2, void *p3)
{
    const struct device *pwm_dev;
    int buzzer_on = 0;

    pwm_dev = device_get_binding(BUZZER_PWM_CONTROLLER);
    if (!pwm_dev)
    {
        printk("PWM non trouvé.\n");
        return;
    }
    while (1)
    {
        if (buzzer_on)
        {
            pwm_pin_set_cycles(pwm_dev, BUZZER_PWM_CHANNEL, 0, 0, 0);
            buzzer_on = 0;
        }
        else
        {
            pwm_pin_set_cycles(pwm_dev, BUZZER_PWM_CHANNEL, 0, 1000, 0);
            buzzer_on = 1;
        }
        k_sleep(K_SECONDS(1));
    }
}


k_tid_t buzzer_thread_id;

void create_buzzer_thread(void)
{
    buzzer_thread_id = k_thread_create(&buzzer_thread, buzzer_thread_stack, K_THREAD_STACK_SIZEOF(buzzer_thread_stack),
                                       buzzer_thread_fn, NULL, NULL, NULL,
                                       CONFIG_BEEPER_THREAD_PRIORITY, 0, K_NO_WAIT);
}


static int buzzer_on = 0;

void button_callback(const struct device *dev, struct gpio_callback *cb, u32_t pins)
{
    buzzer_on = !buzzer_on;
    const struct device *pwm_dev = device_get_binding(BUZZER_PWM_CONTROLLER);
    if (!pwm_dev) {
        printk("PWM non trouvé.\n");
        return;
    }

    if (buzzer_on)
    {
        pwm_pin_set_cycles(pwm_dev, BUZZER_PWM_CHANNEL, 0, 1000, 0);
    } else
    {
        pwm_pin_set_cycles(pwm_dev, BUZZER_PWM_CHANNEL, 0, 0, 0);
    }
}

void main(void)
{
    const struct device *button_dev;
    const struct device *pwm_dev;
    button_dev = device_get_binding("BUTTON_GPIO");
    if (!button_dev) {
        printk("Button GPIO non trouvé.\n");
        return;
    }

    gpio_init_callback(&button_cb, button_callback, BIT(0));
    gpio_add_callback(button_dev, &button_cb);
    gpio_pin_configure(button_dev, 0, GPIO_INPUT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_ACTIVE_HIGH);
    gpio_pin_enable_callback(button_dev, 0);
    pwm_dev = device_get_binding(BUZZER_PWM_CONTROLLER);

    if (!pwm_dev)
    {
        printk("PWM non trouvé.\n");
        return;
    }

        pwm_pin_set_cycles(pwm_dev, BUZZER_PWM_CHANNEL, 0, 0, 0);
        pwm_pin_set_period(pwm_dev, BUZZER_PWM_CHANNEL, 0,200,0);
        pwm_pin_set_duty_cycle(pwm_dev, BUZZER_PWM_CHANNEL, 0,0,0);
        pwm_pin_start(pwm_dev, BUZZER_PWM_CHANNEL);

        k_thread_create(&buzzer_thread, buzzer_thread_stack, K_THREAD_STACK_SIZEOF(buzzer_thread_stack),
                        buzzer_thread_fn, NULL, NULL, NULL, CONFIG_BEEPER_THREAD_PRIORITY, 0, K_NO_WAIT);

        while (1) {
            k_sleep(K_FOREVER);
        }
    }

}

struct gpio_callback button_cb;

void init_button(void)
{
    const struct device *button_dev;

    button_dev = device_get_binding("BUTTON_GPIO");
    if (!button_dev) {
        printk("Button GPIO non trouvé.\n");
        return;
    }

    gpio_init_callback(&button_cb, button_callback, BIT(0));
    gpio_add_callback(button_dev, &button_cb);
    gpio_pin_configure(button_dev, 0, GPIO_INPUT | GPIO_INT_DEBOUNCE | GPIO_INT_EDGE | GPIO_ACTIVE_HIGH);
    gpio_pin_enable_callback(button_dev, 0);
}
*/

