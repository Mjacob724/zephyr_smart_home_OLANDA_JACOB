//
// Created by seatech on 25/10/23.
//

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "../inc/lcd_screen_i2c.h"
#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_NODE DT_ALIAS(afficheur_lcd)

const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});
const struct i2c_dt_spec dev_lcd_screen = I2C_DT_SPEC_GET(LCD_NODE);
const struct i2c_dt_spec capteur_temp = I2C_DT_SPEC_GET(TEMP_NODE);
void error(void);

int main(void)
{
    gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);
    // Init device
    init_lcd(&dev_lcd_screen);

    // Display a message
    write_lcd(&dev_lcd_screen, HELLO_MSG, LCD_LINE_1);
    write_lcd(&dev_lcd_screen, ZEPHYR_MSG, LCD_LINE_2);

    const struct device *dth11;

    dth11 = device_get_binding("DTH11_GPIO");
    if (!dth11)
    {
        error();
    }

    while (1)
    {
        struct sensor_value temp, humidity;

        if (sensor_sample_fetch(dth11) < 0)
        {
            printk("Échec de l'échantillonnage du capteur DTH11\n");
        }

        if (sensor_channel_get(dth11, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0)
        {
            printk("Échec de récupération de la température\n");
        }

        if (sensor_channel_get(dth11, SENSOR_CHAN_HUMIDITY, &humidity) < 0)
        {
            printk("Échec de récupération de l'humidité\n");
        }

        double temperature = sensor_value_to_double(&temp);
        double humidite = sensor_value_to_double(&humidity);

        printk("Température : %.2f °C\n", temperature);
        printk("Humidite : %.2f %%\n", humidite);

        k_sleep(K_SECONDS(10));
    }
}

void error(){
    while(1)
    {
        printk("Capteur DTH11 non trouvé.\n");
    }
}