#ifndef _BUTTON_DEBOUNCE_H
#define _BUTTON_DEBOUNCE_H

#include <Arduino.h>

class button_debounce
{
public:
    button_debounce(int pin, bool nc);
    bool get_state();
    void loop();

private:
    bool _actual_state; // returning button value
    bool _last_flicker_state;
    uint32_t _last_debounce;
    int _button_pin; // pin number of button
    bool _nc;        // normally closed button
};

#endif