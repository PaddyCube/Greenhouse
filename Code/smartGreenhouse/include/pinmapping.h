#ifndef _PINMAPPING_H
#define _PINMAPPPING_H
/*
This file defines the pins.
DON'T CHANGE THEM IF YOU DON'T KNOW WHAT YOU DO

*/
// limit switches of windows and doors
#define pin_window1_closed 34
#define pin_window1_open 35
#define pin_window2_closed 36
#define pin_window2_open 39
#define pin_door_open 16
#define pin_door_closed 17

// motor driver control pins
#define pin_motor1_in1 25
#define pin_motor1_in2 26
#define pin_motor2_in1 27
#define pin_motor2_in2 33
#define pin_motor_enable 32 // enables motor driver
#define pin_motor1_current 04 // motor 1 current sense
#define pin_motor2_current 13 // motor 2 current sense

// voltage measurement
#define pin_vcc_solar 15
#define pin_vcc_battery 2	
			
// light and switch            
#define pin_light_switch 12 // switch for user input
#define pin_light_load 14 // relais to control light

// other loads like fans, water pumps, heaters	
#define pin_relais1 5 // heater
#define pin_relais2 18 // fan
#define pin_relais3 19 // water pump
#define pin_relais4	23


#endif