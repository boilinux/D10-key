#include <Arduino.h>

#define IR_PIN A4
#define IR2_PIN A5

long ir_value = 0, ir2_value = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(IR_PIN, INPUT);
    pinMode(IR2_PIN, INPUT);
}

void loop()
{
    ir_value = digitalRead(IR_PIN);
    ir2_value = digitalRead(IR2_PIN);
    Serial.print("IR1: ");
    Serial.println(ir_value);
    Serial.print("IR2: ");
    Serial.println(ir2_value);

    delay(1000);
}
