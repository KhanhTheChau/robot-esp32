#ifndef ILOGGER_H
#define ILOGGER_H

#include <Arduino.h>

class ILogger
{
public:

    virtual ~ILogger() = default;

    virtual void begin() = 0;

    virtual void debug(const char* message) = 0;

    virtual void info(const char* message) = 0;

    virtual void warning(const char* message) = 0;

    virtual void error(const char* message) = 0;
};

#endif