#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "IButton.h"
#include "../core/ILogger.h"
#include <Arduino.h>

class ButtonManager final : public IButton
{
public:
    explicit ButtonManager(ILogger& logger, uint8_t pin);
    ~ButtonManager() override = default;

    void begin() override;
    void loop() override;
    bool isPressed() override;

private:
    ILogger& logger;
    uint8_t pin;

    int lastState;
    int currentState;
    unsigned long lastDebounceTime;
    static constexpr unsigned long DEBOUNCE_DELAY_MS = 50;

    bool pressedFlag;
};

#endif // BUTTON_MANAGER_H
