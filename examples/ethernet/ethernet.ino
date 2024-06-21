/**
 * This example shows how to connect to server using W5500, ESP32 and SSL Client.
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt/ESP_SSLSClient
 *
 * Copyright (c) 2023 mobizt
 *
 */

#include <Arduino.h>
#include <Ethernet.h>

#include <ESP_SSLClient.h>

#define WIZNET_RESET_PIN 26 // Connect W5500 Reset pin to GPIO 26 of ESP32
#define WIZNET_CS_PIN 5     // Connect W5500 CS pin to GPIO 5 of ESP32
#define WIZNET_MISO_PIN 19  // Connect W5500 MISO pin to GPIO 19 of ESP32
#define WIZNET_MOSI_PIN 23  // Connect W5500 MOSI pin to GPIO 23 of ESP32
#define WIZNET_SCLK_PIN 18  // Connect W5500 SCLK pin to GPIO 18 of ESP32

ESP_SSLClient ssl_client;

EthernetClient basic_client;

uint8_t Eth_MAC[] = {0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01};

void ResetEthernet()
{
    Serial.println("Resetting WIZnet W5500 Ethernet Board...  ");
    pinMode(WIZNET_RESET_PIN, OUTPUT);
    digitalWrite(WIZNET_RESET_PIN, HIGH);
    delay(200);
    digitalWrite(WIZNET_RESET_PIN, LOW);
    delay(50);
    digitalWrite(WIZNET_RESET_PIN, HIGH);
    delay(200);
}

void networkConnection()
{

    Ethernet.init(WIZNET_CS_PIN);

    ResetEthernet();

    Serial.println("Starting Ethernet connection...");
    Ethernet.begin(Eth_MAC);

    unsigned long to = millis();

    while (Ethernet.linkStatus() == LinkOFF || millis() - to < 2000)
    {
        delay(100);
    }

    if (Ethernet.linkStatus() == LinkON)
    {
        Serial.print("Connected with IP ");
        Serial.println(Ethernet.localIP());
    }
    else
    {
        Serial.println("Can't connect");
    }
}

void setup()
{
    Serial.begin(115200);

    networkConnection();

    // ignore server ssl certificate verification
    ssl_client.setInsecure();

    // Set the receive and transmit buffers size in bytes for memory allocation (512 to 16384).
    ssl_client.setBufferSizes(1024 /* rx */, 512 /* tx */);

    /** Call setDebugLevel(level) to set the debug
     * esp_ssl_debug_none = 0
     * esp_ssl_debug_error = 1
     * esp_ssl_debug_warn = 2
     * esp_ssl_debug_info = 3
     * esp_ssl_debug_dump = 4
     */
    ssl_client.setDebugLevel(1);

    // Assign the basic client
    // Due to the basic_client pointer is assigned, to avoid dangling pointer, basic_client should be existed
    // as long as it was used by ssl_client for transportation.
    ssl_client.setClient(&basic_client);
}

void loop()
{
    Serial.println("---------------------------------");
    Serial.print("Connecting to server...");

    String payload = "{\"title\":\"hello\"}";

    if (ssl_client.connect("reqres.in", 443))
    {
        Serial.println(" ok");
        Serial.println("Send POST request...");
        ssl_client.print("POST /api/users HTTP/1.1\r\n");
        ssl_client.print("Host: reqres.in\r\n");
        ssl_client.print("Content-Type: application/json\r\n");
        ssl_client.print("Content-Length: ");
        ssl_client.print(payload.length());
        ssl_client.print("\r\n\r\n");
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