idf_component_register(SRCS "station_example_main.c" 
                        "mqtt_app.c"
                    PRIV_REQUIRES nvs_flash 
                    INCLUDE_DIRS ".")
