/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_DebugUtils.h>


// Replace the next variables with your SSID/Password combination
//const char* ssid = "TCSchwieberdingen";
//const char* password = "TCSchwieberdingen2019";

const char* ssid = "StrangerThings";
const char* password = "58714188517339106583";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.178.34";
const char* mqtt_server = "192.168.178.2";
const int mqtt_port = 1883;
const String clientId = "ESP32-S1";
const char* topic1 = "tcs/sprenger/ausgabe/#";
const char* topic2 = "tcs/flutlicht/ausgabe/#";


WiFiClient espClient;
PubSubClient client(espClient);

// GPIOs für die Sprenger
/*
  #define R1  D0 // ESP32 pin GPIO2 connected to R1
  #define R2  D1 // ESP32 pin GPIO3 connected to R2
  #define R3  D2 // ESP32 pin GPIO4 connected to R3
  #define R4  D3 // ESP32 pin GPIO5 connected to R4
  #define R5  D4 // ESP32 pin GPIO6 connected to R5
  #define R6  D5 // ESP32 pin GPIO7 connected to R6
  #define R7  D6 // ESP32 pin GPIO21 connected to R7
  #define R8  D7 // ESP32 pin GPIO20 connected to R8
  #define R9  D8 // ESP32 pin GPIO8 connected to R9
  #define R10  D9 // ESP32 pin GPIO9 connected to R10
  #define R11  D10 // ESP32 pin GPIO10 connected to R11
*/
const int R[] = {2, 3, 4, 5, 6, 7, 21, 20, 8, 9, 10};
const int numPins = sizeof(R) / sizeof(R[0]);
char buf[13];
char altbuf[13];
int pbit;
int count = 0;
String strpbit;

void setup() {
  Serial.begin(115200);
  delay(250);
  Debug.setDebugLevel(DBG_VERBOSE);
  Debug.timestampOn();
  DEBUG_INFO("ESP32 Sprenger: MQTT client");
  DEBUG_VERBOSE("Number of pins: %d", numPins);

  for (int i = 0; i < numPins; i++)
  {
    DEBUG_VERBOSE("Set pin %d to OUTPUT", R[i]);
    pinMode(R[i], OUTPUT);
  }

  setup_wifi();
  setup_mqtt();
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
  // Loop until we're reconnected
  while (!client.connected()) {
    String newClientId = clientId;
    newClientId += String(random(0xffff), HEX);
    DEBUG_VERBOSE("Attempting MQTT connection, client: %s", newClientId.c_str());
    if (client.connect(clientId.c_str())) {
      DEBUG_INFO("Connected, subscribe to %s, %s", topic1, topic2);
      client.subscribe(topic1);
      client.subscribe(topic2);
    } else {
      DEBUG_VERBOSE("failed, client state=%d, try again in 5 seconds", client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* message, unsigned int length) {
  String messageStr;
  String topicStr(topic);
  for (unsigned int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }
  DEBUG_VERBOSE("Message arrived on topic: %s, Message: %s", topic, messageStr.c_str());

  // If a message is received on the topic tcs/sprenger/ausgabe,
  // you check if the message is either "true" or "false".
  // Changes the output state according to the message
  if (topicStr.startsWith("tcs/sprenger/ausgabe")) {
    // parse number from topic
    int lastSlash = topicStr.lastIndexOf('/');
    String sprengerNumberStr = topicStr.substring(lastSlash);
    int sprengerNumber = sprengerNumberStr.toInt();

                         // check sprenger number
    if (sprengerNumber < 1 || sprengerNumber > 7) {
      DEBUG_VERBOSE("Sprenger Number %s not valid on this ESP32", sprengerNumberStr.c_str());
      return;  //ignore
    }

    int out = LOW; // Turn Relays off
    if (messageStr == "true") {
      out = HIGH; // Turn Relays on
    }
    DEBUG_INFO("Changing output %s to %d", sprengerNumberStr.c_str(), out);
    digitalWrite(R[sprengerNumber], out);
  }
}


void loop() {
  if (!client.connected()) {
    reconnectWifi();
    reconnectMqtt();
  }
  client.loop();
}
