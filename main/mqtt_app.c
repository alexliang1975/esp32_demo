#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "mqtt_app.h"

static char *TAG = "MQTT app";
static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            break;
        case MQTT_EVENT_DELETED:
            ESP_LOGI(TAG, "MQTT_EVENT_DELETED");
            break;
        case MQTT_USER_EVENT:
            ESP_LOGI(TAG, "MQTT USER_EVENT");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void start_mqtt_client()
{
    //TODO: broker address should be configurable, not hardcoded
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = "mqtt://test.mosquitto.org:1883",
        .session.last_will = {
            .topic = DEVICE_STATUS_TOPIC,
            .msg = DEVICE_STATUS_OFFLINE,
            .msg_len = strlen(DEVICE_STATUS_OFFLINE),
            .qos = 1,
        }
    };
    client = esp_mqtt_client_init(&esp_mqtt_client_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void stop_mqtt_client()
{
    if (client) {
       // esp_mqtt_client_stop(client);
        //esp_mqtt_client_destroy(client);
        client = NULL;
    }
}
int mqtt_send(const char *topic, const char *payload, bool retain)
{
    if(client == NULL) {
        ESP_LOGE(TAG, "MQTT client is not initialized");
        return -1;
    }
    return esp_mqtt_client_publish(client, topic, payload, 0, 0, retain);
}