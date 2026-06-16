#ifndef ESP32_WIFI_CONNECT_H
#define ESP32_WIFI_CONNECT_H


 struct wifi_state_t {
    bool connected;
    char ssid[32];
    char ip[16];
} ;

void connect_to_wifi();
struct wifi_state_t *get_wifi_state(void);
void wifi_disconnect(void);



#endif  // ESP32_WIFI_CONNECT_H