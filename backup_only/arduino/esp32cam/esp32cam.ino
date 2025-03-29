#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "base64.h" // Include Base64 encoding library

// Replace with your WiFi credentials
const char *ssid = "starlink_wifi101";
const char *password = "@Starlink25252525";

// Your API endpoint
#define STR_TOKEN "e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8"
const char *serverUrl = "http://192.168.1.135:8001/api/v1/createnodelog";

// ESP32-CAM (AI-THINKER) Camera Pin Configuration
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void startCamera()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // Frame Size
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;

    // Initialize the camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.println("Camera Init Failed");
        return;
    }
}

String captureAndEncodeImage()
{
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        return "";
    }

    // Convert to Base64
    String imageBase64 = base64::encode(fb->buf, fb->len);

    esp_camera_fb_return(fb); // Free memory
    return imageBase64;
}

void sendImageToServer(String imageBase64)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", STR_TOKEN);

        // Create JSON payload
        String payload = "{\"title\":\"image saved from esp32\",\"status\":\"esp32cam\",\"uid\":1,\"key\":1,\"image\":\"" + imageBase64 + "\"}";

        // Send POST request
        int httpResponseCode = http.POST(payload);
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            Serial.println("Server Response: " + response);
        }
        else
        {
            Serial.println("Error on sending POST: " + String(httpResponseCode));
        }

        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    startCamera();

    delay(2000); // Allow the camera to stabilize
    String encodedImage = captureAndEncodeImage();
    if (encodedImage.length() > 0)
    {
        sendImageToServer(encodedImage);
    }
}

void loop()
{
    // Capture and send an image every 30 seconds
    delay(30000);
    String encodedImage = captureAndEncodeImage();
    if (encodedImage.length() > 0)
    {
        sendImageToServer(encodedImage);
    }
}
