//
// Created by seatech on 25/10/23.
//

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "../inc/lcd_screen_i2c.h"
#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>

#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_NODE DT_ALIAS(afficheur_lcd)
#define BUTTON_NODE_1 DT_ALIAS(button1)
#define BUTTON_NODE_2 DT_ALIAS(button2)
#define BUZZER_NODE DT_ALIAS(buzzer)

#define PERIOD_INIT 1000
#define BEEP_DURATION  K_MSEC(10)
#define PAUSE_DURATION  K_MSEC(1)


const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});

const struct i2c_dt_spec dev_lcd_screen = I2C_DT_SPEC_GET(LCD_NODE);

const struct device *const dht11 = DEVICE_DT_GET_ONE(aosong_dht);

const struct gpio_dt_spec button_gpio1 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_1, gpios, {0});
const struct gpio_dt_spec button_gpio2 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_2, gpios, {0});

const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});

void error(void);

void gpio_callback_1();
void gpio_callback_2();
void init_buzzer_gpio();

int main(void)
{
    gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);
    // Init device
    init_lcd(&dev_lcd_screen);

    // Display a message
    write_lcd(&dev_lcd_screen, HELLO_MSG, LCD_LINE_1);
    write_lcd(&dev_lcd_screen, ZEPHYR_MSG, LCD_LINE_2);

    if (!dht11)
    {
        error();
    }

    struct gpio_callback button_1;
    struct gpio_callback button_2;

    // Configurez les broches
    gpio_pin_configure(button_gpio1.port, button_gpio1.pin, GPIO_INPUT );
    // Configurez la gestion des interruptions
    gpio_init_callback(&button_1, gpio_callback_1, BIT(button_gpio1.pin) );
    gpio_add_callback(button_gpio1.port, &button_1);
    // Activez les interruptions
    gpio_pin_interrupt_configure_dt(&button_gpio1, GPIO_INT_EDGE_BOTH);


    gpio_pin_configure(button_gpio2.port, button_gpio2.pin, GPIO_INPUT );
    gpio_init_callback(&button_2, gpio_callback_2, BIT(button_gpio2.pin) );
    gpio_add_callback(button_gpio2.port, &button_2);
    gpio_pin_interrupt_configure_dt(&button_gpio2, GPIO_INT_EDGE_BOTH);


    while (1)
    {
        struct sensor_value temp, humidity, press;

        if (sensor_sample_fetch(dht11) < 0)
        {
            printk("Échec de l'échantillonnage du capteur DTH11\n");
        }

        if (sensor_channel_get(dht11, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0)
        {
            printk("Échec de récupération de la température\n");
        }

        if (sensor_channel_get(dht11, SENSOR_CHAN_HUMIDITY, &humidity) < 0)
        {
            printk("Échec de récupération de l'humidité\n");
        }

        if (sensor_channel_get(dht11, SENSOR_CHAN_PRESS, &press) < 0)
        {
            printk("Échec de récupération de la pression\n");
        }

        sensor_sample_fetch(dht11);
        sensor_channel_get(dht11,SENSOR_CHAN_AMBIENT_TEMP,&temp);
        sensor_channel_get(dht11,SENSOR_CHAN_HUMIDITY,&humidity);
        sensor_channel_get(dht11,SENSOR_CHAN_PRESS,&press);

        printk("temp: %d.%06d; press: %d.%06d; humidity: %d.%06d\n",
               temp.val1, temp.val2, press.val1, press.val2,
               humidity.val1, humidity.val2);

        k_sleep(K_SECONDS(10));
    }
}

void error()
{
    while(1)
    {
        printk("Capteur DTH11 non trouvé.\n");
    }
}

void gpio_callback_1()
{
    printk("Bouton 1 appuyé\n");
}

void gpio_callback_2()
{
    printk("Bouton 2 appuyé\n");
}


void init_buzzer_gpio()
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
        const struct pwm_dt_spec pwm;
        pwm = PWM_DT_SPEC_INST_GET_BY_NAME(n, alpha);

        return ;
    }
}

K_THREAD_DEFINE(compute_thread_id,521, compute_thread, NULL, NULL, NULL, 9, 0, 0);