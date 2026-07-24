#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

#include "ILogger.h"

class Logger final : public ILogger
{
public:

    Logger();

    void begin() override;

    void debug(const char* message) override;

    void info(const char* message) override;

    void warning(const char* message) override;

    void error(const char* message) override;

private:

    enum class Level
    {
        Debug,
        Info,
        Warning,
        Error
    };

    void print(
        Level level,
        const char* message
    );

    const char* levelToString(
        Level level
    ) const;
};

#endif