
// ESP32 #2: TCP SERVER 
#include <WiFi.h>
#include <stdio.h>
#include <string.h>
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
#define SERVER_PORT 80
unsigned int len;
const int R[] = {2,3,4,5,6,7,21,20,8,9,10};
const int numPins = sizeof(R)/sizeof(R[0]);
char buf[13];
char altbuf[13];
const char* ssid = "xx";     // CHANGE TO YOUR WIFI SSID
const char* password = "xx"; // CHANGE TO YOUR WIFI PASSWORD
char i;
int pbit;
int count=0;
String strpbit;
WiFiServer TCPserver(SERVER_PORT);

void setup() {
  Serial.begin(115200);
  Serial.println(numPins);
  for(i=0;i<numPins;i++)
  {
    pinMode(R[i], OUTPUT);
    Serial.println(R[i]);
  }
 //pinMode(R1, OUTPUT);

  Serial.println("ESP32 #2: TCP SERVER + AN LED");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print your local IP address:
  Serial.print("ESP32 #2: TCP Server IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("ESP32 #2: -> Please update the serverAddress in ESP32 #1 code");

  // Start listening for a TCP client (from ESP32 #1)
  TCPserver.begin();
}

void loop() 
{
  // Wait for a TCP client from ESP32 #1:
  WiFiClient client = TCPserver.available();

  if (client) 
  {
    // Read the command from the TCP client:
    String command = client.readStringUntil('\r');
    String aktInput=command.substring(0,11);
    strpbit=command.substring(11);
    Serial.println(command);
    Serial.println(aktInput);
    // Serial.println(strpbit);
    pbit=strpbit.toInt();
    count=0;
    for(i=0;i<11;i++) 
    {
      if(aktInput[i] == '1')
      {
        count++;
      }
    }
    // Serial.println(pbit);
    // Serial.println(count);
    // Serial.print("ESP32 #2: - Received command: ");
    //len = aktInput.length();
    command.toCharArray(buf, 12);
    Serial.println(altbuf);
    Serial.println(buf);
    if(memcmp(buf,altbuf,sizeof(buf)) !=0)
    {
    for(i=0;i<11;i++)
    {
      if(buf[i]=='1')
        digitalWrite(R[i], HIGH); // Turn Relays on
      else if(buf[i]=='0')
        digitalWrite(R[i], LOW); // Turn Relays off
      Serial.println(R[i]);
    }
    Serial.println(aktInput);
    memcpy(altbuf,buf, sizeof(buf));
    }
    client.stop();
  }
}
