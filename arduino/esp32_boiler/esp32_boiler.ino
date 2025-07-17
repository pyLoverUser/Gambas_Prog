// ESP32 mqtt client for boiler and cooler
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_DebugUtils.h>


//const char* ssid = "TCSchwieberdingen";
//const char* password = "TCSchwieberdingen2019";
const char* ssid = "StrangerThings";
const char* password = "58714188517339106583";

const char* mqtt_server = "192.168.178.2";
const int mqtt_port = 1883;
const String clientId = "boil";
const String topicStr = "tcs/" + clientId + "/out";
const String topicState = "tcs/" + clientId + "/state";


WiFiClient espClient;
PubSubClient client(espClient);

boolean wifiConnected = false;
const long reconnectInterval = 4000; // interval in milliseconds
long lastReconnectAttempt = 0;
int reconnectTries = 0;
const int maxReconnectTries = 5; // maximum number of reconnect tries

// only if control is working the cooler or boiler is turned off
// LED Pin
const int ledPin = 21;
// default output is relais on.
const int defaultPinState = LOW; // Turn Relais on


// setup WiFi events
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  DEBUG_DEBUG("Connected to WiFi AP, SSID: %s", ssid);
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  DEBUG_INFO("WiFi connected, Client IP address: %s", WiFi.localIP().toString().c_str());
  wifiConnected = true;
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  DEBUG_WARNING("Disconnected from WiFi, Reason: %d - Reconnect...", info.wifi_sta_disconnected.reason);
  wifiConnected = false;
  WiFi.begin(ssid, password);
}


void setup() {
  Serial.begin(115200);
  delay(250);
  Debug.setDebugLevel(DBG_INFO);
  Debug.timestampOn();
  DEBUG_INFO("ESP32: MQTT client %s", clientId.c_str());

  pinMode(ledPin, OUTPUT);
  // do not switch before connected
  //digitalWrite(ledPin, HIGH);

  delay(10);
  setup_wifi();
  setup_mqtt();
}

void setup_wifi() {
  DEBUG_INFO("Setup WiFi, ssid: %s", ssid);

  // delete old config
  WiFi.disconnect(true);
  wifiConnected = false;
  delay(200);
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  lastReconnectAttempt = millis();
  WiFi.begin(ssid, password);
}

void setup_mqtt() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  DEBUG_INFO("Setup MQTT, server IP address: %s, port: %d", mqtt_server, mqtt_port);
}

boolean reconnectMqtt() {
  // Attempt to reconnect only if Wifi is connected
  if (wifiConnected && !client.connected()) {
    // Attempt to reconnect only if WiFi is connected
    String newClientId = clientId;
    newClientId += "-" + String(random(0xffff), HEX);
    DEBUG_VERBOSE("Attempting MQTT connection, client: %s, state=%d", newClientId.c_str(), client.state());
    if (client.connect(newClientId.c_str())) {
      DEBUG_INFO("Connected, subscribe to topic:%s", topicStr.c_str());
      delay(100);
      client.subscribe(topicStr.c_str());
    } else {
      DEBUG_VERBOSE("failed, client state=%d, try again in %d ms", client.state(), reconnectInterval);
    }
  }
  return client.connected();
}


void callback(char* topic, byte* message, unsigned int length) {
  String messageStr;
  String receicedTopic(topic);
  for (unsigned int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }
  messageStr.trim();
  DEBUG_DEBUG("Message arrived on topic: %s, Message: %s", topic, messageStr.c_str());

  // If a message is received on topic
  // Changes the output state according to the message
  if (receicedTopic.equals(topicStr)) {
    int out = LOW; // Turn Relay on
    if (messageStr == "off") {
      out = HIGH; // Turn Relay off
    } else if (messageStr == "on") {
      out = LOW; // Turn Relay on
    } else {
      DEBUG_DEBUG("Invalid message for output: '%s'", messageStr.c_str());
      return; // ignore invalid messages
    }

    DEBUG_DEBUG("Changing output to %d", out);
    digitalWrite(ledPin, out);
    client.publish(topicState.c_str(), messageStr.c_str(), true); // publish state
  }
}


void loop() {
  if (!client.connected()) {
    // Try to reconnect client in a non blocking way
    long now = millis();
    if ((unsigned long)(now - lastReconnectAttempt) > reconnectInterval) {
      lastReconnectAttempt = now;
      // Attempt to reconnect only if Wifi is connected
      if (reconnectMqtt()) {
        // success
        lastReconnectAttempt = 0;
        reconnectTries = 0; // reset tries
      } else {
        reconnectTries++;
        if (reconnectTries == maxReconnectTries) {
          DEBUG_WARNING("Reconnect tries exceeded, switch to default output state: %d", defaultPinState);
          digitalWrite(ledPin, defaultPinState); // Turn off LED
        }
      }
    } else {
      // reduce load a little while reconnecting
      delay(100);
    }
  } else {
    client.loop();
  }
  delay(1);  // no need to be so fast
}
