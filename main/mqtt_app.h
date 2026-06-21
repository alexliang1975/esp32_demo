#ifndef ESP32_MQTT_APP_H
#define ESP32_MQTT_APP_H

#define DEVICE_STATUS_TOPIC "device/status"
#define DEVICE_STATUS_ONLINE "online"
#define DEVICE_STATUS_OFFLINE "offline"

#define DEEP_SLEEP_TOPIC "device/deep_sleep"

void start_mqtt_client();
void stop_mqtt_client();    
int mqtt_send(const char *topic, const char *payload, bool retain);

#endif