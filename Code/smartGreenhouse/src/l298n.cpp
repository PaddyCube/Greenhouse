#include "l298n.h"

L298N::L298N(int pin_enable, int pin_mot1_in1, int pin_mot1_in2, int pin_mot2_in1, int pin_mot2_in2)
{
    _pin_enable = pin_enable;
    _pin_motor_in1[0] = pin_mot1_in1;
    _pin_motor_in2[0] = pin_mot1_in2;
    _pin_motor_in1[1] = pin_mot2_in1;
    _pin_motor_in2[1] = pin_mot2_in2;
}

void L298N::setup()
{
    pinMode(_pin_motor_in1[0], OUTPUT);
    pinMode(_pin_motor_in2[0], OUTPUT);
    pinMode(_pin_motor_in1[1], OUTPUT);
    pinMode(_pin_motor_in2[1], OUTPUT);
    ledcAttachPin(_pin_enable, 0); // enables motor driver
    ledcSetup(0, 1000, 12);
}

bool L298N::stop(int motor)
{
    if (motor != MOTOR1 && motor != MOTOR2)
        return false;

    motor_enable[motor] = false;
    digitalWrite(_pin_motor_in1[motor], LOW);
    digitalWrite(_pin_motor_in2[motor], LOW);
    _motor_starttime[motor] = 0;
    return true;
}

bool L298N::setSpeed(int speed)
{
    if (speed < 0 || speed > 255)
        return false;

    _speed = speed;
    return true;
}

bool L298N::runMotor(int motor, direction direction)
{
    if (motor != MOTOR1 && motor != MOTOR2)
        return false;

    switch (direction)
    {
    case forward:
        digitalWrite(_pin_motor_in1[motor], LOW);
        digitalWrite(_pin_motor_in2[motor], HIGH);
        break;
    case backward:
        digitalWrite(_pin_motor_in1[motor], HIGH);
        digitalWrite(_pin_motor_in2[motor], LOW);
        break;
    }
    motor_enable[motor] = true;
    _motor_starttime[motor] = 0;
    ledcWrite(PWM_CH, _speed);
    return true;
}

bool L298N::runMotorFor(int motor, direction direction, int duration)
{
    if (duration < 0)
    {
        return false;
    }

    if (runMotor(motor, direction)) // start motor
    {
        _motor_starttime[motor] = millis();
        _motor_runtime[motor] = duration;
        Serial.print("run motor for ");
        Serial.println(duration);
        return true;
    }
    else
    {
        return false;
    }
}

void L298N::loop()
{
    for (int i = 0; i < 3; i++) // check for both motors
    {
        // if a starttime has been set, check if we reached it
        if (_motor_starttime[i] > 0)
        {
            if (millis() - _motor_starttime[i] > _motor_runtime[i])
            { // timeout has been reached, stop the motor
                motor_enable[i] = false;
                stop(i);
            }
        }
    }

    // disable EN pin, if no motor should turn
    if (motor_enable[MOTOR1] == false && motor_enable[MOTOR2] == false)
    {
        ledcWrite(PWM_CH, 0);
    }
};