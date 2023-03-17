# smartGreenhouse, an Greenhouse monitoring and control system based on ESP32
smartGreenhouse is a monitoring and control system for your Greenhouse. It uses an ESP32 DEVKIT V1 module as main MCU. 
As my greenhouse is a small one, the commonly used window lift (wax filled zynilder) didn't fit. So I build a linear actuator to move window.
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

## Environmental data
To measure environmental data like temperature and humidity, a BMP280 sensor gets used. It is connected to I2C port of ESP32 module

## door and window states
Depending on your needs, you can check, if doors and windows are opened or closed. These endstops can be simple reed contacts or switches. For security reasons,
please use normally-closed (NC) contacts for windows. This ensures safe operation, even if your cable is broken or has only poor contact. For door, it doesn't matter
as the door is not motorized (yet).

## Output
A four-channel relais module gets used to switch different outputs like turning on/off heater or water. For light, I use a FR120N MOSFET module, but relais will work here
as well. Thanks to relais, you can also switch high voltage like 230V for a heater. You can also use only battery power, like 12V for a fan.

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
this OTHER output gets toggled with lights and motors. I need this as my battery is a 12V one and motors and light need 24V. I use a DC/DC step up converter,
which only gets enabled, if light and motor are in use.

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
I use a 12V lead-acid battery in conjunction with a solar charge controller to run smartGreenhouse. It is not mandatory to run ot off battery with solar.
You can also operate it from a simple USB 5V mobile phone charger, if you want.
If you use battery and solar, you can measure battery voltage and solar voltage.

## send data to MQTT broker
smartGreenhouse sends every x seconds a message to your mqtt broker. It is up to you to use or not use these data. I use node-red to visualize.

## receive commands from MQTT broker
you can send commands to turn on/off any output and also to operate windows manually. Before that, you need to change operation state from AUTO to MANUAL,
also by command. For details, see Command section below.

## LCD Display
I use a 2004 LCD display attached by I2C to visualize enviromental and operational data. This way, you can check data when you're inside the greenhouse
but without using your phone.

## Wifi configuration interface
during boot and after a long-press of the button, ESP32 will create a wifi access point. By connecting to this with your plhone or PC, you can set up wifi connection
to your home wifi as well as set different parameters. 

- first connect to the WIFI AP of smartGreenhouse. If none gets shown, either restart or long-press button.
- after connected, enter the following address in your browser: http://192.168.4.1 to open configuration wizard.

## PCB Schematics
Beside the code for your ESP32 DevKit V1, this repository also comes with a KiCAD project for a PCB board.

