/**
 * This example shows how to connect to MQTT server via secure port using the SSL client.
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

// https://github.com/arduino-libraries/ArduinoMqttClient
#include <ArduinoMqttClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

ESP_SSLClient ssl_client;

// EthernetClient basic_client;
// GSMClient basic_client;
WiFiClient basic_client;

MqttClient mqttClient(ssl_client);

unsigned long lastMillis = 0;

int count = 0;

const char broker[] = "test.mosquitto.org";
int port = 8883;
const char topic[] = "arduino/simple";

const long interval = 3000;
unsigned long previousMillis = 0;
bool mqttReady = false;

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

  Serial.print("Attempting to connect to the MQTT broker over ssl: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    return;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();
  mqttReady = true;
}

void loop()
{

  if (!mqttReady)
    return;

  int messageSize = mqttClient.parseMessage();
  if (messageSize)
  {
    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // use the Stream interface to print the contents
    while (mqttClient.available())
    {
      Serial.print((char)mqttClient.read());
    }
    Serial.println();

    Serial.println();
  }

  mqttClient.poll();

  if (millis() - lastMillis > interval)
  {
    lastMillis = millis();

    Serial.print("Sending message to topic: ");

    Serial.println(topic);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);

    mqttClient.print(count);

    mqttClient.endMessage();
    count++;
  }
}