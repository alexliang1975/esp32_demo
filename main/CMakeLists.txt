

idf_component_register(SRCS "station_example_main.c" 
                        "mqtt_app.c"
                        "ota_https.c"
                        "telemetry.c"
                        "deep_sleep.c"
                    PRIV_REQUIRES nvs_flash esp_https_ota esp_http_client bootloader_support app_update esp_pm esp_driver_gpio
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "isrgrootx1.pem")
