//
// Created by seatech on 03/11/23.
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
#define CAPTEURPRES_NODE DT_ALIAS(capteur_presence)

const struct i2c_dt_spec dev_lcd_screen = I2C_DT_SPEC_GET(LCD_NODE);
const struct gpio_dt_spec button_gpio1 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_1, gpios, {0});
const struct gpio_dt_spec button_gpio2 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_2, gpios, {0});
const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});
const struct gpio_dt_spec capteur_pres_gpio = GPIO_DT_SPEC_GET_OR(CAPTEURPRES_NODE, gpios, {0});

void alarm_ON_thread()
void alarm_OFF_thread()
void error();
void boutton_callback_1();
void boutton_callback_2();

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

int main(void) {
    gpio_pin_configure(button_gpio1.port, button_gpio1.pin, GPIO_INPUT );
    gpio_init_callback(&button_1, boutton_callback_1, BIT(button_gpio1.pin) );
    gpio_add_callback(button_gpio1.port, &button_1);
    gpio_pin_interrupt_configure_dt(&button_gpio1, GPIO_INT_EDGE_BOTH);

    gpio_pin_configure(button_gpio2.port, button_gpio2.pin, GPIO_INPUT );
    gpio_init_callback(&button_2, boutton_callback_2, BIT(button_gpio2.pin) );
    gpio_add_callback(button_gpio2.port, &button_2);
    gpio_pin_interrupt_configure_dt(&button_gpio2, GPIO_INT_EDGE_BOTH);

    init_lcd(&dev_lcd_screen);
    write_lcd(&dev_lcd_screen, "ALARM MODE :", LCD_LINE_1);
}

void alarm_ON_thread(){
    k_thread_suspend(alarm_ON_thread_id);
    while(1){
        gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);
        k_sleep(K_MSEC(1));
        gpio_pin_configure_dt(&buzzer_gpio, GPIO_OUTPUT_LOW);
        k_sleep(K_MSEC(1));
        gpio_pin_configure_dt(&buzzer_gpio, GPIO_OUTPUT_HIGH);
        k_sleep(K_MSEC(1));
        k_sleep(1);
    }
}
void alarm_OFF_thread(){
    k_thread_suspend(alarm_OFF_thread_id);
    while(1){
        k_sleep(1);
    }
}

void boutton_callback_1()
{
    write_lcd(&dev_lcd_screen, "ON ", LCD_LINE_2);
    k_thread_suspend(alarm_OFF_thread_id);
    k_thread_resume(alarm_ON_thread_id);
}

void boutton_callback_2()
{
    write_lcd(&dev_lcd_screen, "OFF", LCD_LINE_2);
    k_thread_suspend(alarm_ON_thread_id);
    k_thread_resume(alarm_OFF_thread_id);
}

void error()
{
}

K_THREAD_DEFINE(alarm_ON_thread_id,521, alarm_ON_thread, NULL, NULL, NULL, 9, 0, 0);
K_THREAD_DEFINE(alarm_OFF_thread_id,521, alarm_OFF_thread, NULL, NULL, NULL, 9, 0, 0);