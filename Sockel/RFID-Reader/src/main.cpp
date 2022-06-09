//www.elegoo.com
//2016.12.09
#include <Arduino.h>
#include <SPI.h>
#include <string>

#include <WiFi.h>

#include "MFRC522.h"
#include "PubSubClient.h" // Connect and publish to the MQTT broker

#define RST_PIN   16     // Configurable, see typical pin layout above
#define SS_PIN    5    // Configurable, see typical pin layout above

#define NUMBER_TAGS 6
#define DIFFERENT_TAGS 2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance



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


/*
 * Vorherige Schlüssel aller 12:
 * 1. 04 4C 77 15 39 6C 80
 * 2. 04 23 B7 14 39 6C 80
 * 3. 04 BC EC 14 39 6C 80
 * 4. 04 B3 33 15 39 6C 80
 * 5. 04 95 FA 14 39 6C 80
 * 6. 04 7B C5 14 39 6C 80
 * 7. 04 09 8D 15 39 6C 80
 * 8. 04 59 E7 14 39 6C 80
 * 9. 04 52 A4 15 39 6C 80
 * 10. 04 DE F5 05 49 6C 80
 * 11. 04 36 E7 14 39 6C 80
 * 12. 04 F3 74 15 39 6C 80
 */

MFRC522::MIFARE_Key key;

std::string UIDs[DIFFERENT_TAGS][NUMBER_TAGS] = {
  {//A-Bus
 "04 09 8D 15 39 6C 80",
 "04 59 E7 14 39 6C 80",
 "04 52 A4 15 39 6C 80",
 "04 DE F5 05 49 6C 80",
 "04 36 E7 14 39 6C 80",
 "04 F3 74 15 39 6C 80"},
  {//Baum
 "04 4C 77 15 39 6C 80",
 "04 23 B7 14 39 6C 80",
 "04 BC EC 14 39 6C 80",
 "04 B3 33 15 39 6C 80",
 "04 95 FA 14 39 6C 80",
 "04 7B C5 14 39 6C 80"}
};

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  //while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));
  
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //Wifi
  delay(500);

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
}

/**
 * inspiriert von:
 * https://www.c-plusplus.net/forum/topic/327301/byte-zu-hexstring/11
 */
void hexString(char * bytes, std::size_t bytesLength, char* dest){
  static char const lookup[] = "0123456789ABCDEF";

  for (std::size_t i = 0; i < bytesLength; i++){
    dest[3*i  ] = lookup[ bytes[i] >> 4  ];
    dest[3*i+1] = lookup[ bytes[i] & 0xF ];
    if (i != bytesLength - 1) {
      dest[3*i+2] = ' ';
    }
  }
}

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){

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


void loop() {
  std::string res = "none";
  uint8_t cnt = 0;
  while(cnt < 20 && res == "none") {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      char b[mfrc522.uid.size] = {};
      byte size = mfrc522.uid.size * 2 + mfrc522.uid.size - 1;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        b[i] = mfrc522.uid.uidByte[i];
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();

      // je zwei zeichen + leerzeichen (ohne letzte Zahl) + Ende: null
      char dest[3 * sizeof b];

      hexString(b, sizeof b, dest);
      dest[3 * sizeof b - 1] = '\0';
      // Serial.println(dest);

      // default = 0 -> keine gültige Vitrine
      byte showcase = 0;
      std::string s(dest);

      for (int i = 0; i < DIFFERENT_TAGS; i++) {
        for (int j = 0; j < NUMBER_TAGS; j++) {
          if (s == UIDs[i][j]) {
            showcase = i + 1;
          }
        }
      }
      switch (showcase) {
        case 1:
          res = "abus";
          break;
        case 2:
          res = "tree";
          break;
        default:
          break;
      }

      Serial.println(showcase);
    }else{
      cnt++;
      delay(20);
    }
  }
  

  //Ojekt an Broker übertragen

  connect_MQTT();
  //Serial.setTimeout(2000);

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(topic, res.c_str())) {
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
  
  delay(100);
}
