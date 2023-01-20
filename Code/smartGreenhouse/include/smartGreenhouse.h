#ifndef _SMARTGREENHOUSE_H
#define _SMARTGREENHOUSE_H

#include "defaults.h"
#include "config.h"
#include "pinmapping.h"
#include "PubSubClient.h"
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include "pinmapping.h"
#include "l298n.h"

enum relais
{
    HEATER,
    FAN,
    WATER,
    OTHERS,
    LIGHT
};

enum error_states
{
    NO_ERROR,
    WINDOW_ENDSTOP_ERROR,
    WINDOW_MOVE_ERROR,
    DOOR_ERROR,
    MOTOR_OVERLOAD,
    BME_ERROR
};

enum operation_states
{
    AUTO,
    MANUAL
};

class SmartGreenhouse
{
public:
    SmartGreenhouse();
    bool isDoorOpen();
    bool isWindowOpen(int window);
    config settings;
    void loop();
    void setOperationState(operation_states state);
    void moveWindow(int window, int position);
    void toggleRelais(int num, bool on);

private:
    void getSensorData();
    void checkButton();
    void getWindowState();
    void getDoorState();
    void getVoltage();
    void sendMqttData();
    void controlMotor(); // controlling actuators in loop
    void mqtt_callback(char *topic, byte *payload, unsigned int length);

    // mqtt handling
    WiFiClient espClient;
    PubSubClient mqtt_client;
    void mqtt_reconnect();

    // motor
    L298N *motors;
    // error states
    int error_state = 0;
    // operation state
    int operation_state = 0;

    // actual measurement data
    // power
    float batVoltage = 0.0;
    float solarVoltage = 0.0;

    // environmental sensor data
    float temperature = 0.0;
    float humidity = 0.0;

    // other sensor data
    bool window_open[NUM_OF_WINDOWS];
    bool window_closed[NUM_OF_WINDOWS];
    bool door_open = false;
    bool door_closed = false;

    // actuator states
    bool water_pump_enable = false;
    bool heater_enable = false;
    bool fan_enable = false;
    bool light_enable = false;
    bool others_enable = false;
    int window_position[NUM_OF_WINDOWS];
    int window_target_position[NUM_OF_WINDOWS];
    int window_last_target_position[NUM_OF_WINDOWS];

    // actuator durations
    uint32_t time_water_pump = 0;
    uint32_t time_motor1_on = 0;
    uint32_t time_motor2_on = 0;
    uint32_t time_window_active_step = 0; // enable time to move window to next position
    // uint32_t time_window_move_start[NUM_OF_WINDOWS]; // when did we start to move a window
    uint32_t time_light = 0;

    // common times
    uint32_t mqtt_last_time = 0;
    uint32_t mqtt_send_interval = MQTT_SEND_INTERVAL;
    uint32_t time_manual_state = 0;

    const char *error_names[6] = {"OK", "ERR_WINDOW_ENDSTOP", "ERR_WINDOW_MOVE", "ERR_DOOR", "ERR_MOTOR_OVERLOAD", "ERR_BME"};
};
#endif