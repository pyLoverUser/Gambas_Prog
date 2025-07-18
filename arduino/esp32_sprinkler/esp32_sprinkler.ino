// ESP32 mqtt client for sprinkler and floodlight
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_DebugUtils.h>


//const char* ssid = "TCSchwieberdingen";
//const char* password = "TCSchwieberdingen2019";
const char* ssid = "StrangerThings";
const char* password = "58714188517339106583";

const char* mqtt_server = "192.168.178.2";
const int mqtt_port = 1883;
const String clientId = "court1";
// const String clientId = "court6";

const String topicSprinkler = "tcs/sprink/out";
const String topicFloodlight = "tcs/light/out";
const String topicSWildcard = String(topicSprinkler + "/#");
const String topicFWildcard = String(topicFloodlight + "/#");

const String topicSprinklerState = "tcs/sprink/state";
const String topicFloodlightState = "tcs/light/state";


WiFiClient espClient;
PubSubClient client(espClient);

boolean wifiConnected = false;
const long reconnectInterval = 4000; // interval in milliseconds
long lastReconnectAttempt = 0;
const int sprinklerTimeout = 15 * 1000; // timeout in milliseconds for sprinkler to turn off automatically
const int lightTimeout = 60 * 1000; // timeout in milliseconds for floodlight to turn off automatically

const long heartBeatInterval = 6000; // interval in milliseconds
long lastHeartBeat = 0;


// GPIOs für die Sprinkler
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

const int maxSprinklers = 7; // maximum number of sprinklers
// Sprinkler configuration, number is index in array R, 0xff = not used
const int sprinklerConfig[maxSprinklers] = {0, 1, 3, 0xff, 0xff, 0xff, 0xff};

// store the sprinkler start times into an array
// index 0 starts with sprinkler 1
long sprinklerStart[maxSprinklers] = {0, 0, 0, 0, 0, 0, 0};
int currentSprinklerState[maxSprinklers] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// floodlight config
const int maxLigths = 2; // maximum number of flood lights
// floodlight configuration, number is index in array R
const int lightConfig[maxLigths] = {8, 9};
long lightStart = 0;
int currentLightState = 0xff; // 0xff = unknown, 0 = off, 1 = on


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
  Debug.setDebugLevel(DBG_VERBOSE);
  Debug.timestampOn();
  DEBUG_INFO("ESP32 Sprinkler and Floddlight: MQTT client %s", clientId.c_str());
  DEBUG_VERBOSE("Number of pins: %d", numPins);

  for (int i = 0; i < numPins; i++)
  {
    DEBUG_VERBOSE("Set pin %d to OUTPUT", R[i]);
    pinMode(R[i], OUTPUT);
  }

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
      DEBUG_INFO("Connected, subscribe to %s, %s", topicSWildcard.c_str(), topicFloodlight.c_str());
      delay(100);
      client.subscribe(topicSWildcard.c_str());
      if (maxLigths > 0) {
        client.subscribe(topicFloodlight.c_str());
      }
    } else {
      DEBUG_VERBOSE("failed, client state=%d, try again in %d ms", client.state(), reconnectInterval);
    }
  }
  return client.connected();
}


void sendState() {
  String state = "{\"sprinklers\":[";
  for (int i = 0; i < maxSprinklers; i++) {
    state += String(currentSprinklerState[i]);
    if (i < maxSprinklers - 1) {
      state += ",";
    }
  }
  state += "],\"floodlight\":" + String(currentLightState) + "}";
  boolean success = client.publish(topicSprinklerState.c_str(), state.c_str(), false); // publish state
  if (success) {
    DEBUG_VERBOSE("Published state '%s' to topic '%s'", state.c_str(), topicSprinklerState.c_str());
    lastHeartBeat = millis(); // update last heartbeat time
  }
}


void changeSprinkler(int sprinklerNumber, String turnOn) {
  DEBUG_DEBUG("changeSprinkler number: %d, turnOn:'%s'", sprinklerNumber, turnOn.c_str());
  // checks have been done before
  int outPin = R[sprinklerConfig[sprinklerNumber - 1]];
  int outState = LOW;
  if (turnOn.equals("true")) {
    outState = HIGH; // Turn Relays on
    sprinklerStart[sprinklerNumber - 1] = millis(); // store start time
  } else if (turnOn.equals("false")) {
    outState = LOW; // Turn Relays off
    sprinklerStart[sprinklerNumber - 1] = 0; // reset start time
  } else {
    DEBUG_DEBUG("Invalid message for sprinkler %d: '%s'", sprinklerNumber, turnOn.c_str());
    return; // ignore invalid messages
  }
  DEBUG_INFO("Changing sprinkler %d output %d to %d", sprinklerNumber, outPin, outState);
  digitalWrite(outPin, outState);
  currentSprinklerState[sprinklerNumber - 1] = (outState == HIGH) ? 1 : 0; // store current state
  sendState(); // publish state
}

void changeLight(String turnOn) {
  DEBUG_DEBUG("changeLight, turnOn:'%s'", turnOn.c_str());
  // checks have been done before
  int outPin1 = R[lightConfig[0]];
  int outPin2 = R[lightConfig[1]];
  int outState = LOW;
  if (turnOn.equals("true")) {
    outState = HIGH; // Turn Relays on
    lightStart = millis(); // store start time
  } else if (turnOn.equals("false")) {
    outState = LOW; // Turn Relays off
    lightStart = 0; // reset start time
  } else {
    DEBUG_DEBUG("Invalid message for floodlight: '%s'", turnOn.c_str());
    return; // ignore invalid messages
  }
  // set both outputs to the same state
  DEBUG_INFO("Changing light output 1:%d and 2:%d to %d", outPin1, outPin2, outState);
  digitalWrite(outPin1, outState);
  digitalWrite(outPin2, outState);
  currentLightState = (outState == HIGH) ? 1 : 0; // store current state
  sendState();
}


void checkSprinklerTimeouts() {
  long now = millis();
  for (int i = 0; i < maxSprinklers; i++) {
    // if the sprinkler is not configured, ignore it
    if (sprinklerConfig[i] == 0xff) {
      continue;
    }
    unsigned long timeSinceStart = (unsigned long)(now - sprinklerStart[i]);
    if (timeSinceStart > sprinklerTimeout) {
      if ((sprinklerStart[i] > 0) || (currentSprinklerState[i] == 0xff)) {
        // this includes if no information has been received after startup
        DEBUG_WARNING("Sprinkler %d timeout (%d s) reached, turning off", i + 1, timeSinceStart / 1000);
        changeSprinkler(i + 1, "false");
      }
    }
  }
}


void checkLightTimeout() {
  if (maxLigths <= 0) {
    return; // no lights configured
  }
  long now = millis();
  unsigned long timeSinceStart = (unsigned long)(now - lightStart);
  if (timeSinceStart > lightTimeout) {
    if ((lightStart > 0) || (currentLightState == 0xff)) {
      // this includes if no information has been received after startup
      DEBUG_WARNING("Floodlight timeout (%d s) reached, turning off", timeSinceStart / 1000);
      changeLight("false");
    }
  }
}


void callback(char* topic, byte* message, unsigned int length) {
  String messageStr;
  String receicedTopic(topic);
  for (unsigned int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }
  messageStr.trim();
  DEBUG_VERBOSE("Message arrived on topic: %s, Message: %s", topic, messageStr.c_str());

  // If a message is received on subscribed topics,
  // you check if the message is either "true" or "false".
  // Changes the output state according to the message
  if (receicedTopic.startsWith(topicSprinkler)) {
    // parse number from topic
    String sprinklerNumberStr = receicedTopic.substring(receicedTopic.lastIndexOf('/') + 1);
    sprinklerNumberStr.trim();

    // check for debug topic like tcs/sprink/out/debug
    if (sprinklerNumberStr.equals("debug")) {
      Debug.setDebugLevel(messageStr.toInt());
      return;
    }

    int sprinklerNumber = sprinklerNumberStr.toInt();  // in case of failure sprinklerNumber will be 0

    // check sprinkler number range
    if (sprinklerNumber < 1 || sprinklerNumber > maxSprinklers) {
      DEBUG_INFO("Sprinkler number not valid, topic=%s", receicedTopic.c_str());
      return;  //ignore
    }
    // check sprinkler configured, normal because all sprinkler topics are received
    if (sprinklerConfig[sprinklerNumber - 1] == 0xff) {
      DEBUG_VERBOSE("Sprinkler %d not configured, topic=%s", sprinklerNumber, receicedTopic.c_str());
      return;  //ignore
    }
    changeSprinkler(sprinklerNumber, messageStr);

    // If a message is received on the floodlight topic,
  } else if (receicedTopic.equals(topicFloodlight)) {
    changeLight(messageStr);
  } else {
    DEBUG_DEBUG("Received message on unknown topic: %s", receicedTopic.c_str());
  }
}


void loop() {
  // ensure that sprinklers or lights are turned off after timeout
  // even if MQTT connection is not available
  checkSprinklerTimeouts();
  checkLightTimeout();

  if (!client.connected()) {
    // Try to reconnect client in a non blocking way
    long now = millis();
    if ((unsigned long)(now - lastReconnectAttempt) > reconnectInterval) {
      lastReconnectAttempt = now;
      // Attempt to reconnect only if Wifi is connected
      if (reconnectMqtt()) {
        // success
        lastReconnectAttempt = 0;
      }
    } else {
      // reduce load a little while reconnecting
      delay(100);
    }
  } else {
    client.loop();
    // send heartbeat to MQTT broker
    unsigned long lastTime = (unsigned long)(millis() - lastHeartBeat);
    if (lastTime > heartBeatInterval) {
      sendState(); // send current state
    }
  }
  delay(1);  // no need to be so fast
}
