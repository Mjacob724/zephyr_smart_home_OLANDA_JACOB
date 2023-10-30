#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>

void main(void)
{
    const struct device *dth11;

    dth11 = device_get_binding("DTH11_GPIO");
    if (!dth11)
    {
        printk("Capteur DTH11 non trouvé.\n");
        return;
    }

    while (1)
    {
        struct sensor_value temp, humidity;

        if (sensor_sample_fetch(dth11) < 0)
        {
            printk("Échec de l'échantillonnage du capteur DTH11\n");
        }

        if (sensor_channel_get(dth11, SENSOR_CHAN_TEMP, &temp) < 0)
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
        printk("Humidité : %.2f %%\n", humidite);

        k_sleep(K_SECONDS(10));
    }
}


