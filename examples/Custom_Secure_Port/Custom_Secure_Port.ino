/**
 * This example shows how to connect to server via custom secure port.
 *
 * The standard secure ports as listed here are supported by this library https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers.
 *
 * If the port you want to connect with SSL/TLS is not in the list, you can begin the connection in the plain text mode first and upgrade later.
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

// When use in insecure mode (no certificate and fingerprint verification).
// This can save memory and program space usage.
// #define SSLCLIENT_INSECURE_ONLY

// This standard HTTP/1.1 request-response pattern is strictly sequential,
// making it ideally suited for the RAM-saving Half-Duplex mode.
#define SSLCLIENT_HALF_DUPLEX

// When pre-memory allocation are prefered (stack memory used).
// Don't define when dynamic memory allocation is prefered (heap or PSRAM memory used).
// #define STATIC_IN_BUFFER_SIZE 2048
// #define STATIC_OUT_BUFFER_SIZE 1024
// #define STATIC_X509_CONTEXT
// #define STATIC_SSLCLIENT_CONTEXT

// For using external BearSSL library.
// When othere libraries that contain BearSSL source files are used
// with this library, define this macro to exclude the internal BearSSL library
// to be compiled thats makes compilation error.
// #define BSSL_BUILD_EXTERNAL_CORE

// If board supports the filesystem APIs, to use CertStore class.
// #define ENABLE_FS

#include <ESP_SSLClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

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

    // In case ESP32 WiFiClient, the session timeout should be set,
    // if the TCP session was kept alive because it was unable to detect the server disconnection.
#if defined(ESP32)
    ssl_client.setSessionTimeout(120); // Set the timeout in seconds (>=120 seconds)
#endif

    // Assign the basic client to use in non-secure mode.
    ssl_client.setClient(&basic_client, false /* set enable SSL option to false */);
}

void loop()
{
    Serial.println("---------------------------------");

    Serial.print("Connecting to server via HTTP...");

    String payload = "{\"title\":\"hello\"}";

    String server = "your_server.com"; // The server to connect.
    uint16_t port = 5443;              // The port to connect.

    if (ssl_client.connect(server.c_str(), port))
    {
        Serial.println(" ok");

        Serial.print("Upgrade to HTTPS...");
        if (!ssl_client.connectSSL())
        {
            Serial.println(" failed\r\n");
            return;
        }

        Serial.println(" ok");

        Serial.println("Send GET request...");
        ssl_client.print("GET / HTTP/1.1\r\n");
        ssl_client.print("Host: ");
        ssl_client.print(server);
        ssl_client.print("\r\n");
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