/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_pm.h"

#include "wifi_connect.h"
#include "mqtt_app.h"
#include "telemetry.h"

static const char *TAG = "esp32 station";

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    connect_to_wifi();

    ESP_LOGI(TAG, "Start MQTT client");
    start_mqtt_client();
    
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            // Running diagnostic function to check if the app is working correctly.
            // If it is, mark the app as valid and cancel the rollback.
            ESP_LOGI(TAG, "Running OTA app, marking as valid");
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                ESP_LOGI(TAG, "App is valid, rollback cancelled successfully");
            } else {
                ESP_LOGE(TAG, "Failed to cancel rollback");
            }
        }
    }

    // Initialize Power Management configuration parameters
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 160,
        .min_freq_mhz = 10,
        .light_sleep_enable = true // Enforces light sleep automatic cycling during idle tasks
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    ESP_LOGI(TAG, "Start Online");
    mqtt_send(DEVICE_STATUS_TOPIC, DEVICE_STATUS_ONLINE, true);


    while (1) {
        send_firmware_telemetry();
        vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10 seconds

    } 
}


