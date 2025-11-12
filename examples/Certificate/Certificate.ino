/**
 * This example shows how to connect to server via https and verify the root certificate using the SSL client.
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt/ESP_SSLSClient
 *
 * Copyright (c) 2025 mobizt
 *
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_GIGA)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>) || defined(ARDUINO_NANO_RP2040_CONNECT)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>) || defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#elif __has_include(<WiFiC3.h>) || defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif __has_include(<WiFi.h>)
#include <WiFi.h>
#endif

#define ENABLE_DEBUG        // To enable debugging
#define ENABLE_ERROR_STRING // To show details in error
#define DEBUG_PORT Serial   // To define the serial port for debug printing
#include <ESP_SSLClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// GTS Root R4
// Expired on 6/22/2036
const char rootCA[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                              "MIICCjCCAZGgAwIBAgIQbkepyIuUtui7OyrYorLBmTAKBggqhkjOPQQDAzBHMQsw\n"
                              "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
                              "MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"
                              "MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
                              "Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQA\n"
                              "IgNiAATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzu\n"
                              "hXyiQHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/l\n"
                              "xKvRHYqjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MB0GA1Ud\n"
                              "DgQWBBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNnADBkAjBqUFJ0\n"
                              "CMRw3J5QdCHojXohw0+WbhXRIjVhLfoIN+4Zba3bssx9BzT1YBkstTTZbyACMANx\n"
                              "sbqjYAuG7ZoIapVon+Kz4ZNkfF6Tpt95LY2F45TPI11xzPKwTdb+mciUqXWi4w==\n"
                              "-----END CERTIFICATE-----\n";

ESP_SSLClient ssl_client;

// EthernetClient basic_client;
// GSMClient basic_client;
WiFiClient basic_client;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

void setup()
{
    Serial.begin(115200);

    Serial.print("ESP_Client version ");
    Serial.println(ESP_SSLCLIENT_VERSION);

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    multi.addAP(WIFI_SSID, WIFI_PASSWORD);
    multi.run();
#else
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
        if (millis() - ms > 10000)
            break;
#endif
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

// The valid time is required for server certificate verification.
#if defined(ESP8266) || defined(ESP32) && !defined(ARDUINO_NANO_RP2040_CONNECT)

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    while (time(nullptr) < ESP_SSLCLIENT_VALID_TIMESTAMP)
    {
        delay(100);
    }

    // If verification time was not set via this function, the device system time will be used
    // ssl_client.setX509Time(time(nullptr));

#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)

    configTime(10000, 0, "pool.ntp.org", "time.nist.gov");
    while (time(nullptr) < ESP_SSLCLIENT_VALID_TIMESTAMP)
    {
        delay(100);
    }

    // If verification time was not set via this function, the device system time will be used
    // ssl_client.setX509Time(time(nullptr));

#elif __has_include(<WiFiNINA.h>) || __has_include(<WiFi101.h>)
    time_t ts = 0;
    do
    {
        ts = WiFi.getTime();
        delay(100);
    } while (ts < ESP_SSLCLIENT_VALID_TIMESTAMP);

    // The verification time setting is required because the device system time i.e. time(nullptr) is not available in this case.
    ssl_client.setX509Time(ts);
#endif

    // Set the server certificate, intermediate cerificate or root certificate
    // In case connection problem, the certificate may be revoked.
    // You have to provide the current certificate.
    ssl_client.setCACert(rootCA);

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
        ssl_client.print("x-api-key: reqres-free-v1\r\n");
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