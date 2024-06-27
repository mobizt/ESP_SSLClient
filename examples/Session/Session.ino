/**
 * This example shows how to connect to server via https using the SSL client and store the session to speed up handshake.
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt/ESP_SSLSClient
 *
 * Copyright (c) 2023 mobizt
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

#include <ESP_SSLClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

ESP_SSLClient ssl_client;

// EthernetClient basic_client;
// GSMClient basic_client;
WiFiClient basic_client;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

BearSSL_Session session1;

BearSSL_Session session2;

const char *host1 = "reqres.in";

const char *uri1 = "/api/users";

const char *contentType1 = "application/json";

String payload1 = "{\"title\":\"hello\"}";

const char *host2 = "jsonplaceholder.typicode.com";

const char *uri2 = "/posts";

const char *contentType2 = "application/json";

String payload2 = "{\"title\":\"foo\",\"body\": \"bar\",\"userId\": 1}";

void connect(const char *method, BearSSL_Session *session, uint32_t timeout, bool closeSession, const char *host, uint16_t port, const char *uri, const char *contentType, const char *payload, bool showResponse)
{

    if (session)
    {
        if (session->getSession()->cipher_suite > 0)
            Serial.println("### CONNECT WITH SESSION");
        else
            Serial.println("### CONNECT WITH EMPTY SESSION");
    }
    else
        Serial.println("### CONNECT WITHOUT SESSION");

    unsigned long ms = millis();

    // Validation for currently connecting host and port.
    // Close connection when it's not match that of the last connection.
    ssl_client.validate(host, port);

    if (!ssl_client.connected())
    {
        Serial.print("Connecting to ");
        Serial.print(host);
        Serial.print("... ");

        ssl_client.setSession(session);

        if (!ssl_client.connect(host, port))
        {
            Serial.println(" failed\n");
            Serial.println();
            return;
        }
        Serial.println(" ok");
        Serial.print("Connect and negotiate time... ");
        Serial.println(millis() - ms);
    }

    Serial.print("Send ");
    Serial.print(method);
    Serial.println(" request...");

    ssl_client.print(method);
    ssl_client.print(" ");
    if (uri[0] != '/')
        ssl_client.print("/");
    ssl_client.print(uri);
    ssl_client.print(" HTTP/1.1\r\n");
    ssl_client.print("Host: ");
    ssl_client.print(host);
    ssl_client.print("\r\n");

    if (strcasecmp(method, "post") == 0 || strcasecmp(method, "put") == 0)
    {
        ssl_client.print("Content-Type: ");
        ssl_client.print(contentType);
        ssl_client.print("\r\n");
        ssl_client.print("Content-Length: ");
        ssl_client.print(strlen(payload));
        ssl_client.print("\r\n\r\n");
        ssl_client.print(payload);
    }

    Serial.print("Read response...");
    ms = millis();
    while (!ssl_client.available() && millis() - ms < timeout)
    {
        delay(0);
    }

    if (millis() - ms > timeout)
    {
        Serial.println(" timed out");
    }
    else
    {
        if (showResponse)
            Serial.println();

        while (ssl_client.available())
        {
            char res = ssl_client.read();
            if (showResponse)
                Serial.print(res);
        }

        if (!showResponse)
            Serial.println("ok");

        Serial.println();
    }

    if (closeSession)
        ssl_client.stop();

    Serial.println();
}

void setup()
{
    Serial.begin(115200);

    ssl_client.setSession(&session1);

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

    // Assign the basic client
    // Due to the basic_client pointer is assigned, to avoid dangling pointer, basic_client should be existed
    // as long as it was used by ssl_client for transportation.
    ssl_client.setClient(&basic_client);
}

void loop()
{
    connect("POST", nullptr, 3000, true, host1, 443, uri1, contentType1, payload1.c_str(), false);
    delay(5000);
    connect("POST", nullptr, 3000, true, host2, 443, uri2, contentType2, payload2.c_str(), false);
    delay(5000);

    connect("POST", &session1, 3000, true, host1, 443, uri1, contentType1, payload1.c_str(), false);
    delay(5000);
    connect("POST", &session2, 3000, true, host2, 443, uri2, contentType2, payload2.c_str(), false);
    delay(5000);
}