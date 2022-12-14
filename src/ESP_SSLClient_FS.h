#ifndef ESP_SSLClient_FS_H
#define ESP_SSLClient_FS_H
#if defined(ARDUINO)
#include <Arduino.h>
#endif
#if defined(ESP8266) || defined(PICO_RP2040)
#define USE_BSSL_SSL_ENGINE
#endif

#pragma once

// for enable debugging
// #define ESP_SSLCLIENT_ENABLE_DEBUG

// for debug port
// #define ESP_SSLCLIENT_DEBUG_PORT Serial

// for any MCU that runs on the ARM mbedTLS port library
// #define USE_MBEDTLS_SSL_ENGINE

#endif