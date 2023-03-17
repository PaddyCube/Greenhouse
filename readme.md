# smartGreenhouse, an Greenhouse monitoring and control system based on ESP32
smartGreenhouse is a monitoring and control system for your Greenhouse. It uses an ESP32 DEVKIT V1 module as main MCU. 
As my greenhouse is a small one, the commonly used window lift (wax filled zylinder) didn't fit. So I build a linear actuator to move window.
Also I wanted to measure and collect environmental data.

Disclaimer: this project runs great for me. But I'm not responsible for any damage or harm of people. It comes without any warranty !!!
## Functions
With smartGreenhouse you can
- measure temperature
- measure humidity
- monitor door state (open/closed)
- monitor window states (open/closed)
- control up to 5 outputs for
	- heater
	- water
	- fan
	- light
	- other
- control motorized windows
- measure battery and solar voltage
- send data via mqtt to your broker
- send commands to greenhouse from your broker

It also provides:
- LCD display to visualize data 
- wifi configuration interface
- simple switch to control light, water and wifi hotspot
- PCB schematics and files (KICAD)

## Operational state
smartGreenhouse has two main states, which are AUTOMATIC and MANUAL. While in AUTOMATIC mode, it will take care of everything for you. When in MANUAL,
you can control outputs and window positions like you want. smartGreenhouse will enter AUTOMATIC mode again after some time (MANUAL_TIMEOUT, see below)

If any error occurs, it es to error state. This state can only , if you either reset the device (boot), of by MQTT command. Possible errors are:
- no data from BME280
- window endstop errors (open and close endstop triggered at the same time) 

If in error state, all outputs will be turned off (no heater, no fan etc.). Also windows can't be operated.

## Environmental data
To measure environmental data like temperature and humidity, a BME280 sensor gets used. It is connected to I2C port of ESP32 module

## door and window states
Depending on your needs, you can check, if doors and windows are opened or closed. These endstops can be simple reed contacts or switches. For security reasons,
please use normally-closed (NC) contacts for windows. This ensures safe operation, even if your cable is broken or has only poor contact. For door, it doesn't matter
as the door is not motorized (yet).

## Output
A four-channel relais module gets used to switch different outputs like turning on/off heater or water. For light, I use a FR120N MOSFET module, but relais will work here
as well. Thanks to relais, you can also switch high voltage like 230V for a heater. Or you use battery power, like 12V for a fan. It is up to you to hook up what you need.

ATTENTION: 230V IS DANGEROUS. ONLY USE IT, IF YOU KNOW WHAT YOU DO. DON'T BLAME ME, IF YOU KILLED YOURSELF

### Heater
Heaters can be turned on/off automatically based on temperature. You can specify  min and max temperature for heater. If actual temp is below min_temp,
heaters will be turned on. If temp raises above max_temp, heater will be turned off.

### Fan
Fan gets turned on/off based on humidity. If humidity is above a given value (see configuration), fan gets turned on. If it is below this threshold, fan gets turned off.

### Water
Water can only be controlled manually, either by double press on the button, or by sending a command via mqtt.
If water gets turned on, it will run as long as water_timeout has not been reached (see configuration). You can turn it off before, if you double press the button
again or send a command via mqtt.

### Other
This is an additional output without any special purpose. You can use it for whatever you want. It is also possible to combine this output with other meaning
this output gets toggled together with lights and motors. I need this as my battery is a 12V one and motors and light need 24V. I use a DC/DC step up converter,
which only gets enabled, if light and motor are in use.

### Light
With light, I don't mean these plant lights to grow your stuff. I mean a simple light so you'll find vegetables even at night :-)
Light gets either turned on/off by single press of button, or by mqtt command. It will be turned on as long as light_timeout didn't get reached. You can
also press he switch again or send a mqtt command, to turn off before timeout has been reached.

## Window operation
with smartGreenhouse, you can control up to two motorized windows, which gets operated simultaneously. Each window as to be equipped with upper and lower endstops.
smartGreenhouse monitors these endstops individually for each window. So if one window closes faster, it stops when reaching it's endstop while the other window continues 
to close.

A few parameters are taken into account to operate window. For details, see Configuration section below.
- min temperature: if temp is below this value, windows gets fully closed
- max temperature: whem temp is above this value, windows gets fully opened
- positions: how many stops between min and max temp
- step time: duration in seconds motor must run to reach next window positions. Needed as we don't have feedback sensors of actual positions

### Operation mode
When temp is above min_temperature, a window position gets calculated based on difference between actual temperature and min temperature.
Example:
- min_temp 18 C°
- actual_temp 20 C°
- max_temp (fully open) 24 C°
- positions: 6

Window should start to open at 18C° and should be fully opened at 24C°. Between this, 6 different stop positions are allowed. 
Window target position = actual_temp - min_temp / ( [max_temp - min_temp] / positions ) = 2

Motor will be controlled to reach position 2 (operate for 2x step_time).

Beside window position, endstops will be taken into account in any case. If one gets reached, it will always stop and reset position.

## Battery and Solar
I use a 12V lead-acid battery in conjunction with a solar charge controller to run smartGreenhouse. It is not mandatory to run it off battery with solar.
You can also operate it from a simple USB 5V mobile phone charger, if you want.
If you use battery and solar, you can measure battery voltage as well as solar voltage.

## send data to MQTT broker
smartGreenhouse sends every x seconds a message to your mqtt broker. It is up to you to use or not use these data. I use node-red to visualize.

## receive commands from MQTT broker
you can send commands to turn on/off any output and also to operate windows manually. Before that, you need to change operation state from AUTO to MANUAL,
also by command. For details, see Command section below.

## LCD Display
I use a 2004 LCD display attached by I2C to visualize enviromental and operational data. This way, you can check data when you're inside the greenhouse
but without using your phone.

## Wifi configuration interface
during boot and after a long-press of the button, ESP32 will create a wifi access point. By connecting to this with your phone or PC, you can set up wifi connection
to your home wifi as well as set different parameters. 

- first connect to the WIFI AP of smartGreenhouse. If none gets shown, either restart or long-press button.
- after connected, enter the following address in your browser: http://192.168.4.1 to open configuration wizard.

## PCB Schematics
Beside the code for your ESP32 DevKit V1, this repository also comes with a KiCAD project for a PCB board.

# Your own build
## BOM
- ESP32 DevKit V1 (36 pin version)
- BME280
- 4-channel relais board
- FR120N Mosfet Module or 1-channel relais board
- L298N motor controller
- 2004 LCD display with I2C interface
- push button 
- some resistors, contact terminals, jumpers

## Configuration
### CONFIG.H
Have a look at CONFIG.H, which defines some fixed values. Most of the parameters should be self-explanatory. Here you find MQTT send interval and MQTT topic names.
Also you define how many windows you want to control, if dorr should be monitored and so on. CHECK THIS FILE

### WIFI configuration
After connect to smartGreenhouse's WIFI AP, you can set the following parameters:
- MQTT Server: address of your mqtt broker
- MQTT port: port number of your mqtt broker
- wait, what about MQTT security? Sorry, not implemented so far

- Num of window positions: number of stops between fully closed and fully opened window
- close window if temp below C°: if temp is lower, window will be closed (and keep closed)
- fully open window if temp above C°: if temp is above this value, windows will be opened to its maximum.
- time(s) position: how many seconds must the motor run, to reach next position

- turn heater on, if temp below C°
- turn heater off, if temp above C°

- turn fan on, if humidity is above %

- run water pump for (s): when water pump gets enabled, how long should it keeped on before turning water pump off?
- Timeout for light (s): when light has been turned on, turn it off after this time.

## MQTT commands
### toggle auto / manual state
To switch to AUTOMATIC mode, use this command
'''
{"command":"setMode","parameter":"AUTO"}
'''

To switch to MANUAL state, use this command
'''
{"command":"setMode","parameter":"MANUAL"}
'''

### reset error state


### move window
To move the windows to a given position, use this command. Please note, it only works, if you switch operation mode to MANUAL_TIMEOUT
'''
{"command":"WINDOW","parameter":"<your position>"}
'''

### toggle outputs
To toggle any output, use one of the commands below

'''
{"command":"RELAIS_ON","parameter":"HEATER"}
{"command":"RELAIS_OFF","parameter":"HEATER"}
{"command":"RELAIS_ON","parameter":"FAN"}
{"command":"RELAIS_OFF","parameter":"FAN"}
{"command":"RELAIS_ON","parameter":"WATER"}
{"command":"RELAIS_OFF","parameter":"WATER"}
{"command":"RELAIS_ON","parameter":"OTHERS"}
{"command":"RELAIS_OFF","parameter":"OTHERS"}
{"command":"RELAIS_ON","parameter":"LIGHT"}
{"command":"RELAIS_OFF","parameter":"LIGHT"}
'''

# My build
## Pictures of PCB and assembled smartGreenhouse
![PCB front](/pictures/PCB_front.jpg)
![PCB back](/pictures/PCB_back.jpg)