#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_log.h"

#include "mqtt_app.h"

#define WAKEUP_BUTTON_GPIO  GPIO_NUM_4

static char *TAG = "deep sleep";

void enter_deep_sleep_mode(uint64_t sleep_duration_seconds) {
    ESP_LOGW(TAG, "MQTT command received! Preparing for deep sleep configuration.");
    ESP_LOGI(TAG, "Sleep Duration: %llu seconds. Wakeup Pin: GPIO %d", sleep_duration_seconds, WAKEUP_BUTTON_GPIO);


    mqtt_send(DEVICE_STATUS_TOPIC, DEVICE_STATUS_OFFLINE, true);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Stop mqtt client gracefully
    stop_mqtt_client();

    // Source A: Dynamic Time Wakeup
    esp_sleep_enable_timer_wakeup(sleep_duration_seconds * 1000000ULL);

    // Source B: Physical Push Button Wakeup
    esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown(1ULL << WAKEUP_BUTTON_GPIO, ESP_GPIO_WAKEUP_GPIO_LOW);

    // Flush serial buffers and cut execution
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGW(TAG, "Entering Deep Sleep Now. Goodbye.");
    
    esp_deep_sleep_start();
}