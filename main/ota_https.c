
#include "esp_log.h"
#include "esp_err.h"

#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "ota_https.h"
#include "esp_app_desc.h" // Required to parse the application description headers

// This certificate is required to verify the HTTPS server's identity
extern const uint8_t server_cert_pem_start[] asm("_binary_isrgrootx1_pem_start");

static const char *TAG = "OTA HTTPS";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_HEADERS_COMPLETE:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADERS_COMPLETE");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    default:
        break;
    }
    return ESP_OK;
}

void store_and_run_ota(const char *url)
{
    if(url == NULL) {
        url = TEST_OTA_URL; // Use default OTA URL if none provided
    }
    ESP_LOGI(TAG, "Starting OTA update from URL: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = (char *)server_cert_pem_start, // HTTPS certificate
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };
   esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    
    esp_https_ota_handle_t ota_handle = NULL;
    
    // 1. Establish connection and download the image header configuration
    esp_err_t err = esp_https_ota_begin(&ota_config, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed: %d", err);
        return;
    }

    // 2. Extract the Application Description struct from the incoming stream binary
    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(ota_handle, &app_desc);
    
    if (err == ESP_OK) {
        // --- OUTPUT FIRMWARE INFORMATION ---
        ESP_LOGI(TAG, "=============================================");
        ESP_LOGI(TAG, "       INCOMING BINARY IMAGE DETAILS         ");
        ESP_LOGI(TAG, "=============================================");
        ESP_LOGI(TAG, "Project Name : %s", app_desc.project_name);
        ESP_LOGI(TAG, "Version      : %s", app_desc.version);
        ESP_LOGI(TAG, "Compile Date : %s", app_desc.date);
        ESP_LOGI(TAG, "Compile Time : %s", app_desc.time);
        
        // Print the custom IDF version used to compile this image
        ESP_LOGI(TAG, "IDF Version  : %s", app_desc.idf_ver);
        
        // Print secure app binary SHA256 hash (truncated representation)
        char sha256_str[65];
        for (int i = 0; i < 32; i++) {
            sprintf(&sha256_str[i * 2], "%02x", app_desc.app_elf_sha256[i]);
        }
        ESP_LOGI(TAG, "ELF SHA256   : %s", sha256_str);
        ESP_LOGI(TAG, "=============================================");

        // --- OPTIONAL: ANTI-ROLLBACK / SAME VERSION CHECK ---
        // If you want to abort if the incoming version matches your current version:
        // const esp_app_desc_t *running_desc = esp_app_get_description();
        // if (strcmp(running_desc->version, app_desc.version) == 0) { 
        //     ESP_LOGW(TAG, "Firmware version matches current image. Aborting.");
        //     esp_https_ota_abort(ota_handle);
        //     return;
        // }

    } else {
        ESP_LOGE(TAG, "Failed to read image descriptor header metadata: %d", err);
        esp_https_ota_abort(ota_handle);
        return;
    }

    // 3. Proceed to loop through the remainder of the data payload stream chunks
    ESP_LOGI(TAG, "Header validation passed. Streaming binary chunks into flash memory...");
    while (1) {
        err = esp_https_ota_perform(ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break; // Finish looping when error status changes from "In Progress"
        }
        // You can print a loading spinner or update progress metrics here
    }

    // 4. Validate final structural wrap-up 
    if (esp_https_ota_is_complete_data_received(ota_handle)) {
        err = esp_https_ota_finish(ota_handle);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "OTA Download validation complete! Rebooting device...");
            esp_restart();
        } else {
            ESP_LOGE(TAG, "OTA Flash signature closure processing failed: %d", err);
        }
    } else {
        ESP_LOGE(TAG, "Stream disconnected before complete binary payload packet transmission completed.");
        esp_https_ota_abort(ota_handle);
    }
}
