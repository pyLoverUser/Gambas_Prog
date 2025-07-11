/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
//const char* ssid = "TCSchwieberdingen";
//const char* password = "TCSchwieberdingen2019";

char* ssid = "StrangerThings";
char* password = "58714188517339106583";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.178.34";
const char* mqtt_server = "192.168.178.2";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

String clientId = "ESP32-S";

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
unsigned int len;
const int R[] = {2,3,4,5,6,7,21,20,8,9,10};
const int numPins = sizeof(R)/sizeof(R[0]);
char buf[13];
char altbuf[13];
char i;
int pbit;
int count=0;
String strpbit;




// LED Pin
const int ledPin = 21;

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("ESP32 Sprenger: MQTT client");
  Serial.println("Number of pins" + numPins);

  for(i=0;i<numPins;i++)
  {
    pinMode(R[i], OUTPUT);
    Serial.println(R[i]);
  }
 //pinMode(R1, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

//  pinMode(ledPin, OUTPUT);
//  digitalWrite(ledPin, HIGH);
}

void setup_wifi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected, IP address: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* message, unsigned int length) {
  String messageStr;
  for (unsigned int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }
  Serial.print("Message arrived on topic: " + String(topic) + ". Message: " + messageStr);
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
    for(i=0;i<11;i++)
    {
      if(buf[i]=='1')
        digitalWrite(R[i], HIGH); // Turn Relays on
      else if(buf[i]=='0')
        digitalWrite(R[i], LOW); // Turn Relays off
      Serial.println(R[i]);
    }


  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/sprenger") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, LOW);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, HIGH);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/sprenger");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void reconnectWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  }
}

void loop() {
  if (!client.connected()) {
    reconnectWifi();
    reconnect();
  }
  client.loop();
}
