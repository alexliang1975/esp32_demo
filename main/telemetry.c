#include <stdio.h>
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_app_desc.h"
#include "esp_log.h"
#include "telemetry.h"
#include "mqtt_app.h"

#define FIRMWARE_TELEMETRY_TOPIC "device/telemetry/firmware"

static const char *TAG = "TELEMETRY";

/**
 * @brief Send a JSON-formatted telemetry string containing firmware metadata.
 */
void send_firmware_telemetry(void) {
    char buffer[512]; // Temporary buffer for telemetry string

    // 1. Get pointer to the currently executing partition info
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    
    // 2. Fetch the embedded application metadata descriptors
    const esp_app_desc_t *app_desc = esp_app_get_description();

    // 3. Determine slot identification details
    const char *slot_name = running_partition->label;
    
    // 4. Print clean JSON payload directly into your telemetry transport buffer
    snprintf(buffer, sizeof(buffer),
             "{\n"
             "  \"project_name\": \"%s\",\n"
             "  \"running_version\": \"%s\",\n"
             "  \"build_date\": \"%s\",\n"
             "  \"build_time\": \"%s\",\n"
             "  \"idf_version\": \"%s\",\n"
             "  \"runningpartition\": \"%s\"\n"
             "}",
             app_desc->project_name,
             app_desc->version,
             app_desc->date,
             app_desc->time,
             app_desc->idf_ver,
             slot_name);

    ESP_LOGD(TAG, "Sending firmwaretelemetry payload:\n%s", buffer);
    mqtt_send(FIRMWARE_TELEMETRY_TOPIC, buffer, false);

}
