/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "TCSchwieberdingen";
const char* password = "TCSchwieberdingen2019";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.178.34";
//const char* mqtt_server = "YOUR_MQTT_BROKER_IP_ADDRESS";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//uncomment the following lines if you're using SPI
//#include <SPI.h>
String clientId = "ESP32-B";

// LED Pin
const int ledPin = 21;

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
//#  Serial.println();
//#  Serial.print("Connecting to ");
//#  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//#    Serial.print(".");
  }

//#  Serial.println("");
//#  Serial.println("WiFi connected");
//#  Serial.println("IP address: ");
//#  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
//#  Serial.print("Message arrived on topic: ");
//#  Serial.print(topic);
//#  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
//#    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
//#  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/boil") {
//#    Serial.print("Changing output to ");
    if(messageTemp == "on"){
//#      Serial.println("on");
      digitalWrite(ledPin, LOW);
    }
    else if(messageTemp == "off"){
//#      Serial.println("off");
      digitalWrite(ledPin, HIGH);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
//#    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
//#      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/boil");
    } else {
//#      Serial.print("failed, rc=");
//#      Serial.print(client.state());
//#      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
