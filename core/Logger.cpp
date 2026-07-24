#include "Logger.h"
#include "../config/AppConfig.h"

Logger::Logger()
{
}

void Logger::begin()
{
    Serial.begin(AppConfig::SERIAL_BAUD_RATE);
    info("Logger initialized.");
}

void Logger::debug(const char* message)
{
    print(Level::Debug, message);
}

void Logger::info(const char* message)
{
    print(Level::Info, message);
}

void Logger::warning(const char* message)
{
    print(Level::Warning, message);
}

void Logger::error(const char* message)
{
    print(Level::Error, message);
}

void Logger::print(
    Level level,
    const char* message)
{
    Serial.print('[');
    Serial.print(levelToString(level));
    Serial.print("] ");

    Serial.println(message);
}

const char* Logger::levelToString(
    Level level) const
{
    switch(level)
    {
        case Level::Debug:
            return "DEBUG";

        case Level::Info:
            return "INFO";

        case Level::Warning:
            return "WARNING";

        case Level::Error:
            return "ERROR";

        default:
            return "UNKNOWN";
    }
}