#ifndef _DEFAULTS_H
#define _DEFAULTS_H

struct config
{
    /*---------- Network settings ----------*/
    char mqtt_server[40]; // IP address of your MQTT broker
    int mqtt_port = 1883;
    char mqtt_api_token[34];

    /*---------- Window and vents settings ----------*/
    int max_window_positions = 6; // how many steps/stops should be used between fully closed and fully openend window?
    int window_min_temp = 18;     // if temperature is below this value, windows will be closed in any case (°C)
    int window_max_temp = 26;     // if temperature is above this value, windows will be opnened fully (°C)
    int window_step_time = 1000; // run time to reach next window position

    /*----- Light and Switch-----*/
    int max_light_on = 300; // define timeout (seconds) of lights. Will be turned off after reached

    /*----- other environmental controls --------*/
    int heater_min_temp = 5;      // if temp is below this value, turn on heater
    int heater_max_temp = 10;     // if temp is above this value, turn off heater
    int fan_min_humidity = 60;    // if humidity is above this value, start the fan
    int water_pump_timeout = 600; // let pump run for 10 minutes, before turning off. Pump gets triggered manually

    /*----- motor limits --------*/
    float motor_max_current = 0.7;
};

#endif