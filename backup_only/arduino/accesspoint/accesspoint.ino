#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>

#ifndef APSSID
#define APSSID "ESP32_ACCESSPOINT"
#define APPSK "12345678"
#endif

IPAddress ip(192, 168, 1, 1); // IP address for the ESP32 AP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
String json_str = "";

StaticJsonDocument<250> jsonDocument;
char buffer[250];

ESP8266WebServer server(80);

void setup()
{
	// Configure as access point
	WiFi.softAPConfig(ip, gateway, subnet);

	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.on("/postdata", HTTP_POST, handlePost);

	server.begin();
	Serial.println("HTTP server started");
}

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
 */
void handleRoot()
{
	long time_millis = millis();
	server.send(200, "text/html", "esp32 server is working!");
}

void handlePost()
{
	server.send(200, "text/plain", "Create node successfully");
}

void sendPostRequest()
{
	WiFiClient client;
	HTTPClient http;

	const char *serverUrl = "http://191.168.1.100:8001/api/v1/createnodelog";
	String postData = "{\"title\":\"Title from esp32\", \"status\":\"returned\"}";

	http.begin(client, serverUrl); // HTTP
	http.addHeader("Content-Type", "application/json");

	Serial.print("[HTTP] POST...\n");
	// start connection and send HTTP header and body
	int httpCode = http.POST(postData);

	// httpCode will be negative on error
	if (httpCode > -1)
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

	http.end(); // Close the connection
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

	server.handleClient();
}
