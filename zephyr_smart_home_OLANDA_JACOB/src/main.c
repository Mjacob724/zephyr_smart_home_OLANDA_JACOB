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
#define MOTION_SENSOR DT_ALIAS(motion_sensor)

CONFIG_WIFI=y
CONFIG_INIT_STACKS=y
CONFIG_NET_L2_WIFI_MGMT=y
CONFIG_HEAP_MEM_POOL_SIZE=98304
CONFIG_NET_L2_ETHERNET=y
CONFIG_ESP32_WIFI_STA_AUTO_DHCPV4=y

CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y
CONFIG_NET_IPV6=n
CONFIG_NET_UDP=y
CONFIG_NET_TCP=y
CONFIG_DNS_RESOLVER=y
CONFIG_DNS_RESOLVER_AI_MAX_ENTRIES=10
CONFIG_NET_SOCKETS_POSIX_NAMES=y
CONFIG_NET_SOCKETS=y
CONFIG_HTTP_CLIENT=y

CONFIG_NET_DHCPV4=y
CONFIG_NET_CONFIG_SETTINGS=y
CONFIG_NET_TX_STACK_SIZE=2048
CONFIG_NET_RX_STACK_SIZE=2048
CONFIG_NET_PKT_TX_COUNT=10
CONFIG_NET_PKT_RX_COUNT=10
CONFIG_NET_BUF_RX_COUNT=20
CONFIG_NET_BUF_TX_COUNT=20
CONFIG_NET_MAX_CONTEXTS=10






const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});
const struct i2c_dt_spec dev_lcd_screen = I2C_DT_SPEC_GET(LCD_NODE);
const struct gpio_dt_spec button_gpio1 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_1, gpios, {0});
const struct gpio_dt_spec button_gpio2 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_2, gpios, {0});
const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});
const struct gpio_dt_spec MotionSensor = GPIO_DT_SPEC_GET_OR(MOTION_SENSOR, gpios, {0});

int flag = 0;

void alarm_thread();
void error();
void gpio_callback_1();
void gpio_callback_2();

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

int main(void) {
    struct gpio_callback button_1;
    struct gpio_callback button_2;

    // Configurez les broches
    gpio_pin_configure(button_gpio1.port, button_gpio1.pin, GPIO_INPUT);
    // Configurez la gestion des interruptions
    gpio_init_callback(&button_1, gpio_callback_1, BIT(button_gpio1.pin));
    gpio_add_callback(button_gpio1.port, &button_1);
    // Activez les interruptions
    gpio_pin_interrupt_configure_dt(&button_gpio1, GPIO_INT_EDGE_BOTH);


    gpio_pin_configure(button_gpio2.port, button_gpio2.pin, GPIO_INPUT);
    gpio_init_callback(&button_2, gpio_callback_2, BIT(button_gpio2.pin));
    gpio_add_callback(button_gpio2.port, &button_2);
    gpio_pin_interrupt_configure_dt(&button_gpio2, GPIO_INT_EDGE_BOTH);

    init_lcd(&dev_lcd_screen);
    write_lcd(&dev_lcd_screen, "ALARM MODE :", LCD_LINE_1);
    write_lcd(&dev_lcd_screen, "OFF           ", LCD_LINE_2);

    gpio_pin_configure_dt(&MotionSensor, GPIO_INPUT);

    while(1){
        k_sleep(K_SECONDS(10));
    }
}

K_THREAD_DEFINE(alarm_thread_id, 521, alarm_thread, NULL, NULL, NULL, 9, 0, 0);

void alarm_thread(){
    k_thread_suspend(alarm_thread_id);
    while(1){
        if(flag == 1) {
            gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);
            int sens_val = gpio_pin_get_dt(&MotionSensor);
            if(sens_val == 0){
                write_lcd(&dev_lcd_screen, "ALERTE     ", LCD_LINE_1);
                write_lcd(&dev_lcd_screen, "INTRUS     ", LCD_LINE_2);
                for (int i = 0; i < 200; i++) {
                    k_sleep(K_MSEC(1));
                    gpio_pin_configure_dt(&buzzer_gpio, GPIO_OUTPUT_LOW);
                    k_sleep(K_MSEC(1));
                    gpio_pin_configure_dt(&buzzer_gpio, GPIO_OUTPUT_HIGH);
                }
            }
            else{
                write_lcd(&dev_lcd_screen, "ALARM MODE :", LCD_LINE_1);
                write_lcd(&dev_lcd_screen, "ON           ", LCD_LINE_2);
            }
        }
        else{
            write_lcd(&dev_lcd_screen, "ALARM MODE :", LCD_LINE_1);
            write_lcd(&dev_lcd_screen, "OFF          ", LCD_LINE_2);
            gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_LOW);
        }
        k_sleep(K_SECONDS(1));
    }
}

void gpio_callback_1()
{
    printk("Bouton 1 appuyé\n");
    flag = 1;
    k_thread_resume(alarm_thread_id);
}

void gpio_callback_2()
{
    flag = 0;
    printk("Bouton 2 appuyé\n");
}

void error()
{
}

