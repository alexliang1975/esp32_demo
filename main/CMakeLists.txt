

idf_component_register(SRCS "station_example_main.c" 
                        "mqtt_app.c"
                        "ota_https.c"
                    PRIV_REQUIRES nvs_flash esp_https_ota esp_http_client bootloader_support app_update
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "isrgrootx1.pem")
