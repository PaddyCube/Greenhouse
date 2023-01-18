#ifndef _L298N_H
#define _L298N_H

#define MOTOR1 0
#define MOTOR2 1
#define PWM_CH 0

#include <Arduino.h>

enum direction
{
    forward,
    backward
};

class L298N
{
public:
    L298N(int pin_enable, int pin_mot1_in1, int pin_mot1_in2, int pin_mot2_in1, int pin_mot2_in2);

    bool stop(int motor);
    bool setSpeed(int speed);
    bool runMotor(int motor, direction direction);
    bool runMotorFor(int motor, direction direction, int duration); // uint16_t duration);
    void loop();
    bool motor_enable[2];

private:
    void setup();
    int _pin_enable;
    int _pin_motor_in1[2];
    int _pin_motor_in2[2];
    int _speed;

    // uint16_t motor1_runtime;
    int _motor_runtime[2];
    int _motor_starttime[2];
    // uint16_t motor2_runtime;
};


#endif