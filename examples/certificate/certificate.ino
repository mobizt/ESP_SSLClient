/**
 * This example shows how to connect to server via https and verify the root certificate using the SSL client.
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

// Baltimore CyberTrust Root
// Expired on Tue May 13 2025
const char rootCA[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                              "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
                              "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
                              "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
                              "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
                              "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
                              "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
                              "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
                              "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
                              "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
                              "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
                              "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
                              "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
                              "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
                              "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
                              "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
                              "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
                              "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
                              "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
                              "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
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