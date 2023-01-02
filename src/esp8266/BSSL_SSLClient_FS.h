#ifndef BSSL_SSLCLIENT_FS_H
#define BSSL_SSLCLIENT_FS_H
#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include "ESP_SSLClient_FS.h"

#pragma once

// for enable debugging
#if defined(ESP_SSLCLIENT_ENABLE_DEBUG)
#define BSSL_SSLCLIENT_ENABLE_DEBUG
#endif

// for debug port
#if defined(ESP_SSLCLIENT_DEBUG_PORT)
#define BSSL_SSLCLIENT_DEBUG_PORT ESP_SSLCLIENT_DEBUG_PORT
#endif

#endif