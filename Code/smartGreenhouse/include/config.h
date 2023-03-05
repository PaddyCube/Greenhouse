#ifndef _CONFIG_H
#define _CONFIG_H

/* 
This file defines the configuration of your smartGreenhouse. Please go through each section and set
the values according to your given environment and usage
*/

/*---------- MQTT --------------------*/
#define mqtt_topic "/smartGreenhouse"
#define mqtt_subscribe_topic "/smartGreenhouseControl"
#define MQTT_SEND_INTERVAL  5000 // every second;
/*---------- Power settings ----------*/
#define SOLAR_POWERED true          // solar powered station? true/false
#define BATTERY_MONITOR true        // monitor and report battery voltage? true/false

/*---------- Window and vents settings ----------*/
#define NUM_OF_WINDOWS 2                        // how many windows or openings should be monitored/controlled? (max. 2)
#define WINDOW_MOVE_TIMEOUT  60000              // max time allowed to move window until error
#define NUM_OF_DOORS 1                          // how many doors should be monitored? (max 1)
#define WINDOW_ENDSTOPS_NC true                 // endstop switches of windows are normally closed? (true/false)
#define DOOR_ENDSTOP_NC false                   // endstop switch of door is normally closed? (true/false)
#define MOTOR_MAX_CURRENT 700                   // max allowed current before error state in mA
/*----- Light and Switch-----*/
#define USE_LIGHT true                              // define if your greenhouse has light

/*----------- Voltage divider settings for motor current-------*/
#define MOTOR1_R1 1.46
#define MOTOR1_R2 2.6
#define MOTOR2_R1 1.39
#define MOTOR2_R2 2.38
#define MOTOR_CURRENT_ZERO_TIMEOUT 10000  // set zero voltage of ACS712, if ue older than
/*----- other environmental controls --------*/
#define USE_HEATER true                     // do you want to control an elecrical heater(true/false)? If so, connect to relais 1
#define USE_FAN true                        // do you want to control a fan?
#define USE_WATER_PUMP true                 // is a water barrel/pump attached?
#define USE_OTHERS true                     // other output as individual output
#define COMBINE_OTHERS_MOTORS true          // switch OTHERS on, if motor is turning (e.g. for DC/DC converter)
#define COMBINE_OTHERS_LIGHT true           // switch OTHERS on, when LIGHT is on (e.g. for DC/DC converter)
#define COMBINE_OTHERS_WATER false          // switch OTHERS on, when WATER is on (e.g. for DC/DC converter)
#define COMBINE_OTHERS_HEATER false         // switch OTHERS on, when HEATER is on (e.g. for DC/DC converter)
#define COMBINE_OTHERS_FAN false            // switch OTHERS on, when FAN is on (e.g. for DC/DC converter)

/*------ operational state control ----------*/
#define TIMEOUT_MANUAL_STATE 300000         // timeout to go back to mode
#define TIMEOUT_READ_SENSOR 5000           // read new sensor data every x ms

#endif
