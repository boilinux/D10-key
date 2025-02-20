#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define SERVER_IP "192.168.1.100:8001"
#define STR_TOKEN "e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8"

#ifndef STASSID
#define STASSID "Kainoa"
#define STAPSK "@starX@2024"
#endif

StaticJsonDocument<250> jsonDocument;
String json_str = "";

void setup()
{
	Serial.begin(115200);

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
	if (Serial.available() > 0)
	{
		json_str = Serial.readStringUntil('\n');
		deserializeJson(jsonDocument, json_str);

		sendPostRequest();
	}
}

void sendPostRequest()
{
	// wait for WiFi connection
	if ((WiFi.status() == WL_CONNECTED))
	{
		String api_path = "";
		String postData = "";
		WiFiClient client;
		HTTPClient http;

		if (!jsonDocument.containsKey("type"))
		{
			Serial.println("error no type data.");
			return;
		}

		if (jsonDocument["type"] == 0) // check rfid
		{
			String rfid = jsonDocument["rfid"];
			api_path = "checkrfid";
			postData = "{\"rfid\":\"" + rfid + "\"}";
		}

		// configure traged server and url
		http.begin(client, "http://" SERVER_IP "/api/v1/" + api_path); // HTTP
		http.addHeader("Content-Type", "application/json");
		http.addHeader("Authorization", STR_TOKEN);

		// start connection and send HTTP header and body
		Serial.println(postData);
		int httpCode = http.POST(postData);

		// httpCode will be negative on error
		if (httpCode > 0)
		{
			// file found at server
			if (httpCode == HTTP_CODE_OK)
			{
				jsonDocument.clear();
				const String &payload = http.getString();
				deserializeJson(jsonDocument, payload);

				String key_perm = jsonDocument["key_perm"];

				Serial.println("{\"type\":1,\"key_perm\":" + key_perm + "}");
			}
		}

		http.end();
	}
}
