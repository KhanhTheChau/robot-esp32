#include "ButtonManager.h"

ButtonManager::ButtonManager(ILogger& logger, uint8_t pin)
    : logger(logger), pin(pin), lastState(HIGH), currentState(HIGH),
      lastDebounceTime(0), pressedFlag(false)
{
}

void ButtonManager::begin()
{
    logger.info("ButtonManager::begin()");
    pinMode(pin, INPUT_PULLUP);
    lastState = digitalRead(pin);
    currentState = lastState;
}

void ButtonManager::loop()
{
    int reading = digitalRead(pin);

    if (reading != lastState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS)
    {
        if (reading != currentState)
        {
            currentState = reading;

            // LOW means pressed for INPUT_PULLUP
            if (currentState == LOW)
            {
                logger.debug("Button pressed!");
                pressedFlag = true;
            }
        }
    }

    lastState = reading;
}

bool ButtonManager::isPressed()
{
    if (pressedFlag)
    {
        pressedFlag = false;
        return true;
    }
    return false;
}
