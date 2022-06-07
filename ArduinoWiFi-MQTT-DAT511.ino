//Include libraries
#include <WiFi.h>
#include "PubSubClient.h"

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 7, TXPin = 6;
static const uint32_t GPSBaud = 9600;

#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

float latitude; // gps latitude
float longitude; //gps longitude

// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
//byte server[] = { 10, 0, 1, 3 };

//Setup the server / MQTT broker
//char server[] = "34.89.78.241";
char server[] = "192.168.8.195";

//char ssid[] = "morogoro"; // your network SSID (name)
//char pass[] = "Tanzania2000"; // your network password

char ssid[] = "BTWholeHome-KSNZ"; // your network SSID (name)
char pass[] = "WA6R7inp3tpp"; // your network password


char topic[] = "smartcity/smartbin"; //Set MQTT topic 
int status = WL_IDLE_STATUS; // the Wifi radio's status

//Variable to store the MQTT message
char pubChars[100];

//Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

//Setup objects for WiFi Client and MQTT
WiFiClient wfClient;
PubSubClient client(server, 1883, callback, wfClient);

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup() {

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  
  Serial.begin(9600);
  //ss.begin(9600);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
  }

  Serial.println("You're connected to the network");
  printWifiData();
  printCurrentNet();
}

void loop() {

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)  distance is in cm
  
  //If the client is connented, get the sensor values and publish them to the broker
  if (client.connect("ArduinoClient")) { //Name of the client

    latitude = 50.370765;
    longitude = -4.152740;


    if (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      latitude=gps.location.lat();
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
      longitude = gps.location.lng();
    }
  }

       
    String pubString = "{\"distance\":\"" + String(distance)+"\",\"lat\":\""+String(latitude,6)+"\",\"long\":\""+String(longitude,6)+"\",\"id\":\"0\"}"; //convert number/double to a string
    pubString.toCharArray(pubChars, pubString.length() + 1); //covert string to a character array

    client.publish(topic, pubChars); //publish the message
    Serial.println(pubString);
  } else {
    Serial.println("Client can't connect!");
  }
  client.loop();
  delay(1000); //Send message every one second
}

void printWifiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
}
