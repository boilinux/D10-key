#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SERVER_IP "192.168.1.100:8001"

#ifndef STASSID
#define STASSID ""
#define STAPSK ""
#endif

const int BTN1 = 13; // D7 esp32
int btn1_state = 0, btn1_old_state = 0;

void setup()
{
	Serial.begin(115200);

	pinMode(BTN1, INPUT);

	WiFi.begin(STASSID, STAPSK);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected! IP address: ");
	Serial.println(WiFi.localIP());
}

void loop()
{

	btn1_state = digitalRead(BTN1);

	if (btn1_state == LOW && btn1_old_state != btn1_state)
	{
		sendPostRequest("Button 1 is clicked!");
	}
	btn1_old_state = btn1_state;

	if (Serial.available() > 0)
	{

		String str = Serial.readStringUntil('\n');

		sendPostRequest(str);
	}
}

void sendPostRequest(String str)
{
	// wait for WiFi connection
	if ((WiFi.status() == WL_CONNECTED))
	{

		WiFiClient client;
		HTTPClient http;

		Serial.print("[HTTP] begin...\n");
		// configure traged server and url
		http.begin(client, "http://" SERVER_IP "/api/v1/createnodelog"); // HTTP
		http.addHeader("Content-Type", "application/json");
		http.addHeader("Authorization", "e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8");

		Serial.print("[HTTP] POST...\n");
		// start connection and send HTTP header and body
		String postData = "{\"title\":\"" + str + "\", \"status\":\"returned\"}";
		Serial.println(postData);
		int httpCode = http.POST(postData);

		// httpCode will be negative on error
		if (httpCode > 0)
		{
			// HTTP header has been send and Server response header has been handled
			Serial.printf("[HTTP] POST... code: %d\n", httpCode);

			// file found at server
			if (httpCode == HTTP_CODE_OK)
			{
				const String &payload = http.getString();
				Serial.println("received payload:\n<<");
				Serial.println(payload);
				Serial.println(">>");
			}
		}
		else
		{
			Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	}
}