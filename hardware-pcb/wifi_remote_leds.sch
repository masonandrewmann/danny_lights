EESchema Schematic File Version 4
LIBS:wifi_remote_leds-cache
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
L ESP8266:NodeMCU_1.0_(ESP-12E) U1
U 1 1 62258228
P 5900 3550
F 0 "U1" H 5900 4637 60  0000 C CNN
F 1 "NodeMCU_1.0_(ESP-12E)" H 5900 4531 60  0000 C CNN
F 2 "ESP8266:NodeMCU1.0(12-E)" H 5300 2700 60  0001 C CNN
F 3 "" H 5300 2700 60  0000 C CNN
	1    5900 3550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 62258D58
P 5400 4850
F 0 "SW1" H 5400 5085 50  0000 C CNN
F 1 "SW_SPST" H 5400 4994 50  0000 C CNN
F 2 "Connector_JST:JST_XH_S2B-XH-A_1x02_P2.50mm_Horizontal" H 5400 4850 50  0001 C CNN
F 3 "~" H 5400 4850 50  0001 C CNN
	1    5400 4850
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 6225A35C
P 6850 4850
F 0 "SW2" H 6850 5135 50  0000 C CNN
F 1 "SW_Push" H 6850 5044 50  0000 C CNN
F 2 "Connector_JST:JST_XH_S2B-XH-A_1x02_P2.50mm_Horizontal" H 6850 5050 50  0001 C CNN
F 3 "~" H 6850 5050 50  0001 C CNN
	1    6850 4850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 6225BD37
P 3900 5150
F 0 "#PWR0101" H 3900 4900 50  0001 C CNN
F 1 "GND" H 3905 4977 50  0000 C CNN
F 2 "" H 3900 5150 50  0001 C CNN
F 3 "" H 3900 5150 50  0001 C CNN
	1    3900 5150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0102
U 1 1 6225B2CD
P 3900 4850
F 0 "#PWR0102" H 3900 4700 50  0001 C CNN
F 1 "+5V" H 3915 5023 50  0000 C CNN
F 2 "" H 3900 4850 50  0001 C CNN
F 3 "" H 3900 4850 50  0001 C CNN
	1    3900 4850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 6225E48E
P 7150 4150
F 0 "#PWR0103" H 7150 3900 50  0001 C CNN
F 1 "GND" V 7150 3950 50  0000 C CNN
F 2 "" H 7150 4150 50  0001 C CNN
F 3 "" H 7150 4150 50  0001 C CNN
	1    7150 4150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7150 4150 6700 4150
Text GLabel 7200 3550 2    50   Input ~ 0
IR_SENSOR
Wire Wire Line
	7200 3550 6700 3550
$Comp
L power:GND #PWR0104
U 1 1 6225EDCA
P 6700 3450
F 0 "#PWR0104" H 6700 3200 50  0001 C CNN
F 1 "GND" V 6700 3250 50  0000 C CNN
F 2 "" H 6700 3450 50  0001 C CNN
F 3 "" H 6700 3450 50  0001 C CNN
	1    6700 3450
	0    -1   -1   0   
$EndComp
Text GLabel 7200 3250 2    50   Input ~ 0
LED_OUT
$Comp
L Device:R_US R3
U 1 1 62260235
P 6950 3250
F 0 "R3" V 7050 3250 50  0000 C CNN
F 1 "470" V 6850 3250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6990 3240 50  0001 C CNN
F 3 "~" H 6950 3250 50  0001 C CNN
	1    6950 3250
	0    1    1    0   
$EndComp
Wire Wire Line
	7100 3250 7200 3250
Wire Wire Line
	5050 5200 5050 5250
$Comp
L power:GND #PWR0105
U 1 1 622632CD
P 5050 5250
F 0 "#PWR0105" H 5050 5000 50  0001 C CNN
F 1 "GND" V 5050 5050 50  0000 C CNN
F 2 "" H 5050 5250 50  0001 C CNN
F 3 "" H 5050 5250 50  0001 C CNN
	1    5050 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R1
U 1 1 6225A9D8
P 5050 5050
F 0 "R1" H 5118 5096 50  0000 L CNN
F 1 "10k" H 5118 5005 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5090 5040 50  0001 C CNN
F 3 "~" H 5050 5050 50  0001 C CNN
	1    5050 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 4850 5050 4900
Wire Wire Line
	5050 4850 5200 4850
Text GLabel 7200 4250 2    50   Input ~ 0
3v3
Wire Wire Line
	6700 4250 7200 4250
Text GLabel 5700 4850 2    50   Input ~ 0
3v3
Wire Wire Line
	5600 4850 5700 4850
Text GLabel 7150 4850 2    50   Input ~ 0
3v3
Wire Wire Line
	7150 4850 7050 4850
Text GLabel 7200 3750 2    50   Input ~ 0
RESET_PB
Text GLabel 7200 3650 2    50   Input ~ 0
MODE_SW
Wire Wire Line
	7200 3650 6700 3650
Wire Wire Line
	7200 3750 6700 3750
Text GLabel 4900 4850 0    50   Input ~ 0
MODE_SW
Text GLabel 6400 4850 0    50   Input ~ 0
RESET_PB
Wire Wire Line
	6550 5150 6550 5200
$Comp
L power:GND #PWR0106
U 1 1 6226DECB
P 6550 5200
F 0 "#PWR0106" H 6550 4950 50  0001 C CNN
F 1 "GND" V 6550 5000 50  0000 C CNN
F 2 "" H 6550 5200 50  0001 C CNN
F 3 "" H 6550 5200 50  0001 C CNN
	1    6550 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R2
U 1 1 6226DED1
P 6550 5000
F 0 "R2" H 6618 5046 50  0000 L CNN
F 1 "10k" H 6618 4955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6590 4990 50  0001 C CNN
F 3 "~" H 6550 5000 50  0001 C CNN
	1    6550 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 4850 6550 4850
Wire Wire Line
	6650 4850 6550 4850
Connection ~ 6550 4850
Wire Wire Line
	4900 4850 5050 4850
Connection ~ 5050 4850
$Comp
L Device:CP1 C1
U 1 1 622740EA
P 3900 5000
F 0 "C1" H 4015 5046 50  0000 L CNN
F 1 "1000uF" H 4015 4955 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D8.0mm_P5.00mm" H 3900 5000 50  0001 C CNN
F 3 "~" H 3900 5000 50  0001 C CNN
	1    3900 5000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0107
U 1 1 62276E80
P 5000 4250
F 0 "#PWR0107" H 5000 4100 50  0001 C CNN
F 1 "+5V" V 5000 4500 50  0000 C CNN
F 2 "" H 5000 4250 50  0001 C CNN
F 3 "" H 5000 4250 50  0001 C CNN
	1    5000 4250
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 62277A92
P 5000 4150
F 0 "#PWR0108" H 5000 3900 50  0001 C CNN
F 1 "GND" V 5005 3977 50  0000 C CNN
F 2 "" H 5000 4150 50  0001 C CNN
F 3 "" H 5000 4150 50  0001 C CNN
	1    5000 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	5000 4150 5100 4150
Wire Wire Line
	5000 4250 5100 4250
$Comp
L power:GND #PWR0109
U 1 1 62278865
P 5000 3750
F 0 "#PWR0109" H 5000 3500 50  0001 C CNN
F 1 "GND" V 5005 3577 50  0000 C CNN
F 2 "" H 5000 3750 50  0001 C CNN
F 3 "" H 5000 3750 50  0001 C CNN
	1    5000 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	5000 3750 5100 3750
$Comp
L Connector:Conn_01x02_Male J3
U 1 1 6227A0D0
P 2600 4950
F 0 "J3" H 2708 5131 50  0000 C CNN
F 1 "POWER IN" H 2708 5040 50  0000 C CNN
F 2 "Connector_JST:JST_XH_S2B-XH-A_1x02_P2.50mm_Horizontal" H 2600 4950 50  0001 C CNN
F 3 "~" H 2600 4950 50  0001 C CNN
	1    2600 4950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0110
U 1 1 6227AD70
P 2950 4950
F 0 "#PWR0110" H 2950 4800 50  0001 C CNN
F 1 "+5V" V 2950 5200 50  0000 C CNN
F 2 "" H 2950 4950 50  0001 C CNN
F 3 "" H 2950 4950 50  0001 C CNN
	1    2950 4950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 6227B4EA
P 2950 5050
F 0 "#PWR0111" H 2950 4800 50  0001 C CNN
F 1 "GND" V 2955 4877 50  0000 C CNN
F 2 "" H 2950 5050 50  0001 C CNN
F 3 "" H 2950 5050 50  0001 C CNN
	1    2950 5050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2950 5050 2800 5050
Wire Wire Line
	2950 4950 2800 4950
Text GLabel 2900 4400 2    50   Input ~ 0
LED_OUT
$Comp
L power:+5V #PWR0112
U 1 1 622821EB
P 2900 4500
F 0 "#PWR0112" H 2900 4350 50  0001 C CNN
F 1 "+5V" V 2900 4750 50  0000 C CNN
F 2 "" H 2900 4500 50  0001 C CNN
F 3 "" H 2900 4500 50  0001 C CNN
	1    2900 4500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 622821F1
P 2900 4600
F 0 "#PWR0113" H 2900 4350 50  0001 C CNN
F 1 "GND" V 2905 4427 50  0000 C CNN
F 2 "" H 2900 4600 50  0001 C CNN
F 3 "" H 2900 4600 50  0001 C CNN
	1    2900 4600
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x03_Male J2
U 1 1 622826BB
P 2600 4500
F 0 "J2" H 2708 4781 50  0000 C CNN
F 1 "LED out connector" H 2708 4690 50  0000 C CNN
F 2 "Connector_JST:JST_XH_S3B-XH-A_1x03_P2.50mm_Horizontal" H 2600 4500 50  0001 C CNN
F 3 "~" H 2600 4500 50  0001 C CNN
	1    2600 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 4400 2900 4400
Wire Wire Line
	2800 4500 2900 4500
Wire Wire Line
	2800 4600 2900 4600
$Comp
L Mechanical:MountingHole H1
U 1 1 62284826
P 2750 5700
F 0 "H1" H 2850 5746 50  0000 L CNN
F 1 "MountingHole" H 2850 5655 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad" H 2750 5700 50  0001 C CNN
F 3 "~" H 2750 5700 50  0001 C CNN
	1    2750 5700
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 6228562A
P 2750 5900
F 0 "H2" H 2850 5946 50  0000 L CNN
F 1 "MountingHole" H 2850 5855 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad" H 2750 5900 50  0001 C CNN
F 3 "~" H 2750 5900 50  0001 C CNN
	1    2750 5900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 62285D25
P 2750 6100
F 0 "H3" H 2850 6146 50  0000 L CNN
F 1 "MountingHole" H 2850 6055 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad" H 2750 6100 50  0001 C CNN
F 3 "~" H 2750 6100 50  0001 C CNN
	1    2750 6100
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 62286342
P 2750 6300
F 0 "H4" H 2850 6346 50  0000 L CNN
F 1 "MountingHole" H 2850 6255 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5_Pad" H 2750 6300 50  0001 C CNN
F 3 "~" H 2750 6300 50  0001 C CNN
	1    2750 6300
	1    0    0    -1  
$EndComp
Text GLabel 2900 3850 2    50   Input ~ 0
IR_SENSOR
$Comp
L Connector:Conn_01x03_Male J1
U 1 1 62288A86
P 2600 3950
F 0 "J1" H 2708 4231 50  0000 C CNN
F 1 "IR_connector" H 2708 4140 50  0000 C CNN
F 2 "Connector_JST:JST_XH_S3B-XH-A_1x03_P2.50mm_Horizontal" H 2600 3950 50  0001 C CNN
F 3 "~" H 2600 3950 50  0001 C CNN
	1    2600 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3850 2900 3850
Wire Wire Line
	2800 4050 2900 4050
Wire Wire Line
	2800 3950 2900 3950
$Comp
L power:GND #PWR0114
U 1 1 62287884
P 2900 4050
F 0 "#PWR0114" H 2900 3800 50  0001 C CNN
F 1 "GND" V 2905 3877 50  0000 C CNN
F 2 "" H 2900 4050 50  0001 C CNN
F 3 "" H 2900 4050 50  0001 C CNN
	1    2900 4050
	0    -1   -1   0   
$EndComp
Text GLabel 2900 3950 2    50   Input ~ 0
3v3
Wire Wire Line
	6700 3250 6800 3250
$EndSCHEMATC
