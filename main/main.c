#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

/**
 * Log
 */
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

/**
 * Definições Gerais
 */
#define DEBUG 1
#define TAG "main"
#define BUTTON GPIO_NUM_17
#define LED GPIO_NUM_2
#define LED_BLINK_MS 300

/**
 * Protótipos
 */
void task_button(void *pvParameter);
void task_led_blink(void *pvParameter);

/**
 * Task responsavel pela varredura do botao;
 * Ao pressionar button, o valor de counter eh enviado ao log (serial)
 */
void task_button(void *pvParameter)
{
    char str[20];
    int auxBounce = 0;
    int counter = 0;

    if (DEBUG)
        ESP_LOGD(TAG, "task_button run...");

    /**
     * Configura a Button como entrada;
     */
    gpio_pad_select_gpio(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

    for (;;)
    {
        if (gpio_get_level(BUTTON) == auxBounce)
        {
            /**
            * Aguarda 80 ms devido o bounce;
            */
            vTaskDelay(80 / portTICK_PERIOD_MS);

            if (gpio_get_level(BUTTON) == auxBounce)
            {
                auxBounce = !auxBounce;
                if (DEBUG)
                    ESP_LOGD(TAG, "Button %d %s.", BUTTON, auxBounce ? "Pressionado" : "Solto");

                if (auxBounce) // botao pressionado
                {
                    sprintf(str, "%d", counter);
                    ESP_LOGI(TAG, "%s", str);
                    counter++;
                }
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * Task responsavel por piscar o LED
 */
void task_led_blink(void *pvParameter)
{
    if (DEBUG)
        ESP_LOGD(TAG, "task_led_blink run...");

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true)
    {
        gpio_set_level(LED, level);
        level = !level;
        vTaskDelay(LED_BLINK_MS / portTICK_PERIOD_MS);
    }
}

// esp_err_t event_handler(void *ctx, system_event_t *event)
// {
//     return ESP_OK;
// }

void app_main(void)
{
    // possibilita desabilitar log de debug
    if (!DEBUG)
        esp_log_level_set(TAG, ESP_LOG_INFO);

    /*
       Task responsavel em ler estado do botao.
    */
    if (xTaskCreate(task_button, "task_button", 4098, NULL, 2, NULL) != pdTRUE)
    {
        if (DEBUG)
            ESP_LOGE(TAG, "Nao foi possivel alocar task_button.");
        return;
    }

    /*
       Task responsavel em piscar o LED.
    */
    if (xTaskCreate(task_led_blink, "task_led_blink", 4098, NULL, 2, NULL) != pdTRUE)
    {
        if (DEBUG)
            ESP_LOGE(TAG, "Nao foi possivel alocar task_led_blink.");
        return;
    }

    // nvs_flash_init();
    // tcpip_adapter_init();
    // ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // wifi_config_t sta_config = {
    //     .sta = {
    //         .ssid = CONFIG_ESP_WIFI_SSID,
    //         .password = CONFIG_ESP_WIFI_PASSWORD,
    //         .bssid_set = false}};
    // ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    // ESP_ERROR_CHECK(esp_wifi_start());
    // ESP_ERROR_CHECK(esp_wifi_connect());
}
