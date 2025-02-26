#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

#define DELAY_TIME 2000
#define DELAY_TIME_DISPLAY 3000

#define SS_PIN 10
#define RST_PIN 9

#define BUZZER_PIN 8

#define BTN1_PIN A0
#define BTN2_PIN A1
#define BTN3_PIN A2
#define BTN4_PIN A3

#define IR1_PIN 6
#define IR2_PIN 5

#define RELAY1_PIN 7

int btn1_state = 0, btn1_old_state = 0;
int btn2_state = 0, btn2_old_state = 0;
int btn3_state = 0, btn3_old_state = 0;
int btn4_state = 0, btn4_old_state = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address

MFRC522 rfid(SS_PIN, RST_PIN); // Create RFID instance
MFRC522::MIFARE_Key key;

StaticJsonDocument<250> jsonDocument;
String json_str = "";
String str_rfid = "";

void setup()
{
    Serial.begin(115200); // Initialize serial communication
    SPI.begin();          // Init SPI bus
    rfid.PCD_Init();      // Init MFRC522 module

    // init lcd
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    // init buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW); // turn off

    // init buttons
    pinMode(BTN1_PIN, INPUT);
    // init relay
    pinMode(RELAY1_PIN, OUTPUT);
    // init ir
    pinMode(IR1_PIN, INPUT);
    pinMode(IR2_PIN, INPUT);

    digitalWrite(RELAY1_PIN, HIGH); // turn off

    init_display();
}

void beep()
{
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
}

void init_display()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready");
    lcd.setCursor(0, 1);
    lcd.print("Scan RFID");
}

void loop()
{
    if (Serial.available() > 0)
    {
        json_str = Serial.readStringUntil('\n');
        deserializeJson(jsonDocument, json_str);
        int size_perm_key = jsonDocument.size();

        if (jsonDocument["type"] == 1)
        {
            // success rfid scan
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Please wait...");

            beep();
            Serial.println(json_str);

            byte ir_state = 0;

            // what key permission
            String str_key_perm = "";
            const int key_size = 12;
            bool key_array[key_size] = {0};
            for (int i = 0; i < size_perm_key; i++)
            {
                String str_key_value = jsonDocument["key_perm"][i].as<String>();
                int key_value = jsonDocument["key_perm"][i];
                if (str_key_value == "null")
                {
                    continue;
                }

                str_key_perm += " " + str_key_value;
                key_array[key_value] = 1;
            }
            // display on lcd
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Key Permission");
            lcd.setCursor(0, 1);
            lcd.print(str_key_perm);

            while (1) // press button what key
            {
                btn1_state = digitalRead(BTN1_PIN);
                btn2_state = digitalRead(BTN2_PIN);
                btn3_state = digitalRead(BTN3_PIN);
                btn4_state = digitalRead(BTN4_PIN);

                if (key_array[1] && btn1_state == LOW && btn1_state != btn1_old_state)
                {

                    process_the_key(1, RELAY1_PIN, IR1_PIN);
                    lcd_display_success();

                    break;
                }
                if (key_array[2] && btn2_state == LOW && btn2_state != btn2_old_state)
                {
                    process_the_key(2, RELAY1_PIN, IR2_PIN);
                    lcd_display_success();

                    break;
                }

                btn1_old_state = btn1_state;
                btn2_old_state = btn2_state;
                btn3_old_state = btn3_state;
                btn4_old_state = btn4_state;
            }
            beep();
            init_display();
        }
        else if (jsonDocument.containsKey("reset"))
        {
            init_display();
        }
        else if (jsonDocument.containsKey("msg"))
        {
            String msg = jsonDocument["msg"];
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(msg);
        }
        else if (jsonDocument.containsKey("error"))
        {
            String error_msg = jsonDocument["error"];
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(error_msg);
            if (error_msg == "try again" && str_rfid != "")
            {
                Serial.print("{\"type\":0,\"rfid\":\"" + str_rfid + "\"}");
            }
        }
        jsonDocument.clear();
    }
    get_rfid();
}

void lcd_display_success()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Success");
    delay(DELAY_TIME_DISPLAY);
}

byte check_ir_state(byte ir_pin)
{
    return digitalRead(ir_pin);
}

void update_relay_state(byte relay_pin)
{
    digitalWrite(relay_pin, LOW);
    delay(DELAY_TIME);
    digitalWrite(relay_pin, HIGH);
}

void process_key(int btn, String remarks)
{

    Serial.println("{\"type\":2,\"key\":" + String(btn) + ",\"remarks\":\"" + remarks + "\"}");
}

void process_the_key(int what_key, int relay_pin, int ir_pin)
{
    String remarks = "";
    int ir_state = 0;
    update_relay_state(relay_pin);
    ir_state = check_ir_state(ir_pin);

    if (ir_state == HIGH)
    { // if state is HIGH/1 then key not exist
        remarks = "Failed";
    }
    else
    {
        remarks = "Success";
    }
    process_key(what_key, remarks);
}

void get_rfid()
{
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    {
        return; // No new card
    }

    str_rfid = "";

    Serial.print("{\"type\":0,\"rfid\":");
    Serial.print("\"");

    // Print UID of the card
    for (byte i = 0; i < rfid.uid.size; i++)
    {
        str_rfid += rfid.uid.uidByte[i] < 0x10 ? "0" : "";
        str_rfid += rfid.uid.uidByte[i];
    }

    Serial.print(str_rfid);

    Serial.println("\"}");

    rfid.PICC_HaltA();      // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption
}
