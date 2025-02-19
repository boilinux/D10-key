#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SERVER_IP "192.168.1.100:8001"

#ifndef STASSID
#define STASSID "Kainoa"
#define STAPSK "@starX@2024"
#endif

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

		String str = Serial.readStringUntil('\n');

		if (str != "send")
		{
			return;
		}

		sendPostRequest();
	}
	delay(10000);
}

void sendPostRequest()
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
		String postData = "{\"title\":\"Title from esp32\", \"status\":\"returned\"}";
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