// ESP32 Control for Boiler and Cooler
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


WiFiClient espClient;
PubSubClient client(espClient);

const long reconnectInterval = 4000; // interval in milliseconds
long lastReconnectAttempt = 0;

// only if control is working the cooler or boiler is turned off
// default output is relais on.
// LED Pin
const int ledPin = 21;

void setup() {
  Serial.begin(115200);
  delay(250);
  Debug.setDebugLevel(DBG_VERBOSE);
  Debug.timestampOn();
  DEBUG_INFO("ESP32: MQTT client %s", clientId.c_str());

  delay(10);
  setup_wifi();
  setup_mqtt();

  // Switch on LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void setup_wifi() {
  DEBUG_INFO("Setup WiFi, ssid: %s", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_VERBOSE(".");
  }
  DEBUG_VERBOSE("");
  DEBUG_INFO("WiFi connected, Client IP address: %s", WiFi.localIP().toString().c_str());
}

void reconnectWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_VERBOSE("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  }
}

void setup_mqtt() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  DEBUG_INFO("Setup MQTT, server IP address: %s, port: %d", mqtt_server, mqtt_port);
}

void reconnectMqtt() {
  if (!client.connected()) {
    // Attempt to reconnect
    String newClientId = clientId;
    newClientId += "-" + String(random(0xffff), HEX);
    DEBUG_VERBOSE("Attempting MQTT connection, client: %s, state=%d", newClientId.c_str(), client.state());
    if (client.connect(newClientId.c_str())) {
      DEBUG_INFO("Connected, subscribe to %s, %s", topicSWildcard.c_str(), topicFWildcard.c_str());
      client.subscribe(topicSWildcard.c_str());
      client.subscribe(topicFWildcard.c_str());
    } else {
      DEBUG_VERBOSE("failed, client state=%d, try again in %d ms", client.state(), reconnectInterval);
    }
  }
}


void callback(char* topic, byte* message, unsigned int length) {
  String messageStr;
  String receicedTopic(topic);
  for (unsigned int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }
  DEBUG_VERBOSE("Message arrived on topic: %s, Message: %s", receicedTopic.c_str(), messageStr.c_str());

  // If a message is received on topic
  // Changes the output state according to the message
  if (receicedTopic.equals(topicStr)) {
    int out = LOW; // Turn Relay on
    if (messageStr == "off") {
      out = HIGH; // Turn Relay off
    }

    DEBUG_INFO("Changing output to %d", out);
    digitalWrite(ledPin, out);
  }
}


boolean reconnect() {
  reconnectWifi();
  reconnectMqtt();
  return client.connected();
}


void loop() {
  if (!client.connected()) {
    // Try to reconnect client in a non blocking way
    long now = millis();
    if ((unsigned long)(now - lastReconnectAttempt) > reconnectInterval) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        // success
        lastReconnectAttempt = 0;
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
