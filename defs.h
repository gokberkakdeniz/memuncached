#ifndef H_COMPILER
#define H_COMPILER

#include <stdbool.h>
#include <string.h>

/**
 * @brief is server loop running?
 */
extern volatile bool is_running;

#define LISTEN_BACKLOG 5
#define MEMUNCACHED_VERSION "1.0"
#define STRINGIFY(A) #A

#define MEMUNCACHED_HEADER "Memuncached v" MEMUNCACHED_VERSION " (" MEMUNCACHED_COMPILER " on " MEMUNCACHED_OS ")"

#if defined(__clang__)
#define MEMUNCACHED_COMPILER "clang"
#elif defined(__INTEL_COMPILER)
#define MEMUNCACHED_COMPILER "icc"
#elif defined(__GNUC__) || defined(__GNUG__)
#define MEMUNCACHED_COMPILER "gcc"
#elif defined(_MSC_VER)
#define MEMUNCACHED_COMPILER "msvc"
#else
#define MEMUNCACHED_COMPILER "unknown"
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define MEMUNCACHED_OS "windows"
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define MEMUNCACHED_OS "linux"
#elif defined(__APPLE__) || defined(__MACH__)
#define MEMUNCACHED_OS "macos"
#elif defined(__FreeBSD__)
#define MEMUNCACHED_OS "freebsd"
#elif defined(__unix__) || defined(__unix) || defined(unix)
#define MEMUNCACHED_OS "unix"
#else
#define MEMUNCACHED_OS "unknown"
#endif

#endif