//www.elegoo.com
//2016.12.09
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <string>

#include <WiFi.h>

#define RST_PIN   16     // Configurable, see typical pin layout above
#define SS_PIN    5    // Configurable, see typical pin layout above

#define NUMBER_TAGS 6
#define DIFFERENT_TAGS 2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

const char* ssid = "cic";
const char* password =  "cic";

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
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));
  
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //WIFI
  WiFi.begin(ssid, password);
 
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Ich verbinde mich mit dem Internet...");
  // }
  // Serial.println("Ich bin mit dem Internet verbunden!");
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


void loop() {
  // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  
  char b[mfrc522.uid.size] = {};
  byte size = mfrc522.uid.size * 2 + mfrc522.uid.size - 1;
  for (byte i = 0; i < mfrc522.uid.size; i++) {    
    b[i] = mfrc522.uid.uidByte[i];
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  //je zwei zeichen + leerzeichen (ohne letzte Zahl) + Ende: null
  char dest[3*sizeof b];

  hexString(b, sizeof b, dest);
  dest[3*sizeof b - 1] = '\0';
  //Serial.println(dest);

  //default = 0 -> keine gültige Vitrine
  byte showcase = 0;
  std::string s(dest);

  for(int i = 0; i < DIFFERENT_TAGS; i++) {
    for(int j = 0; j < sizeof NUMBER_TAGS; j++) {
      if (s == UIDs[i][j]) {
        showcase = i + 1;
      }
    }
  }
  
  Serial.println(showcase);
  //TODO: Zahl weitergeben!!! -> über WLAN oder Kabel
  
  
  delay(1000);
}
