* arduino

Libraries:

WiFi
PubSubClient
Arduino_DebugUtils
ArduinoJson - doch nicht


* mosquitto

remove retained messages, save state

pi@raspberrypi:~ $ sudo systemctl stop mosquitto
pi@raspberrypi:~ $ sudo rm /var/lib/mosquitto/mosquitto.db
pi@raspberrypi:~ $ sudo systemctl start mosquitto