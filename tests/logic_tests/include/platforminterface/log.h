#ifndef MOCK_LOG_H
#define MOCK_LOG_H

#include <cstdio>
#include <cstdarg>

namespace Qul {
    namespace PlatformInterface {
        
        // Función mock que acepta formato tipo printf (variadic arguments)
        // Puedes descomentar el vprintf si quieres ver los logs en la consola del test.
        static inline void log(const char* format, ...) {
            /*
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            */
        }
    }
}

#endif // MOCK_LOG_H