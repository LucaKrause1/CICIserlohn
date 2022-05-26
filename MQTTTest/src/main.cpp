#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP32
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)

// WiFi
const char* ssid = "CiCIserlohn";                 // Your personal network SSID
const char* wifi_password = "cic12345"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.4.1";  // IP of the MQTT broker
const char* topic = "test";
const char* mqtt_username = "cic"; // MQTT username
const char* mqtt_password = "cic"; // MQTT password
const char* clientID = "esp32test"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(9600);
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(topic, "Test")) {
    Serial.println("sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println(" failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(topic, "Test");
  }
  client.disconnect();  // disconnect from the MQTT broker
  delay(1000*10);       // print new values every 1 Minute
}