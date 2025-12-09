#ifndef EMBEDDED_UTILS_H
#define EMBEDDED_UTILS_H

/**
 * @file embedded_utils.h
 * @brief Defines cross-platform utility macros for embedded systems.
 * * Includes a mandatory halt/breakpoint function that minimizes code size
 * and is specific to the target architecture (AVR, ARM, or Desktop).
 */

// --- Halting/Breakpoint Macro ---

// Define the core macro for unrecoverable errors/software breakpoints
#if defined(__AVR__)
// AVR specific: Uses the assembly BREAK instruction to halt the CPU.
#define EMBEDDED_ASSERT_FAIL() asm("break")

#elif defined(__arm__) || defined(__thumb__) || defined(__CC_ARM)
// ARM Cortex-M specific (Used by ESP32, STM32, etc.):
// Uses the software breakpoint instruction.
#define EMBEDDED_ASSERT_FAIL() __asm__("bkpt #0")

#else
// Standard C/C++ platforms (Desktop, Linux, testing environment):
// Uses the standard C library function to terminate execution.
// Requires <stdlib.h> or <cstdlib>.
//  #include <stdlib.h>
#define EMBEDDED_ASSERT_FAIL() abort()
#endif

#endif // EMBEDDED_UTILS_H