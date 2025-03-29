#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define DELAY_TIME 10000

#define RX_PIN 5
#define TX_PIN 7

#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4
#define BTN4_PIN 6
#define BTN5_PIN 8
#define BTN6_PIN 9
#define BTN7_PIN 10
#define BTN8_PIN 11
#define BTN9_PIN 12
#define BTN10_PIN 42 // was pin 13 sensitive
#define BTN11_PIN 14
#define BTN12_PIN 15

#define IR1_PIN 17
#define IR2_PIN 18
#define IR3_PIN 19
#define IR4_PIN 20
#define IR5_PIN 21
#define IR6_PIN 22
#define IR7_PIN 23
#define IR8_PIN 24
#define IR9_PIN 25
#define IR10_PIN 26
#define IR11_PIN 27
#define IR12_PIN 28

#define RELAY1_PIN 29
#define RELAY2_PIN 30
#define RELAY3_PIN 31
#define RELAY4_PIN 32
#define RELAY5_PIN 33
#define RELAY6_PIN 34
#define RELAY7_PIN 35
#define RELAY8_PIN 36
#define RELAY9_PIN 37
#define RELAY10_PIN 38
#define RELAY11_PIN 39
#define RELAY12_PIN 40

int btn1_state = 0, btn1_old_state = 0;
int btn2_state = 0, btn2_old_state = 0;
int btn3_state = 0, btn3_old_state = 0;
int btn4_state = 0, btn4_old_state = 0;
int btn5_state = 0, btn5_old_state = 0;
int btn6_state = 0, btn6_old_state = 0;
int btn7_state = 0, btn7_old_state = 0;
int btn8_state = 0, btn8_old_state = 0;
int btn9_state = 0, btn9_old_state = 0;
int btn10_state = 0, btn10_old_state = 0;
int btn11_state = 0, btn11_old_state = 0;
int btn12_state = 0, btn12_old_state = 0;

// SoftwareSerial Serial(RX_PIN, TX_PIN);

StaticJsonDocument<350> jsonDocument;
String json_str = "";

void setup()
{
    Serial.begin(9600); // Initialize serial communication
    // Serial.begin(115200);

    // init relay
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    pinMode(RELAY3_PIN, OUTPUT);
    pinMode(RELAY4_PIN, OUTPUT);
    pinMode(RELAY5_PIN, OUTPUT);
    pinMode(RELAY6_PIN, OUTPUT);
    pinMode(RELAY7_PIN, OUTPUT);
    pinMode(RELAY8_PIN, OUTPUT);
    pinMode(RELAY9_PIN, OUTPUT);
    pinMode(RELAY10_PIN, OUTPUT);
    pinMode(RELAY11_PIN, OUTPUT);
    pinMode(RELAY12_PIN, OUTPUT);
    digitalWrite(RELAY1_PIN, HIGH);  // turn off
    digitalWrite(RELAY2_PIN, HIGH);  // turn off
    digitalWrite(RELAY3_PIN, HIGH);  // turn off
    digitalWrite(RELAY4_PIN, HIGH);  // turn off
    digitalWrite(RELAY5_PIN, HIGH);  // turn off
    digitalWrite(RELAY6_PIN, HIGH);  // turn off
    digitalWrite(RELAY7_PIN, HIGH);  // turn off
    digitalWrite(RELAY8_PIN, HIGH);  // turn off
    digitalWrite(RELAY9_PIN, HIGH);  // turn off
    digitalWrite(RELAY10_PIN, HIGH); // turn off
    digitalWrite(RELAY11_PIN, HIGH); // turn off
    digitalWrite(RELAY12_PIN, HIGH); // turn off

    // init btn
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);
    pinMode(BTN4_PIN, INPUT_PULLUP);
    pinMode(BTN5_PIN, INPUT_PULLUP);
    pinMode(BTN6_PIN, INPUT_PULLUP);
    pinMode(BTN7_PIN, INPUT_PULLUP);
    pinMode(BTN8_PIN, INPUT_PULLUP);
    pinMode(BTN9_PIN, INPUT_PULLUP);
    pinMode(BTN10_PIN, INPUT_PULLUP);
    pinMode(BTN11_PIN, INPUT_PULLUP);
    pinMode(BTN12_PIN, INPUT_PULLUP);

    // init ir
    pinMode(IR1_PIN, INPUT_PULLUP);
    pinMode(IR2_PIN, INPUT_PULLUP);
    pinMode(IR3_PIN, INPUT_PULLUP);
    pinMode(IR4_PIN, INPUT_PULLUP);
    pinMode(IR5_PIN, INPUT_PULLUP);
    pinMode(IR6_PIN, INPUT_PULLUP);
    pinMode(IR7_PIN, INPUT_PULLUP);
    pinMode(IR8_PIN, INPUT_PULLUP);
    pinMode(IR9_PIN, INPUT_PULLUP);
    pinMode(IR10_PIN, INPUT_PULLUP);
    pinMode(IR11_PIN, INPUT_PULLUP);
    pinMode(IR12_PIN, INPUT_PULLUP);
}

void loop()
{
    if (Serial.available() > 0)
    {
        String json_str = Serial.readStringUntil('\n');
        deserializeJson(jsonDocument, json_str);

        if (jsonDocument["type"] == 1) // process key perm
        {
            Serial.println("OK!!!");
            Serial.println(json_str);
            int size_perm_key = jsonDocument["k"].size();

            const int key_size = 13;
            bool key_array[key_size] = {0};

            for (int i = 0; i < size_perm_key; i++)
            {
                String str_key_value = jsonDocument["k"][i].as<String>();
                int key_value = jsonDocument["k"][i];
                if (str_key_value == "null")
                {
                    continue;
                }
                key_array[key_value] = 1;
                // Serial.println("key_value: " + String(key_value));
            }
            // for (int i = 0; i < key_size; i++)
            //{
            //     Serial.println("key_array[" + String(i) + "]: " + String(key_array[i]));
            // }

            while (1)
            {
                btn1_state = digitalRead(BTN1_PIN);
                btn2_state = digitalRead(BTN2_PIN);
                btn3_state = digitalRead(BTN3_PIN);
                btn4_state = digitalRead(BTN4_PIN);
                btn5_state = digitalRead(BTN5_PIN);
                btn6_state = digitalRead(BTN6_PIN);
                btn7_state = digitalRead(BTN7_PIN);
                btn8_state = digitalRead(BTN8_PIN);
                btn9_state = digitalRead(BTN9_PIN);
                btn10_state = digitalRead(BTN10_PIN);
                btn11_state = digitalRead(BTN11_PIN);
                btn12_state = digitalRead(BTN12_PIN);
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
                else if (key_array[3] && btn3_state == LOW && btn3_old_state != btn3_state)
                {
                    process_the_key(3, RELAY3_PIN, IR3_PIN);

                    break;
                }
                else if (key_array[4] && btn4_state == LOW && btn4_old_state != btn4_state)
                {
                    process_the_key(4, RELAY4_PIN, IR4_PIN);

                    break;
                }
                else if (key_array[5] && btn5_state == LOW && btn5_old_state != btn5_state)
                {
                    process_the_key(5, RELAY5_PIN, IR5_PIN);

                    break;
                }
                else if (key_array[6] && btn6_state == LOW && btn6_old_state != btn6_state)
                {
                    process_the_key(6, RELAY6_PIN, IR6_PIN);

                    break;
                }
                else if (key_array[7] && btn7_state == LOW && btn7_old_state != btn7_state)
                {
                    process_the_key(7, RELAY7_PIN, IR7_PIN);

                    break;
                }
                else if (key_array[8] && btn8_state == LOW && btn8_old_state != btn8_state)
                {
                    process_the_key(8, RELAY8_PIN, IR8_PIN);

                    break;
                }
                else if (key_array[9] && btn9_state == LOW && btn9_old_state != btn9_state)
                {
                    process_the_key(9, RELAY9_PIN, IR9_PIN);

                    break;
                }
                else if (key_array[10] && btn10_state == LOW && btn10_old_state != btn10_state)
                {
                    process_the_key(10, RELAY10_PIN, IR10_PIN);

                    break;
                }
                else if (key_array[11] && btn11_state == LOW && btn11_old_state != btn11_state)
                {
                    process_the_key(11, RELAY11_PIN, IR11_PIN);

                    break;
                }
                else if (key_array[12] && btn12_state == LOW && btn12_old_state != btn12_state)
                {
                    process_the_key(12, RELAY12_PIN, IR12_PIN);

                    break;
                }
                btn1_old_state = btn1_state;
                btn2_old_state = btn2_state;
                btn3_old_state = btn3_state;
                btn4_old_state = btn4_state;
                btn5_old_state = btn5_state;
                btn6_old_state = btn6_state;
                btn7_old_state = btn7_state;
                btn8_old_state = btn8_state;
                btn9_old_state = btn9_state;
                btn10_old_state = btn10_state;
                btn11_old_state = btn11_state;
                btn12_old_state = btn12_state;
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

    Serial.println("{\"type\":2,\"key\":" + String(btn) + ",\"remarks\":\"" + remarks + "\"}");
}