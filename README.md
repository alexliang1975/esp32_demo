

# ESP32 Station Example

This example demostrates below functions:
    * Use the Wi-Fi Station functionality of the Wi-Fi driver of ESP for connecting to an Access Point.
    * Use ESP MQTT Client connects to an MQTT broker over TLS and demonstrates basic operations: connect, subscribe, publish, receive data, and unsubscribe.

## Configure the example

### Configure WiFi

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the Wi-Fi configuration.
    * Set `WiFi SSID`.
    * Set `WiFi Password`.

Optional: If you need, change the other options according to your requirements.

## Troubleshooting
### Fail to connect to  WiFi SSID with Smart Connect mode

Some modem WiFi router such as ASUS router support smart connection mode which combine 2.4G/5G, but ESP32/ESP32C3 only support 2.4GHz, needs to config 2.4G only SSID.

   ** Disable Smart connect mode
   ** Disable 802.11ax / WiFi 6 mode
   ** Disable Protection manage Frame(PMF)
### Power supply dropouts (brownouts) during radio initialization when powered by USB



