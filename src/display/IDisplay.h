#ifndef IDISPLAY_H
#define IDISPLAY_H

#include <Arduino.h>

class IDisplay
{
public:
    virtual ~IDisplay() = default;

    virtual void begin() = 0;
    virtual void clear() = 0;
    virtual void printText(const char* text, int x = 0, int y = 0) = 0;
    virtual void showStatus(const char* status) = 0;
    virtual void drawFace(const String& emotion) = 0;
    virtual void playGifFrame() = 0;
    virtual void update() = 0;
};

#endif // IDISPLAY_H
