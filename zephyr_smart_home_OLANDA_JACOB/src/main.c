//
// Created by seatech on 03/11/23.
//
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "../inc/lcd_screen_i2c.h"
#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/http/client.h>
#include <errno.h>
#include <zephyr/net/socket.h>

#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_NODE DT_ALIAS(afficheur_lcd)
#define BUTTON_NODE_1 DT_ALIAS(button1)
#define BUTTON_NODE_2 DT_ALIAS(button2)
#define BUZZER_NODE DT_ALIAS(buzzer)
#define MOTION_SENSOR DT_ALIAS(motion_sensor)
#define SSID "OnePlus 8T"
#define PSK "eagleoffire8"

const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});
const struct i2c_dt_spec dev_lcd_screen = I2C_DT_SPEC_GET(LCD_NODE);
const struct gpio_dt_spec button_gpio1 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_1, gpios, {0});
const struct gpio_dt_spec button_gpio2 = GPIO_DT_SPEC_GET_OR(BUTTON_NODE_2, gpios, {0});
const struct gpio_dt_spec buzzer_gpio = GPIO_DT_SPEC_GET_OR(BUZZER_NODE, gpios, {0});
const struct gpio_dt_spec MotionSensor = GPIO_DT_SPEC_GET_OR(MOTION_SENSOR, gpios, {0});
const struct device *const dht11 = DEVICE_DT_GET_ONE(aosong_dht);

int flag = 0;

static K_SEM_DEFINE(wifi_connected, 0, 1);
static K_SEM_DEFINE(ipv4_address_obtained, 0, 1);

static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ipv4_cb;

void alarm_thread();
void error();
void gpio_callback_1();
void gpio_callback_2();

static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb);
static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb);
static void handle_ipv4_result(struct net_if *iface);
static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface);
void wifi_connect(void);
void wifi_status(void);

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

    net_mgmt_init_event_callback(&wifi_cb, wifi_mgmt_event_handler, NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_init_event_callback(&ipv4_cb, wifi_mgmt_event_handler, NET_EVENT_IPV4_ADDR_ADD);

    net_mgmt_add_event_callback(&wifi_cb);
    net_mgmt_add_event_callback(&ipv4_cb);

    wifi_connect();

    k_sem_take(&wifi_connected, K_FOREVER);
    wifi_status();
    k_sem_take(&ipv4_address_obtained, K_FOREVER);


    struct sensor_value temp, humidity, press;
    while(1){
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

            struct sensor_value temp, humidity, press;

            sensor_sample_fetch(dht11);
            sensor_channel_get(dht11,SENSOR_CHAN_AMBIENT_TEMP,&temp);
            sensor_channel_get(dht11,SENSOR_CHAN_HUMIDITY,&humidity);
            sensor_channel_get(dht11,SENSOR_CHAN_PRESS,&press);

            printk("temp: %d.%06d; press: %d.%06d; humidity: %d.%06d\n",
                   temp.val1, temp.val2, press.val1, press.val2,
                   humidity.val1, humidity.val2);
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
    printk("Bouton 2 appuyé\n");
    flag = 0;
}

void error()
{
}

static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;
    if (status->status)
    {
        printk("Connection request failed (%d)\n", status->status);
    }
    else
    {
        printk("Connected\n");
        k_sem_give(&wifi_connected);
    }
}

static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;
    if (status->status)
    {
        printk("Disconnection request (%d)\n", status->status);
    }
    else
    {
        printk("Disconnected\n");
        k_sem_take(&wifi_connected, K_NO_WAIT);
    }
}

static void handle_ipv4_result(struct net_if *iface)
{
    int i = 0;
    for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++)
    {
        char buf[NET_IPV4_ADDR_LEN];
        if (iface->config.ip.ipv4->unicast[i].addr_type != NET_ADDR_DHCP)
        {
            continue;
        }
        printk("IPv4 address: %s\n", net_addr_ntop(AF_INET, &iface->config.ip.ipv4->unicast[i].address.in_addr, buf, sizeof(buf)));
        printk("Subnet: %s\n", net_addr_ntop(AF_INET, &iface->config.ip.ipv4->netmask, buf, sizeof(buf)));
        printk("Router: %s\n", net_addr_ntop(AF_INET, &iface->config.ip.ipv4->gw, buf, sizeof(buf)));
    }
    k_sem_give(&ipv4_address_obtained);
}

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
    switch (mgmt_event)
    {
        case NET_EVENT_WIFI_CONNECT_RESULT:
            handle_wifi_connect_result(cb);
            break;
        case NET_EVENT_WIFI_DISCONNECT_RESULT:
            handle_wifi_disconnect_result(cb);
            break;
        case NET_EVENT_IPV4_ADDR_ADD:
            handle_ipv4_result(iface);
            break;
        default:
            break;
    }
}

void wifi_connect(void)
{
    struct net_if *iface = net_if_get_default();
    struct wifi_connect_req_params wifi_params = {0};
    wifi_params.ssid = SSID;
    wifi_params.psk = PSK;
    wifi_params.ssid_length = strlen(SSID);
    wifi_params.psk_length = strlen(PSK);
    wifi_params.channel = WIFI_CHANNEL_ANY;
    wifi_params.security = WIFI_SECURITY_TYPE_PSK;
    wifi_params.band = WIFI_FREQ_BAND_2_4_GHZ;
    wifi_params.mfp = WIFI_MFP_OPTIONAL;
    printk("Connecting to SSID: %s\n", wifi_params.ssid);
    if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &wifi_params, sizeof(struct wifi_connect_req_params)))
    {
        printk("WiFi Connection Request Failed\n");
    }
}

void wifi_status(void)
{
    struct net_if *iface = net_if_get_default();
    struct wifi_iface_status status = {0};
    if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, iface, &status, sizeof(struct wifi_iface_status)))
    {
        printk("WiFi Status Request Failed\n");
    }
    printk("\n");
    if (status.state >= WIFI_STATE_ASSOCIATED)
    {
        printk("SSID: %-32s\n", status.ssid);
        printk("Channel: %d\n", status.channel);
        printk("RSSI: %d\n", status.rssi);
    }
}

/*
static struct addrinfo hints;
struct addrinfo *res;
int st, sock, ret;
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
st = getaddrinfo(HTTP_HOST, HTTP_PORT, &hints, &res);
printf("getaddrinfo status: %d\n", st);

if (st != 0)
{
printf("Unable to resolve address, quitting\n");
return 0;
}
dump_addrinfo(res);
sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
CHECK(zsock_connect(sock, res->ai_addr, res->ai_addrlen));
*/