/**
 * This example shows how to connect to MQTT server via secure port using the SSL client.
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
#if defined(ESP32) || defined(PICO_RP2040)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
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

  ssl_client.setBufferSizes(1024, 512);

  /** Call setDebugLevel(level) to set the debug
   * esp_ssl_debug_none = 0
   * esp_ssl_debug_error = 1
   * esp_ssl_debug_warn = 2
   * esp_ssl_debug_info = 3
   * esp_ssl_debug_dump = 4
   */
  ssl_client.setDebugLevel(1);

  // assign the basic client
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