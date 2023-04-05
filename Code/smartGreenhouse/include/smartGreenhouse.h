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
#include <LiquidCrystal_I2C.h>
#include <Button2.h>
#include "button_debounce.h"

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
    WINDOW_0_ENDSTOP_ERROR,
    WINDOW_1_ENDSTOP_ERROR,
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
    bool relaisStatus(int num);
    config settings;
    void loop();
    void setOperationState(operation_states state);
    void moveWindow(int window, int position);
    void toggleRelais(int num, bool on);
    void initBME();
    void initLCD();

private:
    void getSensorData();
    void checkButton();
    void getWindowState();
    void getDoorState();
    void getVoltage();
    void sendMqttData();
    void controlMotor(); // controlling actuators in loop
    void writeLCD();
    void mqtt_callback(char *topic, byte *payload, unsigned int length);
    void toggleOTHERS();
    int getMotorCurrent(int motor);
    float getMotorCurrentZeroVoltage(int motor);

    // mqtt handling
    WiFiClient espClient;
    PubSubClient mqtt_client;
    void mqtt_reconnect();
    bool mqtt_trigger = false;
    unsigned int mqttLastReconnect;

    // motor
    L298N *motors;
    // ACS712 zero voltage, measured voltage if motors are not turning
    float motor_current_zeroVoltage[NUM_OF_WINDOWS];
    float motor_last_voltage[NUM_OF_WINDOWS]; // last read voltageore divider
    unsigned int time_motor_current_zero[NUM_OF_WINDOWS];
    unsigned int time_interval_motor_current = 500; // only once per 1/2 second
    unsigned int time_last_motor_current[NUM_OF_WINDOWS];

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
    Adafruit_BME280 bme;

    // LCD Display
    LiquidCrystal_I2C *lcd;
    int lcdWriteCounter = 0;
    int lcdClearCounter = 100;

    // other sensor data
    bool window_open[NUM_OF_WINDOWS];
    bool window_closed[NUM_OF_WINDOWS];
    bool door_open = false;
    bool door_closed = false;
    button_debounce *endstops_open[NUM_OF_WINDOWS];
    button_debounce *endstops_closed[NUM_OF_WINDOWS];

    // actuator states
    bool water_pump_enable = false;
    bool heater_enable = false;
    bool fan_enable = false;
    bool light_enable = false;
    bool others_enable = false;
    int window_position[NUM_OF_WINDOWS];
    int window_target_position[NUM_OF_WINDOWS];
    int window_last_target_position[NUM_OF_WINDOWS];
    unsigned int window_last_time_new_target[NUM_OF_WINDOWS];

    // actuator durations
    uint32_t time_water_pump = 0;
    uint32_t time_motor1_on = 0;
    uint32_t time_motor2_on = 0;
    uint32_t time_window_active_step = 0; // enable time to move window to next position
    // uint32_t time_window_move_start[NUM_OF_WINDOWS]; // when did we start to move a window
    uint32_t time_light = 0;

    // common times
    uint32_t mqtt_last_time = 0;
    uint32_t time_manual_state = 0;
    uint32_t time_last_sensor_read = 0;

    const char *error_names[7] = {"OK", "ERR_WINDOW1_ENDSTOP", "ERR_WINDOW2_ENDSTOP", "ERR_WINDOW_MOVE", "ERR_DOOR", "ERR_MOTOR_OVERLOAD", "ERR_BME"};
};
#endif