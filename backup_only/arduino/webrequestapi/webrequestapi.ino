#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define SERVER_IP "82.180.137.10"
#define STR_TOKEN "e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8"

#ifndef STASSID
#define STASSID "ELECTRONICS-tech"
#define STAPSK "224programming"
#endif

StaticJsonDocument<350> jsonDocument;
String json_str = "";
int uid = 0;

void setup()
{
	Serial.begin(9600);

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

		if (!jsonDocument.containsKey("type"))
		{
			jsonDocument.clear();
			return;
		}

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

		if (jsonDocument["type"] == 0) // check rfid
		{
			String rfid = jsonDocument["rfid"];
			api_path = "checkrfid";
			postData = "{\"rfid\":\"" + rfid + "\"}";
		}
		else if (jsonDocument["type"] == 2) // button was pressed, borrowed key
		{
			int what_key = jsonDocument["key"];
			api_path = "createnodelog";
			String remarks = jsonDocument["remarks"];
			postData = "{\"title\":\"The key was " + remarks + "\",\"status\":\"" + remarks + "\",\"uid\":" + String(uid) + ",\"key\":" + String(what_key) + "}";
		}
		else
		{
			return;
		}

		WiFiClient client;
		HTTPClient http;

		// configure traged server and url
		http.begin(client, "http://" SERVER_IP "/api/v1/" + api_path); // HTTP
		http.addHeader("Content-Type", "application/json");
		http.addHeader("Authorization", STR_TOKEN);

		// start connection and send HTTP header and body
		Serial.println("{\"msg\":\"Sending request\"}");
		int httpCode = http.POST(postData);
		delay(1000);

		// httpCode will be negative on error
		if (httpCode > 0)
		{
			// file found at server
			if (httpCode == HTTP_CODE_OK)
			{
				jsonDocument.clear();
				const String &payload = http.getString();
				deserializeJson(jsonDocument, payload);

				if (!jsonDocument.containsKey("k"))
				{
					Serial.println("{\"reset\":1}");
					jsonDocument.clear();

					http.end();

					return;
				}

				String k = jsonDocument["k"];
				uid = jsonDocument["uid"];

				Serial.println("{\"type\":1,\"k\":" + k + "}");
			}
		}
		else
		{
			// has error
			Serial.println("{\"error\":\"try again\"}");
		}
		jsonDocument.clear();

		http.end();
	}
}
