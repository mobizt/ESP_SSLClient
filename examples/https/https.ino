
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_SSLClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

ESP_SSLClient ssl_client;

// EthernetClient basic_client;
// GSMClient basic_client;
WiFiClient basic_client;

void setup()
{
    Serial.begin(115200);

    // Reset the network connection
    WiFi.disconnect();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.println("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // ignore server ssl certificate verification
    ssl_client.setInsecure();

    // assign the basic client
    ssl_client.setClient(&basic_client);

    Serial.print("Connecting to server...");

    String payload = "{\"title\":\"hello\"}";

    if (ssl_client.connect("reqres.in", 443))
    {
        Serial.println(" ok");
        Serial.println("Send POST request...");
        ssl_client.print("POST /api/users HTTP/1.1\n");
        ssl_client.print("Host: reqres.in\n");
        ssl_client.print("Content-Type: application/json\n");
        ssl_client.print("Content-Length: ");
        ssl_client.print(payload.length());
        ssl_client.print("\n\n");
        ssl_client.print(payload);

        Serial.print("Read response...");

        while (!ssl_client.available())
        {
        }
        Serial.println();
        while (ssl_client.available())
        {
            Serial.print((char)ssl_client.read());
        }
        Serial.println();
    }
    else
        Serial.println(" failed\n");

    ssl_client.stop();
}

void loop()
{
    // put your main code here, to run repeatedly:
}