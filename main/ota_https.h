#ifndef ESP32_OTA_HTTPS_H
#define ESP32_OTA_HTTPS_H

#define OTA_COMMAND_TOPIC "device/ota/command"
#define TEST_OTA_URL "https://raw.githubusercontent.com/alexliang1975/test/main/wifi_station.bin"

void store_and_run_ota(const char *url);


#endif