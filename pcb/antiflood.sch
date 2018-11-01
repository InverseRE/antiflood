EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:antiflood-cache
EELAYER 25 0
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
L GND #PWR01
U 1 1 5B3E6CFC
P 1100 5350
F 0 "#PWR01" H 1100 5100 50  0001 C CNN
F 1 "GND" H 1100 5200 50  0000 C CNN
F 2 "" H 1100 5350 50  0001 C CNN
F 3 "" H 1100 5350 50  0001 C CNN
	1    1100 5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5B3E7376
P 10250 5900
F 0 "#PWR02" H 10250 5650 50  0001 C CNN
F 1 "GND" H 10250 5750 50  0000 C CNN
F 2 "" H 10250 5900 50  0001 C CNN
F 3 "" H 10250 5900 50  0001 C CNN
	1    10250 5900
	1    0    0    -1  
$EndComp
Text Label 3100 3750 0    60   ~ 0
A0
Text Label 3100 3850 0    60   ~ 0
A1
Text Label 3100 3950 0    60   ~ 0
A2
Text Label 3100 4050 0    60   ~ 0
A3
Text Label 3100 4150 0    60   ~ 0
A4
Text Label 3100 4250 0    60   ~ 0
A5
Text Label 1000 4250 0    60   ~ 0
A6
Text Label 1000 4350 0    60   ~ 0
A7
Text Label 9850 4100 0    60   ~ 0
A0
Text Label 9850 4400 0    60   ~ 0
A1
Text Label 9850 4700 0    60   ~ 0
A2
Text Label 9850 5000 0    60   ~ 0
A3
Text Label 9850 5300 0    60   ~ 0
A4
Text Label 9850 5600 0    60   ~ 0
A5
$Comp
L R R17
U 1 1 5B3E8AD7
P 10150 700
F 0 "R17" V 10230 700 50  0000 C CNN
F 1 "100" V 10150 700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 10080 700 50  0001 C CNN
F 3 "" H 10150 700 50  0001 C CNN
	1    10150 700 
	0    1    1    0   
$EndComp
$Comp
L LED D3
U 1 1 5B3E8B6C
P 10550 700
F 0 "D3" H 10550 800 50  0000 C CNN
F 1 "LED" H 10550 600 50  0000 C CNN
F 2 "LEDs:LED_1206" H 10550 700 50  0001 C CNN
F 3 "" H 10550 700 50  0001 C CNN
	1    10550 700 
	-1   0    0    1   
$EndComp
$Comp
L R R18
U 1 1 5B3E8C83
P 10150 1000
F 0 "R18" V 10230 1000 50  0000 C CNN
F 1 "100" V 10150 1000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 10080 1000 50  0001 C CNN
F 3 "" H 10150 1000 50  0001 C CNN
	1    10150 1000
	0    1    1    0   
$EndComp
$Comp
L LED D4
U 1 1 5B3E8C89
P 10550 1000
F 0 "D4" H 10550 1100 50  0000 C CNN
F 1 "LED" H 10550 900 50  0000 C CNN
F 2 "LEDs:LED_1206" H 10550 1000 50  0001 C CNN
F 3 "" H 10550 1000 50  0001 C CNN
	1    10550 1000
	-1   0    0    1   
$EndComp
$Comp
L R R19
U 1 1 5B3E8E07
P 10150 1300
F 0 "R19" V 10230 1300 50  0000 C CNN
F 1 "100" V 10150 1300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 10080 1300 50  0001 C CNN
F 3 "" H 10150 1300 50  0001 C CNN
	1    10150 1300
	0    1    1    0   
$EndComp
$Comp
L LED D5
U 1 1 5B3E8E0D
P 10550 1300
F 0 "D5" H 10550 1400 50  0000 C CNN
F 1 "LED" H 10550 1200 50  0000 C CNN
F 2 "LEDs:LED_1206" H 10550 1300 50  0001 C CNN
F 3 "" H 10550 1300 50  0001 C CNN
	1    10550 1300
	-1   0    0    1   
$EndComp
$Comp
L R R20
U 1 1 5B3E8E13
P 10150 1600
F 0 "R20" V 10230 1600 50  0000 C CNN
F 1 "100" V 10150 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 10080 1600 50  0001 C CNN
F 3 "" H 10150 1600 50  0001 C CNN
	1    10150 1600
	0    1    1    0   
$EndComp
$Comp
L LED D6
U 1 1 5B3E8E19
P 10550 1600
F 0 "D6" H 10550 1700 50  0000 C CNN
F 1 "LED" H 10550 1500 50  0000 C CNN
F 2 "LEDs:LED_1206" H 10550 1600 50  0001 C CNN
F 3 "" H 10550 1600 50  0001 C CNN
	1    10550 1600
	-1   0    0    1   
$EndComp
$Comp
L R R21
U 1 1 5B3E908D
P 10150 1900
F 0 "R21" V 10230 1900 50  0000 C CNN
F 1 "100" V 10150 1900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 10080 1900 50  0001 C CNN
F 3 "" H 10150 1900 50  0001 C CNN
	1    10150 1900
	0    1    1    0   
$EndComp
$Comp
L LED D7
U 1 1 5B3E9093
P 10550 1900
F 0 "D7" H 10550 2000 50  0000 C CNN
F 1 "LED" H 10550 1800 50  0000 C CNN
F 2 "LEDs:LED_1206" H 10550 1900 50  0001 C CNN
F 3 "" H 10550 1900 50  0001 C CNN
	1    10550 1900
	-1   0    0    1   
$EndComp
$Comp
L R R22
U 1 1 5B3E9099
P 10150 2200
F 0 "R22" V 10230 2200 50  0000 C CNN
F 1 "100" V 10150 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 10080 2200 50  0001 C CNN
F 3 "" H 10150 2200 50  0001 C CNN
	1    10150 2200
	0    1    1    0   
$EndComp
$Comp
L LED D8
U 1 1 5B3E909F
P 10550 2200
F 0 "D8" H 10550 2300 50  0000 C CNN
F 1 "LED" H 10550 2100 50  0000 C CNN
F 2 "LEDs:LED_1206" H 10550 2200 50  0001 C CNN
F 3 "" H 10550 2200 50  0001 C CNN
	1    10550 2200
	-1   0    0    1   
$EndComp
Text Label 9850 700  0    60   ~ 0
L0
Text Label 9850 1000 0    60   ~ 0
L1
Text Label 9850 1300 0    60   ~ 0
L2
Text Label 9850 1600 0    60   ~ 0
L3
Text Label 9850 1900 0    60   ~ 0
L4
Text Label 9850 2200 0    60   ~ 0
L5
Text Label 3100 2900 0    60   ~ 0
L0
Text Label 3100 3000 0    60   ~ 0
L1
Text Label 3100 3100 0    60   ~ 0
L2
Text Label 3100 3200 0    60   ~ 0
L3
Text Label 3100 3300 0    60   ~ 0
L4
Text Label 3100 3400 0    60   ~ 0
L5
Text Label 3100 4500 0    60   ~ 0
WRX
Text Label 3100 4600 0    60   ~ 0
WTX
Text Label 3700 3050 0    60   ~ 0
WRX
Text Label 3700 2950 0    60   ~ 0
WTX
$Comp
L +3.3V #PWR03
U 1 1 5B3ED3E6
P 5250 4100
F 0 "#PWR03" H 5250 3950 50  0001 C CNN
F 1 "+3.3V" H 5250 4240 50  0000 C CNN
F 2 "" H 5250 4100 50  0001 C CNN
F 3 "" H 5250 4100 50  0001 C CNN
	1    5250 4100
	1    0    0    -1  
$EndComp
$Comp
L Conn_02x04_Odd_Even J4
U 1 1 5B3FDD33
P 5300 3500
F 0 "J4" H 5350 3700 50  0000 C CNN
F 1 "Conn_02x04_Odd_Even" H 5350 3200 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_2x04_Pitch2.54mm" H 5300 3500 50  0001 C CNN
F 3 "" H 5300 3500 50  0001 C CNN
	1    5300 3500
	-1   0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 5B3FF0E2
P 5550 4000
F 0 "R7" V 5630 4000 50  0000 C CNN
F 1 "3k3" V 5550 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5480 4000 50  0001 C CNN
F 3 "" H 5550 4000 50  0001 C CNN
	1    5550 4000
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 5B3FF1AE
P 5750 4000
F 0 "R9" V 5830 4000 50  0000 C CNN
F 1 "3k3" V 5750 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5680 4000 50  0001 C CNN
F 3 "" H 5750 4000 50  0001 C CNN
	1    5750 4000
	1    0    0    -1  
$EndComp
Text Label 3100 4350 0    60   ~ 0
RST
$Comp
L R R15
U 1 1 5B4030EE
P 7100 4250
F 0 "R15" V 7180 4250 50  0000 C CNN
F 1 "3k3" V 7100 4250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 7030 4250 50  0001 C CNN
F 3 "" H 7100 4250 50  0001 C CNN
	1    7100 4250
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 5B40316D
P 7100 4700
F 0 "C8" H 7125 4800 50  0000 L CNN
F 1 "0.1" H 7125 4600 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7138 4550 50  0001 C CNN
F 3 "" H 7100 4700 50  0001 C CNN
	1    7100 4700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 5B403202
P 7100 5000
F 0 "#PWR04" H 7100 4750 50  0001 C CNN
F 1 "GND" H 7100 4850 50  0000 C CNN
F 2 "" H 7100 5000 50  0001 C CNN
F 3 "" H 7100 5000 50  0001 C CNN
	1    7100 5000
	1    0    0    -1  
$EndComp
Text Label 6900 4500 0    60   ~ 0
RST
$Comp
L Screw_Terminal_01x03 J5
U 1 1 5B451555
P 5550 7550
F 0 "J5" H 5550 7750 50  0000 C CNN
F 1 "01x03" H 5550 7350 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-3_P5.08mm" H 5550 7550 50  0001 C CNN
F 3 "" H 5550 7550 50  0001 C CNN
	1    5550 7550
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_01x02 J10
U 1 1 5B451794
P 10500 4200
F 0 "J10" H 10500 4300 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 10500 4000 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-2_P5.08mm" H 10500 4200 50  0001 C CNN
F 3 "" H 10500 4200 50  0001 C CNN
	1    10500 4200
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_01x02 J11
U 1 1 5B4517F7
P 10500 4500
F 0 "J11" H 10500 4600 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 10500 4300 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-2_P5.08mm" H 10500 4500 50  0001 C CNN
F 3 "" H 10500 4500 50  0001 C CNN
	1    10500 4500
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_01x02 J12
U 1 1 5B451AA2
P 10500 4800
F 0 "J12" H 10500 4900 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 10500 4600 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-2_P5.08mm" H 10500 4800 50  0001 C CNN
F 3 "" H 10500 4800 50  0001 C CNN
	1    10500 4800
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_01x02 J13
U 1 1 5B451B03
P 10500 5100
F 0 "J13" H 10500 5200 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 10500 4900 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-2_P5.08mm" H 10500 5100 50  0001 C CNN
F 3 "" H 10500 5100 50  0001 C CNN
	1    10500 5100
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_01x02 J14
U 1 1 5B4521A2
P 10500 5400
F 0 "J14" H 10500 5500 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 10500 5200 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-2_P5.08mm" H 10500 5400 50  0001 C CNN
F 3 "" H 10500 5400 50  0001 C CNN
	1    10500 5400
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_01x02 J15
U 1 1 5B4521A8
P 10500 5700
F 0 "J15" H 10500 5800 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 10500 5500 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-2_P5.08mm" H 10500 5700 50  0001 C CNN
F 3 "" H 10500 5700 50  0001 C CNN
	1    10500 5700
	1    0    0    1   
$EndComp
$Comp
L 2N3904 Q1
U 1 1 5B459296
P 1550 7200
F 0 "Q1" H 1300 7100 50  0000 L CNN
F 1 "2N3904" H 1300 7000 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Molded_Narrow" H 1750 7125 50  0001 L CIN
F 3 "" H 1550 7200 50  0001 L CNN
	1    1550 7200
	1    0    0    -1  
$EndComp
$Comp
L Screw_Terminal_01x03 J7
U 1 1 5B45B9B7
P 6500 7550
F 0 "J7" H 6500 7750 50  0000 C CNN
F 1 "01x03" H 6500 7350 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_bornier-3_P5.08mm" H 6500 7550 50  0001 C CNN
F 3 "" H 6500 7550 50  0001 C CNN
	1    6500 7550
	1    0    0    1   
$EndComp
$Comp
L R R1
U 1 1 5B45E27B
P 1100 6950
F 0 "R1" V 1180 6950 50  0000 C CNN
F 1 "82" V 1100 6950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 1030 6950 50  0001 C CNN
F 3 "" H 1100 6950 50  0001 C CNN
	1    1100 6950
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW3
U 1 1 5B460258
P 7350 4700
F 0 "SW3" H 7400 4800 50  0000 L CNN
F 1 "SW_Push" H 7350 4640 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_Tactile_SPST_Angled_PTS645Vx83-2LFS" H 7350 4900 50  0001 C CNN
F 3 "" H 7350 4900 50  0001 C CNN
	1    7350 4700
	0    1    1    0   
$EndComp
$Comp
L GND #PWR05
U 1 1 5B4619BD
P 8600 4600
F 0 "#PWR05" H 8600 4350 50  0001 C CNN
F 1 "GND" H 8600 4450 50  0000 C CNN
F 2 "" H 8600 4600 50  0001 C CNN
F 3 "" H 8600 4600 50  0001 C CNN
	1    8600 4600
	1    0    0    -1  
$EndComp
Text Label 8550 4400 0    60   ~ 0
L3
Text Label 7850 4300 0    60   ~ 0
L4
Text Label 7850 4400 0    60   ~ 0
L5
$Comp
L +5V #PWR06
U 1 1 5B464A1B
P 5900 1050
F 0 "#PWR06" H 5900 900 50  0001 C CNN
F 1 "+5V" H 5900 1190 50  0000 C CNN
F 2 "" H 5900 1050 50  0001 C CNN
F 3 "" H 5900 1050 50  0001 C CNN
	1    5900 1050
	0    1    1    0   
$EndComp
$Comp
L +12V #PWR07
U 1 1 5B464A99
P 5000 1050
F 0 "#PWR07" H 5000 900 50  0001 C CNN
F 1 "+12V" H 5000 1190 50  0000 C CNN
F 2 "" H 5000 1050 50  0001 C CNN
F 3 "" H 5000 1050 50  0001 C CNN
	1    5000 1050
	0    -1   -1   0   
$EndComp
$Comp
L VDDA #PWR08
U 1 1 5B464B17
P 5450 1500
F 0 "#PWR08" H 5450 1350 50  0001 C CNN
F 1 "VDDA" H 5450 1650 50  0000 C CNN
F 2 "" H 5450 1500 50  0001 C CNN
F 3 "" H 5450 1500 50  0001 C CNN
	1    5450 1500
	-1   0    0    1   
$EndComp
$Comp
L Barrel_Jack J1
U 1 1 5B4667B7
P 750 850
F 0 "J1" H 750 1060 50  0000 C CNN
F 1 "Barrel_Jack" H 750 675 50  0000 C CNN
F 2 "Connectors:BARREL_JACK" H 800 810 50  0001 C CNN
F 3 "" H 800 810 50  0001 C CNN
	1    750  850 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 5B467C11
P 8900 1400
F 0 "#PWR09" H 8900 1150 50  0001 C CNN
F 1 "GND" H 8900 1250 50  0000 C CNN
F 2 "" H 8900 1400 50  0001 C CNN
F 3 "" H 8900 1400 50  0001 C CNN
	1    8900 1400
	-1   0    0    -1  
$EndComp
Text Label 8900 700  2    60   ~ 0
L0
Text Label 8900 800  2    60   ~ 0
L1
Text Label 8900 900  2    60   ~ 0
L2
Text Label 8900 1000 2    60   ~ 0
L3
Text Label 8900 1100 2    60   ~ 0
L4
Text Label 8900 1200 2    60   ~ 0
L5
$Comp
L LM7812_TO220 U2
U 1 1 5B469CA7
P 2600 750
F 0 "U2" H 2450 875 50  0000 C CNN
F 1 "LM7812_TO220" H 2600 875 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220-3_Horizontal" H 2600 975 50  0001 C CIN
F 3 "" H 2600 700 50  0001 C CNN
	1    2600 750 
	1    0    0    -1  
$EndComp
$Comp
L LM7805_TO220 U3
U 1 1 5B469E8E
P 2600 1700
F 0 "U3" H 2450 1825 50  0000 C CNN
F 1 "LM7805_TO220" H 2600 1825 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220-3_Horizontal" H 2600 1925 50  0001 C CIN
F 3 "" H 2600 1650 50  0001 C CNN
	1    2600 1700
	1    0    0    -1  
$EndComp
$Comp
L LM1117-3.3 U4
U 1 1 5B469F43
P 3550 1700
F 0 "U4" H 3400 1825 50  0000 C CNN
F 1 "LM1117-3.3" H 3550 1825 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-223-3_TabPin2" H 3550 1700 50  0001 C CNN
F 3 "" H 3550 1700 50  0001 C CNN
	1    3550 1700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 5B46A9CE
P 1250 2200
F 0 "#PWR010" H 1250 1950 50  0001 C CNN
F 1 "GND" H 1250 2050 50  0000 C CNN
F 2 "" H 1250 2200 50  0001 C CNN
F 3 "" H 1250 2200 50  0001 C CNN
	1    1250 2200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR011
U 1 1 5B46D8FF
P 4200 1050
F 0 "#PWR011" H 4200 900 50  0001 C CNN
F 1 "+5V" H 4200 1190 50  0000 C CNN
F 2 "" H 4200 1050 50  0001 C CNN
F 3 "" H 4200 1050 50  0001 C CNN
	1    4200 1050
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR012
U 1 1 5B46D98F
P 4200 1350
F 0 "#PWR012" H 4200 1200 50  0001 C CNN
F 1 "+3.3V" H 4200 1490 50  0000 C CNN
F 2 "" H 4200 1350 50  0001 C CNN
F 3 "" H 4200 1350 50  0001 C CNN
	1    4200 1350
	0    1    1    0   
$EndComp
$Comp
L +12V #PWR013
U 1 1 5B46E1EC
P 4200 750
F 0 "#PWR013" H 4200 600 50  0001 C CNN
F 1 "+12V" H 4200 890 50  0000 C CNN
F 2 "" H 4200 750 50  0001 C CNN
F 3 "" H 4200 750 50  0001 C CNN
	1    4200 750 
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 5B468588
P 1600 950
F 0 "C1" H 1625 1050 50  0000 L CNN
F 1 "0.1" H 1625 850 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1638 800 50  0001 C CNN
F 3 "" H 1600 950 50  0001 C CNN
	1    1600 950 
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 5B4696C8
P 4350 1900
F 0 "C7" H 4375 2000 50  0000 L CNN
F 1 "0.1" H 4375 1800 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4388 1750 50  0001 C CNN
F 3 "" H 4350 1900 50  0001 C CNN
	1    4350 1900
	1    0    0    -1  
$EndComp
$Comp
L Jumper_NC_Dual JP1
U 1 1 5B4684C2
P 5450 1050
F 0 "JP1" H 5500 950 50  0000 L CNN
F 1 "Jumper_NC_Dual" H 5450 1150 50  0000 C BNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 5450 1050 50  0001 C CNN
F 3 "" H 5450 1050 50  0001 C CNN
	1    5450 1050
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 5B47B0DB
P 5650 2950
F 0 "R8" V 5730 2950 50  0000 C CNN
F 1 "1k5" V 5650 2950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5580 2950 50  0001 C CNN
F 3 "" H 5650 2950 50  0001 C CNN
	1    5650 2950
	0    1    1    0   
$EndComp
$Comp
L R R12
U 1 1 5B47B242
P 5900 3150
F 0 "R12" V 5980 3150 50  0000 C CNN
F 1 "3k3" V 5900 3150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5830 3150 50  0001 C CNN
F 3 "" H 5900 3150 50  0001 C CNN
	1    5900 3150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR014
U 1 1 5B480980
P 5900 700
F 0 "#PWR014" H 5900 550 50  0001 C CNN
F 1 "VCC" H 5900 850 50  0000 C CNN
F 2 "" H 5900 700 50  0001 C CNN
F 3 "" H 5900 700 50  0001 C CNN
	1    5900 700 
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR015
U 1 1 5B480B0B
P 5000 700
F 0 "#PWR015" H 5000 550 50  0001 C CNN
F 1 "+5V" H 5000 840 50  0000 C CNN
F 2 "" H 5000 700 50  0001 C CNN
F 3 "" H 5000 700 50  0001 C CNN
	1    5000 700 
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR016
U 1 1 5B481AB3
P 1100 2800
F 0 "#PWR016" H 1100 2650 50  0001 C CNN
F 1 "VCC" H 1100 2950 50  0000 C CNN
F 2 "" H 1100 2800 50  0001 C CNN
F 3 "" H 1100 2800 50  0001 C CNN
	1    1100 2800
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR017
U 1 1 5B4820F7
P 7100 3950
F 0 "#PWR017" H 7100 3800 50  0001 C CNN
F 1 "VCC" H 7100 4100 50  0000 C CNN
F 2 "" H 7100 3950 50  0001 C CNN
F 3 "" H 7100 3950 50  0001 C CNN
	1    7100 3950
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 5B50F8D1
P 3400 1150
F 0 "C5" H 3425 1250 50  0000 L CNN
F 1 "0.1" H 3425 1050 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3438 1000 50  0001 C CNN
F 3 "" H 3400 1150 50  0001 C CNN
	1    3400 1150
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR018
U 1 1 5B573BAF
P 10800 2850
F 0 "#PWR018" H 10800 2600 50  0001 C CNN
F 1 "GND" H 10800 2700 50  0000 C CNN
F 2 "" H 10800 2850 50  0001 C CNN
F 3 "" H 10800 2850 50  0001 C CNN
	1    10800 2850
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x07 J9
U 1 1 5B574A50
P 9200 1000
F 0 "J9" H 9200 1400 50  0000 C CNN
F 1 "Conn_01x07" H 9200 600 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x07_Pitch2.54mm" H 9200 1000 50  0001 C CNN
F 3 "" H 9200 1000 50  0001 C CNN
	1    9200 1000
	1    0    0    1   
$EndComp
Text Label 3700 3150 0    60   ~ 0
WCE
Text Label 3100 4700 0    60   ~ 0
WCE
$Comp
L R R3
U 1 1 5B57D4C1
P 4300 3500
F 0 "R3" V 4380 3500 50  0000 C CNN
F 1 "1k5" V 4300 3500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4230 3500 50  0001 C CNN
F 3 "" H 4300 3500 50  0001 C CNN
	1    4300 3500
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 5B57D4C7
P 4650 4000
F 0 "R5" V 4730 4000 50  0000 C CNN
F 1 "3k3" V 4650 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4580 4000 50  0001 C CNN
F 3 "" H 4650 4000 50  0001 C CNN
	1    4650 4000
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR019
U 1 1 5B57DD8C
P 4800 4350
F 0 "#PWR019" H 4800 4100 50  0001 C CNN
F 1 "GND" H 4800 4200 50  0000 C CNN
F 2 "" H 4800 4350 50  0001 C CNN
F 3 "" H 4800 4350 50  0001 C CNN
	1    4800 4350
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 5B57EBCC
P 4800 4000
F 0 "R6" V 4880 4000 50  0000 C CNN
F 1 "3k3" V 4800 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4730 4000 50  0001 C CNN
F 3 "" H 4800 4000 50  0001 C CNN
	1    4800 4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR020
U 1 1 5B581753
P 5900 3500
F 0 "#PWR020" H 5900 3250 50  0001 C CNN
F 1 "GND" H 5900 3350 50  0000 C CNN
F 2 "" H 5900 3500 50  0001 C CNN
F 3 "" H 5900 3500 50  0001 C CNN
	1    5900 3500
	1    0    0    -1  
$EndComp
$Comp
L FRT5 K1
U 1 1 5B8A89A1
P 1850 6500
F 0 "K1" H 2700 6650 50  0000 L CNN
F 1 "P-5" H 2700 6550 50  0000 L CNN
F 2 "Relays_THT:Relay_DPDT_FRT5" H 2600 6550 50  0001 C CNN
F 3 "" H 2700 6650 50  0001 C CNN
	1    1850 6500
	1    0    0    -1  
$EndComp
$Comp
L D D1
U 1 1 5B8AFF68
P 1250 6500
F 0 "D1" H 1250 6600 50  0000 C CNN
F 1 "D" H 1250 6400 50  0000 C CNN
F 2 "Diodes_SMD:D_SMA" H 1250 6500 50  0001 C CNN
F 3 "" H 1250 6500 50  0001 C CNN
	1    1250 6500
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR021
U 1 1 5B8B1060
P 1650 6000
F 0 "#PWR021" H 1650 5850 50  0001 C CNN
F 1 "+5V" H 1650 6140 50  0000 C CNN
F 2 "" H 1650 6000 50  0001 C CNN
F 3 "" H 1650 6000 50  0001 C CNN
	1    1650 6000
	1    0    0    -1  
$EndComp
$Comp
L VDDA #PWR022
U 1 1 5B8B1750
P 2650 5850
F 0 "#PWR022" H 2650 5700 50  0001 C CNN
F 1 "VDDA" H 2650 6000 50  0000 C CNN
F 2 "" H 2650 5850 50  0001 C CNN
F 3 "" H 2650 5850 50  0001 C CNN
	1    2650 5850
	1    0    0    -1  
$EndComp
$Comp
L 2N3904 Q2
U 1 1 5B8B1FB0
P 3700 7150
F 0 "Q2" H 3450 7050 50  0000 L CNN
F 1 "2N3904" H 3450 6950 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Molded_Narrow" H 3900 7075 50  0001 L CIN
F 3 "" H 3700 7150 50  0001 L CNN
	1    3700 7150
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5B8B1FB6
P 3250 6900
F 0 "R2" V 3330 6900 50  0000 C CNN
F 1 "82" V 3250 6900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 3180 6900 50  0001 C CNN
F 3 "" H 3250 6900 50  0001 C CNN
	1    3250 6900
	1    0    0    -1  
$EndComp
$Comp
L FRT5 K2
U 1 1 5B8B1FBC
P 4000 6450
F 0 "K2" H 4850 6600 50  0000 L CNN
F 1 "P-5" H 4850 6500 50  0000 L CNN
F 2 "Relays_THT:Relay_DPDT_FRT5" H 4750 6500 50  0001 C CNN
F 3 "" H 4850 6600 50  0001 C CNN
	1    4000 6450
	1    0    0    -1  
$EndComp
$Comp
L D D2
U 1 1 5B8B1FC2
P 3400 6450
F 0 "D2" H 3400 6550 50  0000 C CNN
F 1 "D" H 3400 6350 50  0000 C CNN
F 2 "Diodes_SMD:D_SMA" H 3400 6450 50  0001 C CNN
F 3 "" H 3400 6450 50  0001 C CNN
	1    3400 6450
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR023
U 1 1 5B8B1FD8
P 3800 5750
F 0 "#PWR023" H 3800 5600 50  0001 C CNN
F 1 "+5V" H 3800 5890 50  0000 C CNN
F 2 "" H 3800 5750 50  0001 C CNN
F 3 "" H 3800 5750 50  0001 C CNN
	1    3800 5750
	1    0    0    -1  
$EndComp
$Comp
L VDDA #PWR024
U 1 1 5B8B1FE6
P 4800 5950
F 0 "#PWR024" H 4800 5800 50  0001 C CNN
F 1 "VDDA" H 4800 6100 50  0000 C CNN
F 2 "" H 4800 5950 50  0001 C CNN
F 3 "" H 4800 5950 50  0001 C CNN
	1    4800 5950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 5B8B394B
P 1650 7600
F 0 "#PWR025" H 1650 7350 50  0001 C CNN
F 1 "GND" H 1650 7450 50  0000 C CNN
F 2 "" H 1650 7600 50  0001 C CNN
F 3 "" H 1650 7600 50  0001 C CNN
	1    1650 7600
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW1
U 1 1 5B8C3F50
P 6050 5350
F 0 "SW1" H 6100 5450 50  0000 L CNN
F 1 "SW_Push" H 6050 5290 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_Tactile_SPST_Angled_PTS645Vx83-2LFS" H 6050 5550 50  0001 C CNN
F 3 "" H 6050 5550 50  0001 C CNN
	1    6050 5350
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW2
U 1 1 5B8C409C
P 6050 6150
F 0 "SW2" H 6100 6250 50  0000 L CNN
F 1 "SW_Push" H 6050 6090 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_Tactile_SPST_Angled_PTS645Vx83-2LFS" H 6050 6350 50  0001 C CNN
F 3 "" H 6050 6350 50  0001 C CNN
	1    6050 6150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 5B8C413D
P 6350 6550
F 0 "#PWR026" H 6350 6300 50  0001 C CNN
F 1 "GND" H 6350 6400 50  0000 C CNN
F 2 "" H 6350 6550 50  0001 C CNN
F 3 "" H 6350 6550 50  0001 C CNN
	1    6350 6550
	1    0    0    -1  
$EndComp
Text Label 5550 5350 0    60   ~ 0
FON
Text Label 5550 6150 0    60   ~ 0
FCL
Text Label 1750 6900 0    60   ~ 0
FON
Text Label 3900 6850 0    60   ~ 0
FCL
$Comp
L R R10
U 1 1 5B8F0FC5
P 5750 5550
F 0 "R10" V 5830 5550 50  0000 C CNN
F 1 "33k" V 5750 5550 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5680 5550 50  0001 C CNN
F 3 "" H 5750 5550 50  0001 C CNN
	1    5750 5550
	1    0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 5B8F1088
P 5750 5950
F 0 "R11" V 5830 5950 50  0000 C CNN
F 1 "33k" V 5750 5950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5680 5950 50  0001 C CNN
F 3 "" H 5750 5950 50  0001 C CNN
	1    5750 5950
	1    0    0    -1  
$EndComp
Text Label 5550 5750 0    60   ~ 0
A7
$Comp
L ATMEGA328P-AU U1
U 1 1 5B3E6B28
P 2050 4000
F 0 "U1" H 1300 5250 50  0000 L BNN
F 1 "ATMEGA328P-AU" H 2450 2600 50  0000 L BNN
F 2 "Housings_QFP:TQFP-32_7x7mm_Pitch0.8mm" H 2050 4000 50  0001 C CIN
F 3 "" H 2050 4000 50  0001 C CNN
	1    2050 4000
	1    0    0    -1  
$EndComp
Text Label 3100 3500 0    60   ~ 0
O1
Text Label 3100 3600 0    60   ~ 0
O2
Text Label 3100 4800 0    60   ~ 0
WCR
$Comp
L GS2 J16
U 1 1 5B490401
P 10800 2550
F 0 "J16" H 10900 2700 50  0000 C CNN
F 1 "GS2" H 10900 2401 50  0000 C CNN
F 2 "Connectors:GS2" V 10874 2550 50  0001 C CNN
F 3 "" H 10800 2550 50  0001 C CNN
	1    10800 2550
	-1   0    0    1   
$EndComp
Text Label 6750 7250 0    60   ~ 0
A6
$Comp
L Crystal Y1
U 1 1 5B93E413
P 7800 2900
F 0 "Y1" H 7800 3050 50  0000 C CNN
F 1 "16MHz" H 7800 2750 50  0000 C CNN
F 2 "Crystals:Crystal_HC49-U_Vertical" H 7800 2900 50  0001 C CNN
F 3 "" H 7800 2900 50  0001 C CNN
	1    7800 2900
	1    0    0    -1  
$EndComp
$Comp
L R R16
U 1 1 5B93E75F
P 7800 2650
F 0 "R16" V 7880 2650 50  0000 C CNN
F 1 "1M" V 7800 2650 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 7730 2650 50  0001 C CNN
F 3 "" H 7800 2650 50  0001 C CNN
	1    7800 2650
	0    -1   -1   0   
$EndComp
$Comp
L C C10
U 1 1 5B93EC0F
P 8050 3150
F 0 "C10" H 8075 3250 50  0000 L CNN
F 1 "22p" H 8075 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 8088 3000 50  0001 C CNN
F 3 "" H 8050 3150 50  0001 C CNN
	1    8050 3150
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 5B93ECE4
P 7550 3150
F 0 "C9" H 7575 3250 50  0000 L CNN
F 1 "22p" H 7575 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7588 3000 50  0001 C CNN
F 3 "" H 7550 3150 50  0001 C CNN
	1    7550 3150
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR027
U 1 1 5B93F7C9
P 7800 3400
F 0 "#PWR027" H 7800 3150 50  0001 C CNN
F 1 "GND" H 7800 3250 50  0000 C CNN
F 2 "" H 7800 3400 50  0001 C CNN
F 3 "" H 7800 3400 50  0001 C CNN
	1    7800 3400
	1    0    0    -1  
$EndComp
Text Label 7300 2650 0    60   ~ 0
O1
Text Label 8200 2650 0    60   ~ 0
O2
$Comp
L Conn_02x03_Odd_Even J8
U 1 1 5B94199C
P 8300 4400
F 0 "J8" H 8350 4600 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 8350 4200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 8300 4400 50  0001 C CNN
F 3 "" H 8300 4400 50  0001 C CNN
	1    8300 4400
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR028
U 1 1 5B9422A8
P 8600 4200
F 0 "#PWR028" H 8600 4050 50  0001 C CNN
F 1 "VCC" H 8600 4350 50  0000 C CNN
F 2 "" H 8600 4200 50  0001 C CNN
F 3 "" H 8600 4200 50  0001 C CNN
	1    8600 4200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR029
U 1 1 5B945BC5
P 3650 5350
F 0 "#PWR029" H 3650 5100 50  0001 C CNN
F 1 "GND" H 3650 5200 50  0000 C CNN
F 2 "" H 3650 5350 50  0001 C CNN
F 3 "" H 3650 5350 50  0001 C CNN
	1    3650 5350
	1    0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 5B9467C5
P 1950 950
F 0 "C2" H 1975 1050 50  0000 L CNN
F 1 "100" H 1975 850 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.7" H 1988 800 50  0001 C CNN
F 3 "" H 1950 950 50  0001 C CNN
	1    1950 950 
	1    0    0    -1  
$EndComp
$Comp
L CP C3
U 1 1 5B946E29
P 3050 950
F 0 "C3" H 3075 1050 50  0000 L CNN
F 1 "47" H 3075 850 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.7" H 3088 800 50  0001 C CNN
F 3 "" H 3050 950 50  0001 C CNN
	1    3050 950 
	1    0    0    -1  
$EndComp
$Comp
L CP C4
U 1 1 5B946F9D
P 3050 1900
F 0 "C4" H 3075 2000 50  0000 L CNN
F 1 "100" H 3075 1800 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.7" H 3088 1750 50  0001 C CNN
F 3 "" H 3050 1900 50  0001 C CNN
	1    3050 1900
	1    0    0    -1  
$EndComp
$Comp
L CP C6
U 1 1 5B9470E1
P 4050 1900
F 0 "C6" H 4075 2000 50  0000 L CNN
F 1 "47" H 4075 1800 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.7" H 4088 1750 50  0001 C CNN
F 3 "" H 4050 1900 50  0001 C CNN
	1    4050 1900
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 5B94F8F1
P 4300 3700
F 0 "R4" V 4380 3700 50  0000 C CNN
F 1 "1k5" V 4300 3700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4230 3700 50  0001 C CNN
F 3 "" H 4300 3700 50  0001 C CNN
	1    4300 3700
	0    1    1    0   
$EndComp
Text Label 3700 3250 0    60   ~ 0
WCR
Wire Wire Line
	1100 2800 1100 3500
Connection ~ 1100 3000
Wire Wire Line
	1100 5000 1100 5350
Wire Wire Line
	1100 5200 1150 5200
Connection ~ 5900 2950
Wire Wire Line
	5900 2950 5900 3000
Wire Wire Line
	3950 3250 3650 3250
Wire Wire Line
	3950 3700 3950 3250
Wire Wire Line
	4150 3700 3950 3700
Connection ~ 4800 3700
Wire Wire Line
	4450 3700 4800 3700
Connection ~ 4650 3500
Wire Wire Line
	4650 3850 4650 3500
Wire Wire Line
	4650 4250 4800 4250
Wire Wire Line
	4650 4150 4650 4250
Connection ~ 4800 4250
Wire Wire Line
	4950 4250 5750 4250
Wire Wire Line
	3050 5000 3750 5000
Wire Wire Line
	3050 4900 3750 4900
Wire Wire Line
	8600 4300 8600 4200
Wire Wire Line
	8500 4300 8600 4300
Connection ~ 7800 3300
Wire Wire Line
	7800 3300 7800 3400
Wire Wire Line
	7550 3300 8050 3300
Connection ~ 8050 2650
Connection ~ 8050 2900
Wire Wire Line
	7950 2900 8050 2900
Wire Wire Line
	8050 2650 8050 3000
Wire Wire Line
	7950 2650 8350 2650
Connection ~ 7550 2900
Wire Wire Line
	7650 2900 7550 2900
Connection ~ 7550 2650
Wire Wire Line
	7550 2650 7550 3000
Wire Wire Line
	7250 2650 7650 2650
Wire Wire Line
	1250 2100 4350 2100
Connection ~ 3400 750 
Wire Wire Line
	3400 750  3400 550 
Wire Wire Line
	3400 550  2150 550 
Wire Wire Line
	2150 550  2150 1700
Connection ~ 3700 1150
Wire Wire Line
	3550 1150 3700 1150
Wire Wire Line
	8600 4500 8600 4600
Wire Wire Line
	8500 4500 8600 4500
Wire Wire Line
	10800 2750 10800 2850
Wire Wire Line
	5000 700  5900 700 
Wire Wire Line
	4050 3150 4050 3500
Wire Wire Line
	4050 3500 4150 3500
Wire Wire Line
	3050 3600 3250 3600
Wire Wire Line
	3250 3500 3050 3500
Connection ~ 5750 5750
Wire Wire Line
	5750 5750 5500 5750
Wire Wire Line
	5750 5700 5750 5800
Connection ~ 5750 6150
Wire Wire Line
	5750 6100 5750 6150
Connection ~ 5750 5350
Wire Wire Line
	5750 5350 5750 5400
Wire Wire Line
	6350 5350 6350 6550
Wire Wire Line
	5500 5350 5850 5350
Wire Wire Line
	5500 6150 5850 6150
Connection ~ 6350 6150
Wire Wire Line
	6250 6150 6800 6150
Wire Wire Line
	6250 5350 6350 5350
Wire Wire Line
	6300 6950 6300 7450
Wire Wire Line
	4600 6950 6300 6950
Wire Wire Line
	4600 6750 4600 6950
Wire Wire Line
	4200 7050 4200 6750
Wire Wire Line
	5350 7050 4200 7050
Wire Wire Line
	5350 7450 5350 7050
Wire Wire Line
	6100 7650 6300 7650
Wire Wire Line
	6100 7150 6100 7650
Wire Wire Line
	5150 7150 6100 7150
Wire Wire Line
	5150 7450 5150 7150
Wire Wire Line
	2450 7450 5150 7450
Wire Wire Line
	2450 6800 2450 7450
Wire Wire Line
	2050 7650 2050 6800
Wire Wire Line
	5350 7650 2050 7650
Wire Wire Line
	6200 7550 6300 7550
Wire Wire Line
	6200 7250 6200 7550
Wire Wire Line
	5250 7250 6900 7250
Wire Wire Line
	5250 6050 5250 7550
Connection ~ 1650 7550
Wire Wire Line
	1650 7400 1650 7600
Wire Wire Line
	3800 7550 3800 7350
Wire Wire Line
	1650 7550 3800 7550
Wire Wire Line
	5250 7550 5350 7550
Connection ~ 4700 5950
Wire Wire Line
	1950 6050 5250 6050
Wire Wire Line
	4500 6150 4500 6050
Wire Wire Line
	4700 5950 4700 6150
Wire Wire Line
	4300 5950 4800 5950
Wire Wire Line
	4300 6150 4300 5950
Wire Wire Line
	3800 5750 3800 6150
Wire Wire Line
	3400 5800 3400 6300
Connection ~ 3800 6850
Wire Wire Line
	3400 6850 3400 6600
Wire Wire Line
	3400 6850 4050 6850
Wire Wire Line
	3800 6750 3800 6950
Wire Wire Line
	3250 5100 3250 6750
Wire Wire Line
	3250 7150 3250 7050
Wire Wire Line
	3500 7150 3250 7150
Connection ~ 2550 5850
Connection ~ 1650 6100
Wire Wire Line
	2550 5850 2550 6200
Wire Wire Line
	2150 5850 2650 5850
Wire Wire Line
	2150 5850 2150 6200
Wire Wire Line
	1650 6000 1650 6200
Wire Wire Line
	1250 6100 1650 6100
Wire Wire Line
	1250 6350 1250 6100
Connection ~ 1650 6900
Wire Wire Line
	1250 6900 1250 6650
Wire Wire Line
	1250 6900 1900 6900
Wire Wire Line
	1650 6800 1650 7000
Wire Wire Line
	1100 5600 1100 6800
Wire Wire Line
	3150 5600 1100 5600
Wire Wire Line
	3150 5200 3150 5600
Wire Wire Line
	1100 7200 1100 7100
Wire Wire Line
	1350 7200 1100 7200
Wire Wire Line
	5450 1150 5450 1500
Wire Wire Line
	4700 3050 3650 3050
Wire Wire Line
	4700 3400 4700 3050
Wire Wire Line
	5000 3400 4700 3400
Wire Wire Line
	3650 3150 4050 3150
Wire Wire Line
	4450 3500 5000 3500
Wire Wire Line
	5800 2950 6100 2950
Connection ~ 5250 4250
Wire Wire Line
	5250 4100 5250 4250
Wire Wire Line
	4800 4150 4800 4350
Connection ~ 5550 4250
Wire Wire Line
	5750 4250 5750 4150
Wire Wire Line
	5550 4250 5550 4150
Wire Wire Line
	5750 3500 5750 3850
Wire Wire Line
	5500 3500 5750 3500
Wire Wire Line
	5550 3600 5550 3850
Wire Wire Line
	5500 3600 5550 3600
Wire Wire Line
	4800 3600 4800 3850
Wire Wire Line
	5000 3600 4800 3600
Wire Wire Line
	4950 3700 4950 4250
Wire Wire Line
	5000 3700 4950 3700
Connection ~ 5900 3400
Wire Wire Line
	5500 3400 5900 3400
Wire Wire Line
	5900 3300 5900 3500
Wire Wire Line
	3650 2950 5500 2950
Wire Wire Line
	10250 5700 10300 5700
Wire Wire Line
	3050 4700 3250 4700
Wire Wire Line
	3050 4600 3250 4600
Wire Wire Line
	3050 4500 3250 4500
Wire Wire Line
	3050 4800 3250 4800
Wire Wire Line
	3150 5200 3050 5200
Wire Wire Line
	3250 5100 3050 5100
Wire Wire Line
	6100 2950 6100 3700
Wire Wire Line
	8900 1300 8900 1400
Wire Wire Line
	9000 1300 8900 1300
Wire Wire Line
	10250 4200 10250 5900
Wire Wire Line
	10800 700  10800 2350
Wire Wire Line
	3700 1050 3700 1400
Wire Wire Line
	3700 1400 3050 1400
Connection ~ 3050 1150
Wire Wire Line
	3050 4350 3250 4350
Wire Wire Line
	950  4350 1150 4350
Wire Wire Line
	5700 1050 5900 1050
Wire Wire Line
	5000 1050 5200 1050
Wire Wire Line
	4200 1050 3700 1050
Connection ~ 4050 2100
Wire Wire Line
	4350 2100 4350 2050
Wire Wire Line
	4350 1700 4350 1750
Connection ~ 1600 1150
Wire Wire Line
	1600 1100 1600 1150
Connection ~ 1600 750 
Wire Wire Line
	1600 800  1600 750 
Connection ~ 3550 2100
Wire Wire Line
	3550 2100 3550 2000
Wire Wire Line
	2900 750  4200 750 
Wire Wire Line
	4200 1350 4050 1350
Connection ~ 4050 1700
Connection ~ 3050 750 
Wire Wire Line
	3050 750  3050 800 
Connection ~ 2600 1150
Wire Wire Line
	3050 1150 3050 1100
Connection ~ 3050 1700
Wire Wire Line
	3050 1400 3050 1750
Wire Wire Line
	4050 1350 4050 1750
Connection ~ 3050 2100
Wire Wire Line
	4050 2100 4050 2050
Connection ~ 2600 2100
Wire Wire Line
	3050 2100 3050 2050
Connection ~ 1250 2100
Wire Wire Line
	2600 2000 2600 2100
Connection ~ 1950 750 
Wire Wire Line
	1950 750  1950 800 
Connection ~ 1950 1150
Wire Wire Line
	1950 1150 1950 1100
Wire Wire Line
	3850 1700 4350 1700
Connection ~ 1250 1150
Wire Wire Line
	1250 1150 3250 1150
Wire Wire Line
	2600 1150 2600 1050
Connection ~ 1250 950 
Wire Wire Line
	1050 950  1250 950 
Wire Wire Line
	1250 850  1250 2200
Wire Wire Line
	1050 850  1250 850 
Wire Wire Line
	2900 1700 3250 1700
Wire Wire Line
	2150 1700 2300 1700
Wire Wire Line
	1050 750  2300 750 
Wire Wire Line
	8750 700  9000 700 
Wire Wire Line
	8750 800  9000 800 
Wire Wire Line
	8750 900  9000 900 
Wire Wire Line
	8750 1000 9000 1000
Wire Wire Line
	8750 1100 9000 1100
Wire Wire Line
	8750 1200 9000 1200
Wire Wire Line
	7800 4400 8000 4400
Wire Wire Line
	8700 4400 8500 4400
Wire Wire Line
	7800 4300 8000 4300
Connection ~ 7350 4500
Connection ~ 7100 4900
Wire Wire Line
	7350 4900 7100 4900
Wire Wire Line
	7350 4800 7350 4900
Wire Wire Line
	6850 4500 8000 4500
Connection ~ 10250 4500
Wire Wire Line
	10300 4200 10250 4200
Connection ~ 10250 4800
Wire Wire Line
	10300 4500 10250 4500
Connection ~ 10250 5100
Wire Wire Line
	10250 4800 10300 4800
Connection ~ 10250 5400
Wire Wire Line
	10250 5100 10300 5100
Connection ~ 10250 5700
Wire Wire Line
	10250 5400 10300 5400
Connection ~ 7100 4500
Wire Wire Line
	7100 4850 7100 5000
Wire Wire Line
	7100 3950 7100 4100
Wire Wire Line
	7100 4400 7100 4550
Wire Wire Line
	1150 5000 1100 5000
Wire Wire Line
	1100 5100 1150 5100
Connection ~ 1100 5100
Connection ~ 1100 5200
Wire Wire Line
	1100 3000 1150 3000
Wire Wire Line
	1150 2900 1100 2900
Connection ~ 1100 2900
Wire Wire Line
	1100 3200 1150 3200
Wire Wire Line
	1100 3500 1150 3500
Connection ~ 1100 3200
Wire Wire Line
	3050 3750 3250 3750
Wire Wire Line
	3050 3850 3250 3850
Wire Wire Line
	3050 3950 3250 3950
Wire Wire Line
	3050 4050 3250 4050
Wire Wire Line
	3050 4150 3250 4150
Wire Wire Line
	3050 4250 3250 4250
Wire Wire Line
	950  4250 1150 4250
Wire Wire Line
	9800 4100 10300 4100
Wire Wire Line
	9800 4400 10300 4400
Wire Wire Line
	9800 4700 10300 4700
Wire Wire Line
	9800 5000 10300 5000
Wire Wire Line
	9800 5300 10300 5300
Wire Wire Line
	9800 5600 10300 5600
Wire Wire Line
	10700 700  10800 700 
Wire Wire Line
	10700 1000 10800 1000
Connection ~ 10800 1000
Wire Wire Line
	10800 1300 10700 1300
Connection ~ 10800 1300
Wire Wire Line
	10800 1600 10700 1600
Connection ~ 10800 1600
Wire Wire Line
	10800 1900 10700 1900
Connection ~ 10800 1900
Wire Wire Line
	10800 2200 10700 2200
Connection ~ 10800 2200
Wire Wire Line
	10300 700  10400 700 
Wire Wire Line
	10300 1000 10400 1000
Wire Wire Line
	10300 1300 10400 1300
Wire Wire Line
	10300 1600 10400 1600
Wire Wire Line
	10300 1900 10400 1900
Wire Wire Line
	10300 2200 10400 2200
Wire Wire Line
	9800 2200 10000 2200
Wire Wire Line
	9800 1900 10000 1900
Wire Wire Line
	9800 1600 10000 1600
Wire Wire Line
	9800 1300 10000 1300
Wire Wire Line
	9800 1000 10000 1000
Wire Wire Line
	9800 700  10000 700 
Wire Wire Line
	3050 2900 3250 2900
Wire Wire Line
	3050 3000 3250 3000
Wire Wire Line
	3050 3100 3250 3100
Wire Wire Line
	3050 3200 3250 3200
Wire Wire Line
	3050 3300 3250 3300
Wire Wire Line
	3050 3400 3250 3400
Wire Wire Line
	6100 3700 5500 3700
$Comp
L R R13
U 1 1 5B97E621
P 6600 6400
F 0 "R13" V 6680 6400 50  0000 C CNN
F 1 "0.5" V 6600 6400 50  0000 C CNN
F 2 "Resistors_SMD:R_2010" V 6530 6400 50  0001 C CNN
F 3 "" H 6600 6400 50  0001 C CNN
	1    6600 6400
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 5B97E6DA
P 6800 6400
F 0 "R14" V 6880 6400 50  0000 C CNN
F 1 "0.5" V 6800 6400 50  0000 C CNN
F 2 "Resistors_SMD:R_2010" V 6730 6400 50  0001 C CNN
F 3 "" H 6800 6400 50  0001 C CNN
	1    6800 6400
	1    0    0    -1  
$EndComp
Connection ~ 6200 7250
Wire Wire Line
	6600 6550 6600 7250
Connection ~ 6600 7250
Wire Wire Line
	6600 6650 6800 6650
Wire Wire Line
	6800 6650 6800 6550
Connection ~ 6600 6650
Wire Wire Line
	6600 6150 6600 6250
Wire Wire Line
	6800 6150 6800 6250
Connection ~ 6600 6150
Wire Wire Line
	4100 6050 4100 6150
Connection ~ 5250 7250
Connection ~ 4500 6050
Wire Wire Line
	3400 5800 3800 5800
Connection ~ 3800 5800
Wire Wire Line
	1950 6050 1950 6200
Connection ~ 4100 6050
Wire Wire Line
	2350 6200 2350 6050
Connection ~ 2350 6050
$Comp
L Conn_01x04 J2
U 1 1 5B9C22A1
P 3950 5100
F 0 "J2" H 3950 5300 50  0000 C CNN
F 1 "Conn_01x04" H 3950 4800 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x04_Pitch2.54mm" H 3950 5100 50  0001 C CNN
F 3 "" H 3950 5100 50  0001 C CNN
	1    3950 5100
	1    0    0    1   
$EndComp
Wire Wire Line
	3750 5200 3650 5200
Wire Wire Line
	3650 5200 3650 5350
Wire Wire Line
	3750 5100 3650 5100
Wire Wire Line
	3650 5100 3650 4700
$Comp
L +5V #PWR030
U 1 1 5B9C3AA2
P 3650 4700
F 0 "#PWR030" H 3650 4550 50  0001 C CNN
F 1 "+5V" H 3650 4840 50  0000 C CNN
F 2 "" H 3650 4700 50  0001 C CNN
F 3 "" H 3650 4700 50  0001 C CNN
	1    3650 4700
	1    0    0    -1  
$EndComp
$EndSCHEMATC
