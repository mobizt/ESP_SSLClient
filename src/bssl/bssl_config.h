#ifndef BSSL_CONFIG_H
#define BSSL_CONFIG_H


#include <Arduino.h>
// Use BearSSL engine library internal or platform (ESP8266 and RP2040)
// For non ESP8266 and RP2040 devices, define BSSL_BUILD_EXTERNAL_CORE when 
// external BearSSL libraries are used to prevent multiple functions definitions compile error.
#if !defined(BSSL_BUILD_EXTERNAL_CORE)

#if (defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)) && !defined(ARDUINO_NANO_RP2040_CONNECT)
#define BSSL_BUILD_PLATFORM_CORE
#else
#define BSSL_BUILD_INTERNAL_CORE
#endif

// workaround when these macros 
// are previousely defined.
#if defined(MAX)
#undef MAX
#endif

#if defined(MIN)
#undef MIN
#endif


#endif
#endif