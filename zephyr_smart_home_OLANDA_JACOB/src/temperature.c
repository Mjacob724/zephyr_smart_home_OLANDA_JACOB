#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>

const struct device *const dht11 = DEVICE_DT_GET_ONE(aosong_dht);

void error(void);

void main(void)
{

    if (!dht11)
    {
        error();
    }

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


