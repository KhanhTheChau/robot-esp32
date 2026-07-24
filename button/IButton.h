#ifndef IBUTTON_H
#define IBUTTON_H

class IButton
{
public:
    virtual ~IButton() = default;

    virtual void begin() = 0;
    virtual void loop() = 0;
    
    // Trả về true nếu nút vừa được nhấn (chỉ trả về true 1 lần cho 1 lần nhấn)
    virtual bool isPressed() = 0;
};

#endif // IBUTTON_H
