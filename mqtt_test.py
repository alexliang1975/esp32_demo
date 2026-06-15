import time
import paho.mqtt.client as mqtt

# --- Configuration ---
# Note the typo fix from your prompt: we need a colon before the port number
BROKER = "test.mosquitto.org"
PORT = 1883
STATUS_TOPIC = "device/status"
TELEMETRY_TOPIC = "device/telemetry"


# --- Event Handlers ---
def on_connect(client, userdata, flags, reason_code, properties=None):
    """Callback triggered when the script successfully connects to the broker."""
    if reason_code == 0:
        print(f" Successfully connected to {BROKER}!")
        
        # Subscribe to both the status topic and the telemetry topic
        client.subscribe(STATUS_TOPIC)
        client.subscribe(TELEMETRY_TOPIC)
        print(f" Subscribed to topics:\n   - {STATUS_TOPIC}\n   - {TELEMETRY_TOPIC}")
        print("Waiting for ESP32 data...\n" + "="*40)
    else:
        print(f"❌ Connection failed with result code {reason_code}")

def on_message(client, userdata, msg):
    """Callback triggered whenever a new message arrives on a subscribed topic."""
    # Convert binary payload bytes to a readable string
    payload = msg.payload.decode("utf-8")
    
    # Check if the message was stored on the broker (Retained)
    retain_flag = "[RETAINED]" if msg.retain else ""
    
    print(f" Topic:   {msg.topic} {retain_flag}")
    print(f" Payload: {payload}")
    print(f" QoS:     {msg.qos}")
    print("-" * 40)


# --- Main Logic ---
def main():
    # Setup the MQTT client (using MQTTv5 for modern compliance)
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    
    # Assign our custom callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    print(f"Connecting to broker: {BROKER}...")
    try:
        client.connect(BROKER, PORT, keepalive=60)
    except Exception as e:
        print(f"❌ Could not connect to broker: {e}")
        return

    # Start a non-blocking background loop that handles network traffic & reconnects
    client.loop_start()

    try:
        # Keep the main thread alive so the background loop can print messages
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nStopping receiver script. Goodbye!")
    finally:
        client.loop_stop()
        client.disconnect()

if __name__ == "__main__":
    main()