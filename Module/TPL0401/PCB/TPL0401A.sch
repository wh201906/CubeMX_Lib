EESchema Schematic File Version 4
EELAYER 30 0
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
L Potentiometer_Digital:TPL0401A-10-Q1 U1
U 1 1 60D5AA60
P 5550 2850
F 0 "U1" H 5550 3531 50  0000 C CNN
F 1 "TPL0401A-10-Q1" H 5550 3440 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:Texas_R-PDSO-G6" H 5600 2400 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tpl0401a-10-q1.pdf" H 5600 2300 50  0001 L CNN
	1    5550 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J1
U 1 1 60D5B895
P 3400 2400
F 0 "J1" H 3508 2681 50  0000 C CNN
F 1 "Conn_01x04_Male" H 3508 2590 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3400 2400 50  0001 C CNN
F 3 "~" H 3400 2400 50  0001 C CNN
	1    3400 2400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR03
U 1 1 60D5DCE7
P 5550 2050
F 0 "#PWR03" H 5550 1900 50  0001 C CNN
F 1 "+5V" H 5565 2223 50  0000 C CNN
F 2 "" H 5550 2050 50  0001 C CNN
F 3 "" H 5550 2050 50  0001 C CNN
	1    5550 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 60D5E874
P 5550 3550
F 0 "#PWR04" H 5550 3300 50  0001 C CNN
F 1 "GND" H 5555 3377 50  0000 C CNN
F 2 "" H 5550 3550 50  0001 C CNN
F 3 "" H 5550 3550 50  0001 C CNN
	1    5550 3550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 60D5F5B2
P 4500 1900
F 0 "#PWR01" H 4500 1750 50  0001 C CNN
F 1 "+5V" H 4515 2073 50  0000 C CNN
F 2 "" H 4500 1900 50  0001 C CNN
F 3 "" H 4500 1900 50  0001 C CNN
	1    4500 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 60D5FA4F
P 4500 2200
F 0 "#PWR02" H 4500 1950 50  0001 C CNN
F 1 "GND" H 4505 2027 50  0000 C CNN
F 2 "" H 4500 2200 50  0001 C CNN
F 3 "" H 4500 2200 50  0001 C CNN
	1    4500 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 2350 5550 2050
Wire Wire Line
	5550 3350 5550 3550
Text GLabel 5000 2750 0    50   Input ~ 0
SDA
Text GLabel 5000 2950 0    50   Input ~ 0
SCL
Text GLabel 4150 2750 2    50   Output ~ 0
SCL
Text GLabel 4150 2950 2    50   Output ~ 0
SDA
$Comp
L Device:C C1
U 1 1 60D62721
P 4250 2050
F 0 "C1" H 4365 2096 50  0000 L CNN
F 1 "C" H 4365 2005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4288 1900 50  0001 C CNN
F 3 "~" H 4250 2050 50  0001 C CNN
	1    4250 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 1900 4250 1900
Wire Wire Line
	4250 1900 3600 1900
Wire Wire Line
	3600 1900 3600 2300
Connection ~ 4250 1900
Wire Wire Line
	4500 2200 4250 2200
Wire Wire Line
	4250 2200 3850 2200
Wire Wire Line
	3850 2200 3850 2400
Wire Wire Line
	3850 2400 3600 2400
Connection ~ 4250 2200
Wire Wire Line
	3850 2750 3850 2500
Wire Wire Line
	3850 2500 3600 2500
Wire Wire Line
	3700 2950 3700 2600
Wire Wire Line
	3700 2600 3600 2600
Wire Wire Line
	5050 2750 5000 2750
Wire Wire Line
	5050 2950 5000 2950
$Comp
L Connector:Conn_01x04_Male J2
U 1 1 60D6AB71
P 6650 2650
F 0 "J2" H 6622 2624 50  0000 R CNN
F 1 "Conn_01x04_Male" H 6622 2533 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 6650 2650 50  0001 C CNN
F 3 "~" H 6650 2650 50  0001 C CNN
	1    6650 2650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6450 2650 6450 2550
Wire Wire Line
	6450 2550 6050 2550
Connection ~ 6450 2550
Wire Wire Line
	6450 2750 6450 2850
Wire Wire Line
	6450 2850 6050 2850
Connection ~ 6450 2850
$Comp
L power:+5V #PWR0101
U 1 1 60D5D775
P 4550 2600
F 0 "#PWR0101" H 4550 2450 50  0001 C CNN
F 1 "+5V" H 4565 2773 50  0000 C CNN
F 2 "" H 4550 2600 50  0001 C CNN
F 3 "" H 4550 2600 50  0001 C CNN
	1    4550 2600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 60D60549
P 4200 2600
F 0 "R1" V 3993 2600 50  0000 C CNN
F 1 "R" V 4084 2600 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4130 2600 50  0001 C CNN
F 3 "~" H 4200 2600 50  0001 C CNN
	1    4200 2600
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 60D5F6F4
P 4200 3100
F 0 "R2" V 4347 3100 50  0000 C CNN
F 1 "R" V 4280 3100 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4130 3100 50  0001 C CNN
F 3 "~" H 4200 3100 50  0001 C CNN
	1    4200 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	3850 2750 4000 2750
Wire Wire Line
	3700 2950 4000 2950
Wire Wire Line
	4050 3100 4000 3100
Wire Wire Line
	4000 3100 4000 2950
Connection ~ 4000 2950
Wire Wire Line
	4000 2950 4150 2950
Wire Wire Line
	4050 2600 4000 2600
Wire Wire Line
	4000 2600 4000 2750
Connection ~ 4000 2750
Wire Wire Line
	4000 2750 4150 2750
Wire Wire Line
	4350 2600 4550 2600
Wire Wire Line
	4350 3100 4550 3100
Wire Wire Line
	4550 3100 4550 2600
Connection ~ 4550 2600
$EndSCHEMATC
