#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Create RFID instance
MFRC522::MIFARE_Key key;

StaticJsonDocument<250> jsonDocument;
String json_str = "";

void setup()
{
    Serial.begin(115200); // Initialize serial communication
    SPI.begin();          // Init SPI bus
    rfid.PCD_Init();      // Init MFRC522 module
}

void loop()
{
    if (Serial.available() > 0)
    {
        json_str = Serial.readStringUntil('\n');
        deserializeJson(jsonDocument, json_str);

        if (jsonDocument["type"] == 1)
        { // success rfid scan
            Serial.println(json_str);
        }
    }
    get_rfid();
}

void get_rfid()
{
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    {
        return; // No new card
    }

    Serial.print("{\"type\":0,\"rfid\":");
    Serial.print("\"");

    // Print UID of the card
    for (byte i = 0; i < rfid.uid.size; i++)
    {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.print(rfid.uid.uidByte[i], HEX);
    }

    Serial.println("\"}");

    rfid.PICC_HaltA();      // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption
}
