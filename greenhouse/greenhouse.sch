EESchema Schematic File Version 4
LIBS:greenhouse-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L doit-esp32-devkit-v1:DOIT-ESP32-DEVKIT-V1 ESP32_DevKitV1
U 1 1 63B0334F
P 4250 3200
F 0 "ESP32_DevKitV1" H 4250 4781 50  0000 C CNN
F 1 "ESP32-WROOM-32" H 4250 4690 50  0000 C CNN
F 2 "greenhouse:ESP32-DOIT-DEVKIT" H 4250 1700 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf" H 3950 3250 50  0001 C CNN
	1    4250 3200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x05 I2C1
U 1 1 63B0867C
P 6450 2000
F 0 "I2C1" H 6368 1575 50  0000 C CNN
F 1 "I2C-BME" H 6368 1666 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B5B-XH-A_1x05_P2.50mm_Vertical" H 6450 2000 50  0001 C CNN
F 3 "~" H 6450 2000 50  0001 C CNN
	1    6450 2000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x07 Relais1
U 1 1 63B18D95
P 7250 3000
F 0 "Relais1" H 7330 2992 50  0000 L CNN
F 1 "Relaisboard" H 7330 2901 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B7B-XH-A_1x07_P2.50mm_Vertical" H 7250 3000 50  0001 C CNN
F 3 "~" H 7250 3000 50  0001 C CNN
	1    7250 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x04 Door1
U 1 1 63B14646
P 7250 3600
F 0 "Door1" H 7330 3592 50  0000 L CNN
F 1 "DoorSwitch" H 7330 3501 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 7250 3600 50  0001 C CNN
F 3 "~" H 7250 3600 50  0001 C CNN
	1    7250 3600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x08 Motor1
U 1 1 63B0C67F
P 1200 3400
F 0 "Motor1" H 1280 3392 50  0000 L CNN
F 1 "LM298N" H 1280 3301 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B8B-XH-A_1x08_P2.50mm_Vertical" H 1200 3400 50  0001 C CNN
F 3 "~" H 1200 3400 50  0001 C CNN
	1    1200 3400
	-1   0    0    1   
$EndComp
$Comp
L Connector:Screw_Terminal_01x08 Window1
U 1 1 63B045B8
P 1200 2400
F 0 "Window1" H 1280 2392 50  0000 L CNN
F 1 "Windows" H 1300 2300 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B8B-XH-A_1x08_P2.50mm_Vertical" H 1200 2400 50  0001 C CNN
F 3 "~" H 1200 2400 50  0001 C CNN
	1    1200 2400
	-1   0    0    1   
$EndComp
Text GLabel 3300 3000 0    50   Input ~ 0
Window1_open
Text GLabel 3300 3300 0    50   Input ~ 0
Window2_closed
Text GLabel 3300 3100 0    50   Input ~ 0
Window1_closed
Text GLabel 3300 3200 0    50   Input ~ 0
Window2_open
Text GLabel 1400 2600 2    50   Input ~ 0
Window2_closed
Text GLabel 1400 2400 2    50   Input ~ 0
Window2_open
Text GLabel 1400 2200 2    50   Input ~ 0
Window1_closed
Text GLabel 1400 2000 2    50   Input ~ 0
Window1_open
Text GLabel 1400 2300 2    50   Input ~ 0
GND
Text GLabel 1400 2100 2    50   Input ~ 0
GND
Text GLabel 1400 2500 2    50   Input ~ 0
GND
Text GLabel 1400 2700 2    50   Input ~ 0
GND
Text GLabel 3300 3500 0    50   Input ~ 0
Motor1_DIR
Text GLabel 3300 3600 0    50   Input ~ 0
Motor1_PWM
Text GLabel 3300 3700 0    50   Input ~ 0
Motor2_DIR
Text GLabel 3300 3800 0    50   Input ~ 0
Motor2_PWM
Text GLabel 1400 3000 2    50   Input ~ 0
5V
Text GLabel 1400 3100 2    50   Input ~ 0
3.3V
Text GLabel 1400 3300 2    50   Input ~ 0
GND
Text GLabel 1400 3400 2    50   Input ~ 0
Motor1_DIR
Text GLabel 1400 3500 2    50   Input ~ 0
Motor1_PWM
Text GLabel 1400 3600 2    50   Input ~ 0
Motor2_DIR
Text GLabel 1400 3700 2    50   Input ~ 0
Motor2_PWM
Text GLabel 3300 4100 0    50   Input ~ 0
Motor1_Current
Text GLabel 3300 4500 0    50   Input ~ 0
GND
Text GLabel 5150 4600 2    50   Input ~ 0
3V_ESP
Text GLabel 5150 3900 2    50   Input ~ 0
Motor2_Current
Text GLabel 5150 4000 2    50   Input ~ 0
BatterySense
Text GLabel 5150 4100 2    50   Input ~ 0
SolarSense
$Comp
L Connector:Screw_Terminal_01x04 Mot1Current1
U 1 1 63B119D5
P 7250 4150
F 0 "Mot1Current1" H 7330 4142 50  0000 L CNN
F 1 "Analog Motorstrom, VCC" H 7330 4051 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 7250 4150 50  0001 C CNN
F 3 "~" H 7250 4150 50  0001 C CNN
	1    7250 4150
	1    0    0    -1  
$EndComp
Text GLabel 7050 4150 0    50   Input ~ 0
3.3V
Text GLabel 7050 4050 0    50   Input ~ 0
5V
Text GLabel 7050 4250 0    50   Input ~ 0
GND
Text GLabel 7050 4350 0    50   Input ~ 0
Motor1_Current
$Comp
L Connector:Screw_Terminal_01x04 Mot2Current1
U 1 1 63B3D549
P 7250 4650
F 0 "Mot2Current1" H 7330 4642 50  0000 L CNN
F 1 "Analog Motorstrom, VCC" H 7330 4551 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 7250 4650 50  0001 C CNN
F 3 "~" H 7250 4650 50  0001 C CNN
	1    7250 4650
	1    0    0    -1  
$EndComp
Text GLabel 7050 4650 0    50   Input ~ 0
3.3V
Text GLabel 7050 4550 0    50   Input ~ 0
5V
Text GLabel 7050 4750 0    50   Input ~ 0
GND
Text GLabel 7050 4850 0    50   Input ~ 0
Motor2_Current
$Comp
L Connector:Screw_Terminal_01x03 Battery1
U 1 1 63B3DD0C
P 7800 5500
F 0 "Battery1" H 7880 5542 50  0000 L CNN
F 1 "MAIN" H 7880 5451 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-3_P5.08mm" H 7800 5500 50  0001 C CNN
F 3 "~" H 7800 5500 50  0001 C CNN
	1    7800 5500
	1    0    0    -1  
$EndComp
Text GLabel 6500 5400 0    50   Input ~ 0
BatterySense
Text GLabel 6450 6050 0    50   Input ~ 0
SolarSense
Text GLabel 5150 3300 2    50   Input ~ 0
SDA
Text GLabel 5150 3000 2    50   Input ~ 0
SCL
Text GLabel 6250 1800 0    50   Input ~ 0
5V
Text GLabel 6250 1900 0    50   Input ~ 0
3.3V
Text GLabel 6250 2000 0    50   Input ~ 0
GND
Text GLabel 6250 2100 0    50   Input ~ 0
SCL
Text GLabel 6250 2200 0    50   Input ~ 0
SDA
Text GLabel 7050 3500 0    50   Input ~ 0
Door_open
Text GLabel 7050 3600 0    50   Input ~ 0
GND
Text GLabel 7050 3800 0    50   Input ~ 0
Door_closed
Text GLabel 7050 3700 0    50   Input ~ 0
GND
Text GLabel 5150 3700 2    50   Input ~ 0
Door_open
Text GLabel 5150 3800 2    50   Input ~ 0
Door_closed
Text GLabel 7050 2700 0    50   Input ~ 0
5V
Text GLabel 7050 2800 0    50   Input ~ 0
3.3V
Text GLabel 7050 2900 0    50   Input ~ 0
GND
Text GLabel 7050 3000 0    50   Input ~ 0
Relais1
Text GLabel 7050 3100 0    50   Input ~ 0
Relais2
Text GLabel 7050 3200 0    50   Input ~ 0
Relais3
Text GLabel 7050 3300 0    50   Input ~ 0
Relais4
Text GLabel 5150 3600 2    50   Input ~ 0
Relais4
Text GLabel 5150 3500 2    50   Input ~ 0
Relais3
Text GLabel 5150 3400 2    50   Input ~ 0
Relais2
Text GLabel 5150 2900 2    50   Input ~ 0
Relais1
Text GLabel 3300 4600 0    50   Input ~ 0
5V
$Comp
L Connector:Screw_Terminal_01x02 LightSwitch1
U 1 1 63B446B0
P 1200 4600
F 0 "LightSwitch1" H 1118 4275 50  0000 C CNN
F 1 "Light_switch" H 1118 4366 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 1200 4600 50  0001 C CNN
F 3 "~" H 1200 4600 50  0001 C CNN
	1    1200 4600
	-1   0    0    1   
$EndComp
$Comp
L Connector:Screw_Terminal_01x04 LightLoad1
U 1 1 63B457AB
P 1200 4250
F 0 "LightLoad1" H 1118 3825 50  0000 C CNN
F 1 "LightLoad" H 1118 3916 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 1200 4250 50  0001 C CNN
F 3 "~" H 1200 4250 50  0001 C CNN
	1    1200 4250
	-1   0    0    1   
$EndComp
Text GLabel 1400 4600 2    50   Input ~ 0
GND
Text GLabel 1400 4500 2    50   Input ~ 0
LightSwitch
Text GLabel 3300 3900 0    50   Input ~ 0
LightSwitch
Text GLabel 1400 4350 2    50   Input ~ 0
5V
Text GLabel 1400 4250 2    50   Input ~ 0
3.3V
Text GLabel 1400 4150 2    50   Input ~ 0
GND
Text GLabel 1400 4050 2    50   Input ~ 0
LightTrigger
Text GLabel 3300 4000 0    50   Input ~ 0
LightTrigger
$Comp
L Connector:Screw_Terminal_01x05 J11
U 1 1 63B490CC
P 7050 2000
F 0 "J11" H 6968 1575 50  0000 C CNN
F 1 "I2C-BME" H 6968 1666 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B5B-XH-A_1x05_P2.50mm_Vertical" H 7050 2000 50  0001 C CNN
F 3 "~" H 7050 2000 50  0001 C CNN
	1    7050 2000
	1    0    0    -1  
$EndComp
Text GLabel 6850 1800 0    50   Input ~ 0
5V
Text GLabel 6850 1900 0    50   Input ~ 0
3.3V
Text GLabel 6850 2000 0    50   Input ~ 0
GND
Text GLabel 6850 2100 0    50   Input ~ 0
SCL
Text GLabel 6850 2200 0    50   Input ~ 0
SDA
$Comp
L Connector:Screw_Terminal_01x05 J12
U 1 1 63B49696
P 7700 2000
F 0 "J12" H 7618 1575 50  0000 C CNN
F 1 "I2C-BME" H 7618 1666 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B5B-XH-A_1x05_P2.50mm_Vertical" H 7700 2000 50  0001 C CNN
F 3 "~" H 7700 2000 50  0001 C CNN
	1    7700 2000
	1    0    0    -1  
$EndComp
Text GLabel 7500 1800 0    50   Input ~ 0
5V
Text GLabel 7500 1900 0    50   Input ~ 0
3.3V
Text GLabel 7500 2000 0    50   Input ~ 0
GND
Text GLabel 7500 2100 0    50   Input ~ 0
SCL
Text GLabel 7500 2200 0    50   Input ~ 0
SDA
$Comp
L Connector:Screw_Terminal_01x05 J13
U 1 1 63B498AC
P 8350 2000
F 0 "J13" H 8268 1575 50  0000 C CNN
F 1 "I2C-BME" H 8268 1666 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B5B-XH-A_1x05_P2.50mm_Vertical" H 8350 2000 50  0001 C CNN
F 3 "~" H 8350 2000 50  0001 C CNN
	1    8350 2000
	1    0    0    -1  
$EndComp
Text GLabel 8150 1800 0    50   Input ~ 0
5V
Text GLabel 8150 1900 0    50   Input ~ 0
3.3V
Text GLabel 8150 2000 0    50   Input ~ 0
GND
Text GLabel 8150 2100 0    50   Input ~ 0
SCL
Text GLabel 8150 2200 0    50   Input ~ 0
SDA
$Comp
L Device:R_Small R1
U 1 1 63B4B235
P 6800 5400
F 0 "R1" V 6604 5400 50  0000 C CNN
F 1 "10K" V 6695 5400 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 6800 5400 50  0001 C CNN
F 3 "~" H 6800 5400 50  0001 C CNN
	1    6800 5400
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R2
U 1 1 63B4BAE1
P 6700 5550
F 0 "R2" H 6759 5596 50  0000 L CNN
F 1 "1K" H 6759 5505 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 6700 5550 50  0001 C CNN
F 3 "~" H 6700 5550 50  0001 C CNN
	1    6700 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 5400 6700 5400
Wire Wire Line
	6700 5450 6700 5400
Connection ~ 6700 5400
Text GLabel 6700 5650 3    50   Input ~ 0
GND
$Comp
L Device:R_Small R4
U 1 1 63B4EF82
P 6700 6200
F 0 "R4" H 6759 6246 50  0000 L CNN
F 1 "1K" H 6759 6155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 6700 6200 50  0001 C CNN
F 3 "~" H 6700 6200 50  0001 C CNN
	1    6700 6200
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R3
U 1 1 63B4FA3A
P 6800 6050
F 0 "R3" V 6604 6050 50  0000 C CNN
F 1 "10K" V 6695 6050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" H 6800 6050 50  0001 C CNN
F 3 "~" H 6800 6050 50  0001 C CNN
	1    6800 6050
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 6050 6700 6050
Wire Wire Line
	6700 6100 6700 6050
Connection ~ 6700 6050
Text GLabel 6700 6300 3    50   Input ~ 0
GND
$Comp
L Connector:Screw_Terminal_01x03 Main5V1
U 1 1 63B51A9A
P 1200 5200
F 0 "Main5V1" H 1118 4875 50  0000 C CNN
F 1 "Main 5V" H 1118 4966 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 1200 5200 50  0001 C CNN
F 3 "~" H 1200 5200 50  0001 C CNN
	1    1200 5200
	-1   0    0    1   
$EndComp
Text GLabel 2050 5100 2    50   Input ~ 0
5V
Text GLabel 1400 5300 2    50   Input ~ 0
GND
Text GLabel 7150 5400 1    50   Input ~ 0
MAIN_VCC
Text GLabel 7600 5500 0    50   Input ~ 0
MAIN_GND
Wire Wire Line
	6900 5400 7600 5400
$Comp
L Connector:Screw_Terminal_01x10 BatteryVCC1
U 1 1 63B5CBA3
P 9300 5800
F 0 "BatteryVCC1" H 9380 5792 50  0000 L CNN
F 1 "MAIN_VCC" H 9380 5701 50  0000 L CNN
F 2 "TerminalBlock_RND:TerminalBlock_RND_205-00240_1x10_P5.08mm_Horizontal" H 9300 5800 50  0001 C CNN
F 3 "~" H 9300 5800 50  0001 C CNN
	1    9300 5800
	1    0    0    -1  
$EndComp
Text GLabel 9100 5400 0    50   Input ~ 0
MAIN_VCC
Text GLabel 9100 5500 0    50   Input ~ 0
MAIN_VCC
Text GLabel 9100 5600 0    50   Input ~ 0
MAIN_VCC
Text GLabel 9100 5700 0    50   Input ~ 0
MAIN_VCC
Text GLabel 9100 5800 0    50   Input ~ 0
MAIN_VCC
Text GLabel 9100 5900 0    50   Input ~ 0
MAIN_GND
Text GLabel 9100 6000 0    50   Input ~ 0
MAIN_GND
Text GLabel 9100 6100 0    50   Input ~ 0
MAIN_GND
Text GLabel 9100 6200 0    50   Input ~ 0
MAIN_GND
Text GLabel 9100 6300 0    50   Input ~ 0
MAIN_GND
$Comp
L Jumper:Jumper_2_Bridged JP1
U 1 1 63B64618
P 1850 5100
F 0 "JP1" H 1850 5295 50  0000 C CNN
F 1 "Jumper_Mains" H 1850 5204 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 1850 5100 50  0001 C CNN
F 3 "~" H 1850 5100 50  0001 C CNN
	1    1850 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 5100 1650 5100
Text GLabel 2750 5200 2    50   Input ~ 0
3V_ESP
$Comp
L Jumper:Jumper_3_Bridged12 JP2
U 1 1 63B1ED2A
P 2500 5200
F 0 "JP2" H 2500 5395 50  0000 C CNN
F 1 "Jumper_3v_ext" H 2500 5304 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 2500 5200 50  0001 C CNN
F 3 "~" H 2500 5200 50  0001 C CNN
	1    2500 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 5200 2250 5200
Text GLabel 2500 5350 3    50   Input ~ 0
3.3V
Text GLabel 2250 5200 3    50   Input ~ 0
3V_ext
Connection ~ 2250 5200
Wire Wire Line
	2250 5200 2300 5200
Wire Wire Line
	7600 5600 7600 6050
Wire Wire Line
	6900 6050 7600 6050
Text GLabel 3300 3400 0    50   Input ~ 0
MotorEnable
Text GLabel 1400 3200 2    50   Input ~ 0
MotorEnable
$EndSCHEMATC
