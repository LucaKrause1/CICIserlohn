#include <Arduino.h>
#include "PubSubClient.h" 
#include "WiFi.h"
#include <CheapStepper.h>

// WiFi
const char* ssid = "CiCIserlohn";                 // Your personal network SSID
const char* wifi_password = "cic12345"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.4.1";  // IP of the MQTT broker
const char* topic = "vitrinen/abusData";
const char* mqtt_username = "cic"; // MQTT username
const char* mqtt_password = "cic"; // MQTT password
const char* clientID = "vitrineABus"; // MQTT client ID

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); 


// next, declare the stepper
// and connect pins 8,9,10,11 to IN1,IN2,IN3,IN4 on ULN2003 board
CheapStepper stepper (32,33,25,26); 

boolean moveClockwise = true;
uint16_t currentDegrees = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String inString = "";

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    inString += (char)payload[i];
  }
  Serial.println();

  uint16_t newDegerees = inString.toInt();
  if(currentDegrees != newDegerees){
    int32_t difference = newDegerees-currentDegrees;
    if((difference>0 && difference<180) || (difference<-180)){
      moveClockwise = true;
    } else {
      moveClockwise = false;
    }
    currentDegrees = newDegerees;
    Serial.println("Moving to " + String(newDegerees));
    stepper.moveDegrees(moveClockwise, abs(difference));
    Serial.println("Moving completed");
  }
}

void connect_MQTT(){
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void setup() {
  Serial.begin(9600);

  delay(500);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  stepper.setRpm(10); 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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