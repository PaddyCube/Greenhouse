#include "button_debounce.h"

button_debounce::button_debounce(int pin, bool nc)
{
    _nc = nc;
    _button_pin = pin;
    _last_flicker_state = false;
    _actual_state = false;
    _last_debounce = 0;
}

void button_debounce::loop()
{
    bool currentState;
    if (_nc)
    {
        currentState = digitalRead(_button_pin);
    }
    else
    {
        currentState = !digitalRead(_button_pin);
    }

    // pin has been changed
    if (currentState != _last_flicker_state)
    {
        _last_debounce = millis();
        _last_flicker_state = currentState;
    }

    // Timeout reached, debounce complete as it is steady since 50ms
    if( (millis() - _last_debounce ) > 50 )
    {
        _actual_state = currentState;
    }
}

bool button_debounce::get_state()
{
    return _actual_state;
}
