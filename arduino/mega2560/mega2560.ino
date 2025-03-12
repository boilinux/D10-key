#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define DELAY_TIME 2000

#define RX_PIN 5
#define TX_PIN 7

#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4
#define BTN4_PIN 6

#define IR1_PIN 17
#define IR2_PIN 18
#define IR3_PIN 19
#define IR4_PIN 20

#define RELAY1_PIN 29
#define RELAY2_PIN 30
#define RELAY3_PIN 31
#define RELAY4_PIN 32

int btn1_state = 0, btn1_old_state = 0;
int btn2_state = 0, btn2_old_state = 0;
int btn3_state = 0, btn3_old_state = 0;
int btn4_state = 0, btn4_old_state = 0;

SoftwareSerial megaSerial(RX_PIN, TX_PIN);

StaticJsonDocument<250> jsonDocument;
String json_str = "";

void setup()
{
    Serial.begin(115200); // Initialize serial communication
    megaSerial.begin(115200);

    // init btn
    pinMode(BTN1_PIN, INPUT);
    pinMode(BTN2_PIN, INPUT);
    pinMode(BTN3_PIN, INPUT);
    pinMode(BTN4_PIN, INPUT);

    // init ir
    pinMode(IR1_PIN, INPUT);
    pinMode(IR2_PIN, INPUT);
    pinMode(IR3_PIN, INPUT);
    pinMode(IR4_PIN, INPUT);

    // init relay
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    pinMode(RELAY3_PIN, OUTPUT);
    pinMode(RELAY4_PIN, OUTPUT);
    digitalWrite(RELAY1_PIN, HIGH); // turn off
    digitalWrite(RELAY2_PIN, HIGH); // turn off
    digitalWrite(RELAY3_PIN, HIGH); // turn off
    digitalWrite(RELAY4_PIN, HIGH); // turn off
}

void loop()
{
    if (megaSerial.available() > 0)
    {
        String json_str = megaSerial.readStringUntil('\n');
        deserializeJson(jsonDocument, json_str);

        if (jsonDocument["type"] == 1) // process key perm
        {
            int key_perm = jsonDocument["key_perm"];
            int size_perm_key = jsonDocument["key_perm"].size();

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
                key_array[key_value] = 1;
            }

            while (1)
            {
                btn1_state = digitalRead(BTN1_PIN);
                btn2_state = digitalRead(BTN2_PIN);
                btn3_state = digitalRead(BTN3_PIN);
                btn4_state = digitalRead(BTN4_PIN);
                if (key_array[1] && btn1_state == LOW && btn1_old_state != btn1_state)
                {
                    process_the_key(1, RELAY1_PIN, IR1_PIN);

                    break;
                }
                else if (key_array[2] && btn2_state == LOW && btn2_old_state != btn2_state)
                {
                    process_the_key(2, RELAY2_PIN, IR2_PIN);

                    break;
                }
                // else if (key_array[3] && btn3_state == LOW && btn3_old_state != btn3_state)
                //{
                //     process_the_key(3, RELAY3_PIN, IR3_PIN);

                //    break;
                //}
                // else if (key_array[4] && btn4_state == LOW && btn4_old_state != btn4_state)
                //{
                //    process_the_key(4, RELAY4_PIN, IR4_PIN);

                //    break;
                //}
            }
        }
    }
}
void update_relay_state(byte relay_pin)
{
    digitalWrite(relay_pin, LOW);
    delay(DELAY_TIME);
    digitalWrite(relay_pin, HIGH);
}

void process_the_key(int what_key, int relay_pin, int ir_pin)
{
    String remarks = "";
    int ir_state = 0;
    update_relay_state(relay_pin);
    ir_state = check_ir_state(ir_pin);

    if (ir_state == HIGH)
    { // if state is HIGH/1 then key not exist
        remarks = "no_detection";
    }
    else
    {
        remarks = "yes_detection";
    }
    process_key(what_key, remarks);
}

byte check_ir_state(byte ir_pin)
{
    return digitalRead(ir_pin);
}

void process_key(int btn, String remarks)
{

    megaSerial.println("{\"type\":2,\"key\":" + String(btn) + ",\"remarks\":\"" + remarks + "\"}");
}