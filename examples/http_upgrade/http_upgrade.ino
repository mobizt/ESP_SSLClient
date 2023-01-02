/**
 * This example shows how to upgrade from http connection to https connection.
 * 
 * This example works on the Arduino-Pico SDK from Earle F. Philhower.
 * https://github.com/earlephilhower/arduino-pico
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt/ESP_SSLSClient
 *
 * Copyright (c) 2023 mobizt
 *
 */
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else
// RP2040 (Raspberry Pi Pico W)
#include <WiFi.h>
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
  ssl_client.setClient(&basic_client, false /* set enable SSL option to false */);
}

void loop()
{
  Serial.println("---------------------------------");

  Serial.print("Connecting to server via HTTP...");

  String payload = "{\"title\":\"hello\"}";

  if (ssl_client.connect("reqres.in", 443))
  {
    Serial.println(" ok");

    Serial.print("Upgrade to HTTPS...");
    if (!ssl_client.connectSSL())
    {
      Serial.println(" failed\n");
      return;
    }

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
    unsigned long ms = millis();
    while (!ssl_client.available() && millis() - ms < 3000)
    {
      delay(0);
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
  
  Serial.println();

  delay(5000);
}