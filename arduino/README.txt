* arduino

Libraries:

WiFi
PubSubClient
Arduino_DebugUtils
ArduinoJson - doch nicht


* mosquitto

remove retained messages, save state

sudo systemctl stop mosquitto
sudo rm /var/lib/mosquitto/mosquitto.db
sudo systemctl start mosquitto