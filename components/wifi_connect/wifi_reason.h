// Helper to convert wifi_err_reason_t to human-readable string
#ifndef WIFI_REASON_H
#define WIFI_REASON_H

#include "esp_wifi.h"

const char* wifi_reason_to_string(wifi_err_reason_t reason);


#endif // WIFI_REASON_H
