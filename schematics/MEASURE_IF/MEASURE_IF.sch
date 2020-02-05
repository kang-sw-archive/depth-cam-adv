EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Depscan Interface Board"
Date "2020-02-03"
Rev "0.9"
Comp "KOREATECH"
Comment1 "@Author Seungwoo Kang"
Comment2 "Copyright 2020. Seungwoo Kang. All rights reserved"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:USB_B_Micro J7
U 1 1 5E381C4B
P 8550 1400
F 0 "J7" H 8607 1867 50  0000 C CNN
F 1 "USB_B_Micro" H 8607 1776 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Molex_47346-0001" H 8700 1350 50  0001 C CNN
F 3 "~" H 8700 1350 50  0001 C CNN
	1    8550 1400
	1    0    0    -1  
$EndComp
$Comp
L Power_Protection:USBLC6-2SC6 U3
U 1 1 5E383128
P 10050 1650
F 0 "U3" V 10250 2000 50  0000 L CNN
F 1 "USBLC6-2SC6" V 10350 2000 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 9300 2050 50  0001 C CNN
F 3 "http://www2.st.com/resource/en/datasheet/CD00050750.pdf" H 10250 2000 50  0001 C CNN
	1    10050 1650
	0    1    1    0   
$EndComp
NoConn ~ 8450 1800
NoConn ~ 8850 1600
Wire Wire Line
	9950 1150 9950 1100
Wire Wire Line
	9950 1100 9050 1100
Wire Wire Line
	9050 1100 9050 1400
Wire Wire Line
	9050 1400 8850 1400
Wire Wire Line
	8850 1500 9050 1500
Wire Wire Line
	9050 1500 9050 2200
Wire Wire Line
	9050 2200 9950 2200
Wire Wire Line
	9950 2200 9950 2150
Wire Wire Line
	10150 2150 10150 2200
Wire Wire Line
	10150 2200 10550 2200
Wire Wire Line
	10150 1150 10150 1100
Wire Wire Line
	10150 1100 10550 1100
Text Label 10550 1100 2    50   ~ 0
USB_DP
Text Label 10550 2200 2    50   ~ 0
USB_DM
Text Label 4550 6200 2    50   ~ 0
USB_DP
Wire Wire Line
	4550 6200 4100 6200
Text Label 4550 6100 2    50   ~ 0
USB_DM
Wire Wire Line
	4550 6100 4100 6100
$Comp
L power:GND #PWR036
U 1 1 5E38F234
P 9400 1850
F 0 "#PWR036" H 9400 1600 50  0001 C CNN
F 1 "GND" H 9405 1677 50  0000 C CNN
F 2 "" H 9400 1850 50  0001 C CNN
F 3 "" H 9400 1850 50  0001 C CNN
	1    9400 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 1850 9400 1650
Wire Wire Line
	9400 1650 9550 1650
Text Label 2550 5600 0    50   ~ 0
OSC_IN
Text Label 2550 5700 0    50   ~ 0
OSC_OUT
Wire Wire Line
	2650 5000 2750 5000
$Comp
L Device:R_US R1
U 1 1 5E397687
P 2750 4800
F 0 "R1" H 2818 4846 50  0000 L CNN
F 1 "22k" H 2818 4755 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2790 4790 50  0001 C CNN
F 3 "~" H 2750 4800 50  0001 C CNN
	1    2750 4800
	1    0    0    -1  
$EndComp
Connection ~ 2750 5000
Wire Wire Line
	2750 5000 3000 5000
Wire Wire Line
	2750 5000 2750 4950
$Comp
L power:GND #PWR05
U 1 1 5E3992B9
P 2150 5100
F 0 "#PWR05" H 2150 4850 50  0001 C CNN
F 1 "GND" H 2155 4927 50  0000 C CNN
F 2 "" H 2150 5100 50  0001 C CNN
F 3 "" H 2150 5100 50  0001 C CNN
	1    2150 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 5100 2150 5000
Wire Wire Line
	2150 5000 2250 5000
Text Label 2550 5200 0    50   ~ 0
BOOT0
Wire Wire Line
	2550 5200 3000 5200
$Comp
L MCU_ST_STM32F3:STM32G431KBTx U2
U 1 1 5E3B8CB5
P 3600 5700
F 0 "U2" H 3850 6650 50  0000 C CNN
F 1 "STM32G431KBTx" H 4100 6550 50  0000 C CNN
F 2 "Package_QFP:LQFP-32_7x7mm_P0.8mm" H 3100 4800 50  0001 R CNN
F 3 "https://www.st.com/resource/en/datasheet/DM00507199.pdf" H 3600 5700 50  0001 C CNN
	1    3600 5700
	1    0    0    -1  
$EndComp
Text Label 4550 6300 2    50   ~ 0
SWCLK
Text Label 4550 6400 2    50   ~ 0
SWDIO
Wire Wire Line
	4100 6400 4550 6400
Wire Wire Line
	4100 6300 4550 6300
Text Label 2550 6100 0    50   ~ 0
JTAG_SWO
$Comp
L power:GND #PWR010
U 1 1 5E3C2442
P 3500 7000
F 0 "#PWR010" H 3500 6750 50  0001 C CNN
F 1 "GND" H 3505 6827 50  0000 C CNN
F 2 "" H 3500 7000 50  0001 C CNN
F 3 "" H 3500 7000 50  0001 C CNN
	1    3500 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 6700 3500 6900
Wire Wire Line
	3500 6900 3600 6900
Connection ~ 3500 6900
Wire Wire Line
	3500 6900 3500 7000
Wire Wire Line
	3600 6700 3600 6900
Wire Wire Line
	3500 4800 3500 4650
Wire Wire Line
	3500 4650 3600 4650
Wire Wire Line
	3600 4800 3600 4650
Wire Wire Line
	3500 4650 3500 4550
Connection ~ 3500 4650
Wire Wire Line
	8950 1200 8850 1200
Wire Wire Line
	8550 1800 8550 1950
$Comp
L power:GND #PWR031
U 1 1 5E3CECB4
P 8550 1950
F 0 "#PWR031" H 8550 1700 50  0001 C CNN
F 1 "GND" H 8555 1777 50  0000 C CNN
F 2 "" H 8550 1950 50  0001 C CNN
F 3 "" H 8550 1950 50  0001 C CNN
	1    8550 1950
	1    0    0    -1  
$EndComp
$Comp
L Driver_Motor:Pololu_Breakout_DRV8825 A2
U 1 1 5E3CF2A9
P 9800 4400
F 0 "A2" H 9900 5050 50  0000 C CNN
F 1 "DRV8825_MOT1" H 10100 4950 50  0000 C CNN
F 2 "Module:Pololu_Breakout-16_15.2x20.3mm" H 10000 3600 50  0001 L CNN
F 3 "https://www.pololu.com/product/2982" H 9900 4100 50  0001 C CNN
	1    9800 4400
	1    0    0    -1  
$EndComp
$Comp
L Driver_Motor:Pololu_Breakout_DRV8825 A1
U 1 1 5E3D332B
P 8100 4400
F 0 "A1" H 8000 5050 50  0000 C CNN
F 1 "DRV8825_MOT2" H 7500 4950 50  0000 L CNN
F 2 "Module:Pololu_Breakout-16_15.2x20.3mm" H 8300 3600 50  0001 L CNN
F 3 "https://www.pololu.com/product/2982" H 8200 4100 50  0001 C CNN
	1    8100 4400
	-1   0    0    -1  
$EndComp
Text Label 4550 5800 2    50   ~ 0
MOT1_STEP
Text Label 2550 6200 0    50   ~ 0
MOT2_STEP
Wire Wire Line
	2550 6200 3000 6200
Wire Wire Line
	4100 5800 4550 5800
Text Label 2550 6300 0    50   ~ 0
MOT_DIR_1
Text Label 2550 6400 0    50   ~ 0
MOT_DIR_2
Wire Wire Line
	2550 6400 3000 6400
Wire Wire Line
	2550 6300 3000 6300
NoConn ~ 3000 6500
NoConn ~ 4100 6000
NoConn ~ 4100 5900
NoConn ~ 3000 5900
Wire Wire Line
	2550 6100 3000 6100
NoConn ~ 4100 6500
Text Label 4550 5000 2    50   ~ 0
ARGUS_CS
Text Label 4550 5100 2    50   ~ 0
ARGUS_IRQ
Wire Wire Line
	4550 5100 4100 5100
Wire Wire Line
	4100 5000 4550 5000
$Comp
L power:VDD #PWR09
U 1 1 5E3E26F4
P 3500 4550
F 0 "#PWR09" H 3500 4400 50  0001 C CNN
F 1 "VDD" H 3517 4723 50  0000 C CNN
F 2 "" H 3500 4550 50  0001 C CNN
F 3 "" H 3500 4550 50  0001 C CNN
	1    3500 4550
	1    0    0    -1  
$EndComp
$Comp
L power:VDDA #PWR011
U 1 1 5E3E38A1
P 3700 4550
F 0 "#PWR011" H 3700 4400 50  0001 C CNN
F 1 "VDDA" H 3717 4723 50  0000 C CNN
F 2 "" H 3700 4550 50  0001 C CNN
F 3 "" H 3700 4550 50  0001 C CNN
	1    3700 4550
	1    0    0    -1  
$EndComp
$Comp
L power:VSSA #PWR012
U 1 1 5E3E66AA
P 3700 7000
F 0 "#PWR012" H 3700 6850 50  0001 C CNN
F 1 "VSSA" H 3718 7173 50  0000 C CNN
F 2 "" H 3700 7000 50  0001 C CNN
F 3 "" H 3700 7000 50  0001 C CNN
	1    3700 7000
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 4650 2750 4550
$Comp
L power:VDD #PWR07
U 1 1 5E40A444
P 2750 4550
F 0 "#PWR07" H 2750 4400 50  0001 C CNN
F 1 "VDD" H 2767 4723 50  0000 C CNN
F 2 "" H 2750 4550 50  0001 C CNN
F 3 "" H 2750 4550 50  0001 C CNN
	1    2750 4550
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:LD3985M33R_SOT23 U1
U 1 1 5E41D838
P 3100 1200
F 0 "U1" H 3100 1542 50  0000 C CNN
F 1 "LD3985M33R_SOT23" H 3100 1451 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 3100 1525 50  0001 C CIN
F 3 "http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00003395.pdf" H 3100 1200 50  0001 C CNN
	1    3100 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 1200 8950 950 
Wire Wire Line
	8950 950  9200 950 
Text Label 9200 950  2    50   ~ 0
VBUS
$Comp
L Device:D_Schottky_Small D2
U 1 1 5E4297D4
P 1750 1200
F 0 "D2" H 1850 1150 50  0000 C CNN
F 1 "BAT60JFILM" H 1500 1150 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" V 1750 1200 50  0001 C CNN
F 3 "~" V 1750 1200 50  0001 C CNN
	1    1750 1200
	-1   0    0    1   
$EndComp
$Comp
L Device:D_Schottky_Small D1
U 1 1 5E42FD0C
P 1750 1000
F 0 "D1" H 1850 950 50  0000 C CNN
F 1 "BAT60JFILM" H 1500 950 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" V 1750 1000 50  0001 C CNN
F 3 "~" V 1750 1000 50  0001 C CNN
	1    1750 1000
	-1   0    0    1   
$EndComp
Wire Wire Line
	1850 1000 2250 1000
Wire Wire Line
	2250 1000 2250 1100
Wire Wire Line
	1850 1200 2250 1200
Connection ~ 2250 1100
Text Label 1100 1200 0    50   ~ 0
VBUS
Text Label 1100 1000 0    50   ~ 0
STLINK_VIN
Wire Wire Line
	4050 1100 3400 1100
Wire Wire Line
	2250 1100 2250 1200
Wire Wire Line
	2250 1100 2400 1100
Wire Wire Line
	2800 1200 2650 1200
Connection ~ 2650 1100
Wire Wire Line
	2650 1100 2800 1100
Wire Wire Line
	10800 1650 10550 1650
Text Label 10800 1650 2    50   ~ 0
VBUS
NoConn ~ 9400 4000
Wire Wire Line
	8500 4100 8950 4100
Wire Wire Line
	9400 4200 8950 4200
NoConn ~ 8500 4000
Wire Wire Line
	8950 4200 8950 4100
Connection ~ 8950 4200
Wire Wire Line
	8950 4200 8500 4200
Connection ~ 8950 4100
Wire Wire Line
	8950 4100 9400 4100
Wire Wire Line
	9400 4400 8950 4400
Wire Wire Line
	8950 4400 8950 5550
Connection ~ 8950 4400
Wire Wire Line
	8950 4400 8500 4400
Text Label 4550 5500 2    50   ~ 0
SPI_SCK
Text Label 4550 5600 2    50   ~ 0
SPI_MISO
Text Label 4550 5700 2    50   ~ 0
SPI_MOSI
Wire Wire Line
	4550 5700 4100 5700
Wire Wire Line
	4100 5600 4550 5600
Wire Wire Line
	4550 5500 4100 5500
NoConn ~ 4100 5200
Wire Wire Line
	4550 5400 4100 5400
Wire Wire Line
	4100 5300 4550 5300
Text Label 8600 5550 1    50   ~ 0
MOTuCONF1
Wire Wire Line
	8600 4800 8500 4800
Wire Wire Line
	8500 4900 8600 4900
Connection ~ 8600 4900
Wire Wire Line
	8600 4900 8600 4800
Wire Wire Line
	8500 5000 8600 5000
Wire Wire Line
	8600 5000 8600 4900
Text Label 9300 5550 1    50   ~ 0
MOTuCONF2
Wire Wire Line
	9300 4800 9400 4800
Wire Wire Line
	9400 4900 9300 4900
Connection ~ 9300 4900
Wire Wire Line
	9300 4900 9300 4800
Wire Wire Line
	9400 5000 9300 5000
Wire Wire Line
	9300 5000 9300 4900
$Comp
L Connector:Screw_Terminal_01x02 J5
U 1 1 5E4AFADA
P 7500 4300
F 0 "J5" H 7600 4300 50  0000 C CNN
F 1 "MOT_TERM" H 7750 4200 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 7500 4300 50  0001 C CNN
F 3 "~" H 7500 4300 50  0001 C CNN
	1    7500 4300
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J6
U 1 1 5E4B2B95
P 7500 4600
F 0 "J6" H 7600 4600 50  0000 C CNN
F 1 "MOT_TERM" H 7750 4500 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 7500 4600 50  0001 C CNN
F 3 "~" H 7500 4600 50  0001 C CNN
	1    7500 4600
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J9
U 1 1 5E4B2F07
P 10400 4600
F 0 "J9" H 10500 4600 50  0000 C CNN
F 1 "MOT_TERM" H 10650 4500 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 10400 4600 50  0001 C CNN
F 3 "~" H 10400 4600 50  0001 C CNN
	1    10400 4600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J8
U 1 1 5E4B585D
P 10400 4300
F 0 "J8" H 10500 4300 50  0000 C CNN
F 1 "MOT_TERM" H 10650 4200 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 10400 4300 50  0001 C CNN
F 3 "~" H 10400 4300 50  0001 C CNN
	1    10400 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR034
U 1 1 5E4B5DB9
P 8950 5550
F 0 "#PWR034" H 8950 5300 50  0001 C CNN
F 1 "GND" H 8955 5377 50  0000 C CNN
F 2 "" H 8950 5550 50  0001 C CNN
F 3 "" H 8950 5550 50  0001 C CNN
	1    8950 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR030
U 1 1 5E4B62AF
P 8100 5550
F 0 "#PWR030" H 8100 5300 50  0001 C CNN
F 1 "GND" H 8105 5377 50  0000 C CNN
F 2 "" H 8100 5550 50  0001 C CNN
F 3 "" H 8100 5550 50  0001 C CNN
	1    8100 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR037
U 1 1 5E4B6605
P 9800 5550
F 0 "#PWR037" H 9800 5300 50  0001 C CNN
F 1 "GND" H 9805 5377 50  0000 C CNN
F 2 "" H 9800 5550 50  0001 C CNN
F 3 "" H 9800 5550 50  0001 C CNN
	1    9800 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 5200 10000 5400
Wire Wire Line
	7900 5450 8000 5200
$Comp
L Device:CP1_Small C5
U 1 1 5E4C3E62
P 2400 1400
F 0 "C5" H 2400 1500 50  0000 L CNN
F 1 "1uF/25V/X5R" H 1850 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2400 1400 50  0001 C CNN
F 3 "~" H 2400 1400 50  0001 C CNN
	1    2400 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 1300 2400 1100
Connection ~ 2400 1100
Wire Wire Line
	2400 1100 2650 1100
Wire Wire Line
	2650 1100 2650 1200
Connection ~ 2650 1200
Wire Wire Line
	2650 1200 2650 1300
Wire Wire Line
	2650 1500 2650 1700
Wire Wire Line
	2650 1700 3100 1700
Wire Wire Line
	3100 1700 3100 1500
Wire Wire Line
	2400 1500 2400 1700
Wire Wire Line
	2400 1700 2650 1700
Connection ~ 2650 1700
Wire Wire Line
	3400 1200 3600 1200
Wire Wire Line
	3600 1200 3600 1350
$Comp
L Device:CP1_Small C7
U 1 1 5E4D340A
P 3600 1450
F 0 "C7" H 3600 1550 50  0000 L CNN
F 1 "10nF/X7R" H 3650 1350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3600 1450 50  0001 C CNN
F 3 "~" H 3600 1450 50  0001 C CNN
	1    3600 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1550 3600 1700
Wire Wire Line
	3600 1700 3100 1700
Connection ~ 3100 1700
Wire Wire Line
	3100 1700 3100 1850
$Comp
L power:GND #PWR08
U 1 1 5E4DD176
P 3100 1850
F 0 "#PWR08" H 3100 1600 50  0001 C CNN
F 1 "GND" H 3105 1677 50  0000 C CNN
F 2 "" H 3100 1850 50  0001 C CNN
F 3 "" H 3100 1850 50  0001 C CNN
	1    3100 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 1300 4050 1100
Wire Wire Line
	4050 1500 4050 1700
Wire Wire Line
	4050 1700 3600 1700
Connection ~ 3600 1700
Text Notes 2050 850  0    100  Italic 20
PWR
Text Notes 9250 950  0    100  ~ 20
USB
Text Notes 2000 4500 0    100  ~ 20
MCU
Text Notes 7350 3850 0    100  ~ 20
MOTOR
Text Notes 2150 3100 0    100  ~ 20
SWD
Wire Wire Line
	3700 6700 3700 7000
Wire Wire Line
	3700 4550 3700 4800
Text Label 2000 3350 0    50   ~ 0
SWCLK
Text Label 2000 3450 0    50   ~ 0
SWDIO
Text Label 2000 3550 0    50   ~ 0
JTAG_SWO
$Comp
L power:GND #PWR04
U 1 1 5E5132C1
P 2000 3850
F 0 "#PWR04" H 2000 3600 50  0001 C CNN
F 1 "GND" H 2005 3677 50  0000 C CNN
F 2 "" H 2000 3850 50  0001 C CNN
F 3 "" H 2000 3850 50  0001 C CNN
	1    2000 3850
	1    0    0    -1  
$EndComp
Text Label 2000 3100 2    50   ~ 0
STLINK_VIN
Wire Wire Line
	2000 3100 2000 3250
Wire Wire Line
	2000 3750 2000 3850
Wire Wire Line
	4350 1650 4350 1700
Wire Wire Line
	4350 1700 4300 1700
Connection ~ 4050 1700
$Comp
L Device:C_Small C8
U 1 1 5E5463CE
P 4050 1400
F 0 "C8" H 4050 1500 50  0000 L CNN
F 1 "100nF" H 4050 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4050 1400 50  0001 C CNN
F 3 "~" H 4050 1400 50  0001 C CNN
	1    4050 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C6
U 1 1 5E546E6C
P 2650 1400
F 0 "C6" H 2650 1500 50  0000 L CNN
F 1 "100nF" H 2650 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2650 1400 50  0001 C CNN
F 3 "~" H 2650 1400 50  0001 C CNN
	1    2650 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C9
U 1 1 5E54EC51
P 4350 1550
F 0 "C9" H 4350 1650 50  0000 L CNN
F 1 "100nF" H 4350 1450 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4350 1550 50  0001 C CNN
F 3 "~" H 4350 1550 50  0001 C CNN
	1    4350 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C10
U 1 1 5E54F0CD
P 4600 1250
F 0 "C10" H 4600 1350 50  0000 L CNN
F 1 "100nF" H 4600 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4600 1250 50  0001 C CNN
F 3 "~" H 4600 1250 50  0001 C CNN
	1    4600 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C11
U 1 1 5E54F693
P 4850 1550
F 0 "C11" H 4850 1650 50  0000 L CNN
F 1 "100nF" H 4850 1450 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4850 1550 50  0001 C CNN
F 3 "~" H 4850 1550 50  0001 C CNN
	1    4850 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 1700 4350 1700
Connection ~ 4350 1700
Wire Wire Line
	4050 1100 4100 1100
Connection ~ 4050 1100
Wire Wire Line
	4350 1450 4350 1100
Connection ~ 4350 1100
Wire Wire Line
	4350 1100 4600 1100
Wire Wire Line
	4600 1100 4600 1150
Wire Wire Line
	4600 1350 4600 1700
Wire Wire Line
	4850 1450 4850 1100
Wire Wire Line
	4850 1100 4600 1100
Connection ~ 4600 1100
Wire Wire Line
	4850 1650 4850 1700
Wire Wire Line
	4850 1700 4600 1700
Connection ~ 4600 1700
$Comp
L power:VDD #PWR015
U 1 1 5E58BB8E
P 5400 1000
F 0 "#PWR015" H 5400 850 50  0001 C CNN
F 1 "VDD" H 5417 1173 50  0000 C CNN
F 2 "" H 5400 1000 50  0001 C CNN
F 3 "" H 5400 1000 50  0001 C CNN
	1    5400 1000
	1    0    0    -1  
$EndComp
Connection ~ 4850 1100
$Comp
L Device:Ferrite_Bead_Small FB2
U 1 1 5E5973BE
P 4200 1700
F 0 "FB2" V 4300 1750 50  0000 C CNN
F 1 "Ferrite_Bead_2012" V 4350 1700 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 4130 1700 50  0001 C CNN
F 3 "~" H 4200 1700 50  0001 C CNN
	1    4200 1700
	0    1    1    0   
$EndComp
$Comp
L Device:Ferrite_Bead_Small FB1
U 1 1 5E597E7F
P 4200 1100
F 0 "FB1" V 3963 1100 50  0000 C CNN
F 1 "Ferrite_Bead_2012" V 4054 1100 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 4130 1100 50  0001 C CNN
F 3 "~" H 4200 1100 50  0001 C CNN
	1    4200 1100
	0    1    1    0   
$EndComp
Wire Wire Line
	5400 1100 5400 1000
$Comp
L Device:CP1_Small C13
U 1 1 5E5AA756
P 5400 1350
F 0 "C13" H 5450 1450 50  0000 L CNN
F 1 "2.2uF" H 5450 1250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5400 1350 50  0001 C CNN
F 3 "~" H 5400 1350 50  0001 C CNN
	1    5400 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 1250 5400 1100
Connection ~ 5400 1100
Wire Wire Line
	5400 1450 5400 1700
Wire Wire Line
	4300 1100 4350 1100
Wire Wire Line
	4100 1700 4050 1700
Wire Wire Line
	4850 1100 5100 1100
Wire Wire Line
	5400 1700 5100 1700
Connection ~ 4850 1700
Wire Wire Line
	5400 1850 5400 1700
Connection ~ 5400 1700
$Comp
L power:VSSA #PWR016
U 1 1 5E5DE8C6
P 5400 1850
F 0 "#PWR016" H 5400 1700 50  0001 C CNN
F 1 "VSSA" H 5418 2023 50  0000 C CNN
F 2 "" H 5400 1850 50  0001 C CNN
F 3 "" H 5400 1850 50  0001 C CNN
	1    5400 1850
	-1   0    0    1   
$EndComp
$Comp
L power:VDDA #PWR019
U 1 1 5E5DF25D
P 5650 1000
F 0 "#PWR019" H 5650 850 50  0001 C CNN
F 1 "VDDA" H 5667 1173 50  0000 C CNN
F 2 "" H 5650 1000 50  0001 C CNN
F 3 "" H 5650 1000 50  0001 C CNN
	1    5650 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 1000 5650 1100
Wire Wire Line
	5650 1100 5400 1100
$Comp
L Device:C_Small C12
U 1 1 5E5E8DDF
P 5100 1250
F 0 "C12" H 5100 1350 50  0000 L CNN
F 1 "100nF" H 5100 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5100 1250 50  0001 C CNN
F 3 "~" H 5100 1250 50  0001 C CNN
	1    5100 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 1350 5100 1700
Connection ~ 5100 1700
Wire Wire Line
	5100 1700 4850 1700
Wire Wire Line
	5100 1150 5100 1100
Connection ~ 5100 1100
Wire Wire Line
	5100 1100 5400 1100
$Comp
L Device:CP1_Small C1
U 1 1 5E5F8563
P 1450 1450
F 0 "C1" H 1450 1550 50  0000 L CNN
F 1 "2.2uF/15V" H 1450 1350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1450 1450 50  0001 C CNN
F 3 "~" H 1450 1450 50  0001 C CNN
	1    1450 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 1200 1450 1350
Connection ~ 1450 1200
Wire Wire Line
	1450 1200 1650 1200
$Comp
L power:GND #PWR03
U 1 1 5E60DA06
P 1450 1750
F 0 "#PWR03" H 1450 1500 50  0001 C CNN
F 1 "GND" H 1455 1577 50  0000 C CNN
F 2 "" H 1450 1750 50  0001 C CNN
F 3 "" H 1450 1750 50  0001 C CNN
	1    1450 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 1750 1450 1550
$Comp
L Device:CP1_Small C4
U 1 1 5E62A12A
P 1800 3500
F 0 "C4" H 1600 3550 50  0000 L CNN
F 1 "2.2uF/15V" H 1400 3400 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1800 3500 50  0001 C CNN
F 3 "~" H 1800 3500 50  0001 C CNN
	1    1800 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 3750 2000 3750
Connection ~ 2000 3750
Wire Wire Line
	1800 3400 1800 3250
Wire Wire Line
	1800 3250 2000 3250
Connection ~ 2000 3250
Text Label 8850 4950 1    50   ~ 0
MOT1_STEP
Text Label 9050 4950 1    50   ~ 0
MOT2_STEP
Wire Wire Line
	9050 4500 9400 4500
Wire Wire Line
	9050 4500 9050 4950
Wire Wire Line
	8850 4500 8500 4500
Wire Wire Line
	8850 4500 8850 4950
Wire Wire Line
	8500 4600 8750 4600
Wire Wire Line
	8750 4600 8750 5150
Wire Wire Line
	9400 4600 9150 4600
Wire Wire Line
	9150 4600 9150 5150
Text Label 8750 5150 1    50   ~ 0
MOT_DIR_1
Text Label 9150 5150 1    50   ~ 0
MOT_DIR_2
Text Label 4550 5300 2    50   ~ 0
MOTuCONF1
Text Label 4550 5400 2    50   ~ 0
MOTuCONF2
Text Notes 1000 5800 0    100  ~ 20
XTAL
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5E6DB794
P 2750 3450
F 0 "J1" H 2722 3424 50  0000 R CNN
F 1 "SWD_CONN" H 2722 3333 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 2750 3450 50  0001 C CNN
F 3 "~" H 2750 3450 50  0001 C CNN
	1    2750 3450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1800 3600 1800 3750
Wire Wire Line
	2000 3750 2550 3750
Wire Wire Line
	2000 3250 2550 3250
Text Label 2750 5000 0    50   ~ 0
NRST
Text Label 2000 3650 0    50   ~ 0
NRST
Wire Wire Line
	2000 3650 2550 3650
Wire Wire Line
	2000 3550 2550 3550
Wire Wire Line
	2000 3450 2550 3450
Wire Wire Line
	2000 3350 2550 3350
$Comp
L power:VCC #PWR01
U 1 1 5E7891F2
P 800 1050
F 0 "#PWR01" H 800 900 50  0001 C CNN
F 1 "VCC" H 817 1223 50  0000 C CNN
F 2 "" H 800 1050 50  0001 C CNN
F 3 "" H 800 1050 50  0001 C CNN
	1    800  1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  1050 800  1200
Wire Wire Line
	800  1200 1450 1200
Text Notes 2600 3950 0    50   ~ 0
2.54mm Normal Pitch
Connection ~ 5450 6600
Wire Wire Line
	5450 6500 5900 6500
Wire Wire Line
	5450 6600 5450 6500
Wire Wire Line
	5900 6400 5350 6400
Wire Wire Line
	5350 6300 5900 6300
Wire Wire Line
	5900 6200 5350 6200
Wire Wire Line
	5350 6100 5900 6100
Wire Wire Line
	5900 6000 5350 6000
Text Label 5350 6100 0    50   ~ 0
SPI_SCK
Text Label 5350 6300 0    50   ~ 0
SPI_MOSI
Text Label 5350 6200 0    50   ~ 0
SPI_MISO
Text Label 5350 6000 0    50   ~ 0
ARGUS_IRQ
Text Label 5350 6400 0    50   ~ 0
ARGUS_CS
Connection ~ 5450 6700
Wire Wire Line
	5450 6600 5450 6700
Wire Wire Line
	5900 6600 5450 6600
Wire Wire Line
	5450 6700 5900 6700
Wire Wire Line
	5450 6750 5450 6700
$Comp
L power:GND #PWR018
U 1 1 5E811E5D
P 5450 6750
F 0 "#PWR018" H 5450 6500 50  0001 C CNN
F 1 "GND" H 5455 6577 50  0000 C CNN
F 2 "" H 5450 6750 50  0001 C CNN
F 3 "" H 5450 6750 50  0001 C CNN
	1    5450 6750
	1    0    0    -1  
$EndComp
Connection ~ 5450 5800
Wire Wire Line
	5450 5900 5450 5800
Wire Wire Line
	5900 5900 5450 5900
Wire Wire Line
	5450 5800 5450 5750
Wire Wire Line
	5900 5800 5450 5800
$Comp
L power:VCC #PWR017
U 1 1 5E7EBCAC
P 5450 5750
F 0 "#PWR017" H 5450 5600 50  0001 C CNN
F 1 "VCC" H 5467 5923 50  0000 C CNN
F 2 "" H 5450 5750 50  0001 C CNN
F 3 "" H 5450 5750 50  0001 C CNN
	1    5450 5750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Male J3
U 1 1 5E7E9C9E
P 6100 6300
F 0 "J3" H 6072 6182 50  0000 R CNN
F 1 "ARGUS_SPI_FFC_CONN" H 6072 6273 50  0000 R CNN
F 2 "Connector_FFC-FPC:Hirose_FH12-10S-0.5SH_1x10-1MP_P0.50mm_Horizontal" H 6100 6300 50  0001 C CNN
F 3 "~" H 6100 6300 50  0001 C CNN
	1    6100 6300
	-1   0    0    1   
$EndComp
Wire Wire Line
	5250 5050 5650 5050
Wire Wire Line
	5650 5150 5250 5150
Wire Wire Line
	5250 5250 5650 5250
Wire Wire Line
	5250 5350 5650 5350
Wire Wire Line
	6550 5350 6150 5350
Wire Wire Line
	6750 5150 6750 5200
Wire Wire Line
	6150 5150 6750 5150
Wire Wire Line
	6750 5050 6150 5050
Wire Wire Line
	6750 5000 6750 5050
$Comp
L power:VCC #PWR025
U 1 1 5E79200A
P 6750 5000
F 0 "#PWR025" H 6750 4850 50  0001 C CNN
F 1 "VCC" H 6767 5173 50  0000 C CNN
F 2 "" H 6750 5000 50  0001 C CNN
F 3 "" H 6750 5000 50  0001 C CNN
	1    6750 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR026
U 1 1 5E788666
P 6750 5200
F 0 "#PWR026" H 6750 4950 50  0001 C CNN
F 1 "GND" H 6755 5027 50  0000 C CNN
F 2 "" H 6750 5200 50  0001 C CNN
F 3 "" H 6750 5200 50  0001 C CNN
	1    6750 5200
	1    0    0    -1  
$EndComp
NoConn ~ 6150 5250
Text Label 6550 5350 2    50   ~ 0
ARGUS_CS
Text Label 5250 5350 0    50   ~ 0
ARGUS_IRQ
Text Label 5250 5250 0    50   ~ 0
SPI_MISO
Text Label 5250 5150 0    50   ~ 0
SPI_MOSI
Text Label 5250 5050 0    50   ~ 0
SPI_SCK
NoConn ~ 6150 4950
NoConn ~ 5650 4950
Text Notes 5200 4600 0    100  ~ 20
SENSOR
$Comp
L Connector_Generic:Conn_02x05_Counter_Clockwise J2
U 1 1 5E3BCD38
P 5850 5150
F 0 "J2" H 5900 5567 50  0000 C CNN
F 1 "ARGUS_SPI_LEGACY_CONN" H 5900 5476 50  0000 C CNN
F 2 "Connector_PinHeader_1.27mm:CONN02x05CLOCKWISE" H 5850 5150 50  0001 C CNN
F 3 "~" H 5850 5150 50  0001 C CNN
	1    5850 5150
	1    0    0    -1  
$EndComp
$Comp
L power:+15V #PWR032
U 1 1 5EA03463
P 8950 3500
F 0 "#PWR032" H 8950 3350 50  0001 C CNN
F 1 "+15V" H 8965 3673 50  0000 C CNN
F 2 "" H 8950 3500 50  0001 C CNN
F 3 "" H 8950 3500 50  0001 C CNN
	1    8950 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 3500 8950 3600
Wire Wire Line
	8950 3600 8100 3600
Wire Wire Line
	8100 3600 8100 3800
Wire Wire Line
	8950 3600 9800 3600
Wire Wire Line
	9800 3600 9800 3800
Connection ~ 8950 3600
Wire Wire Line
	1300 6150 1300 6350
Connection ~ 1300 6150
Wire Wire Line
	1250 6150 1300 6150
$Comp
L power:GND #PWR02
U 1 1 5E6D2E23
P 1250 6150
F 0 "#PWR02" H 1250 5900 50  0001 C CNN
F 1 "GND" V 1255 6022 50  0000 R CNN
F 2 "" H 1250 6150 50  0001 C CNN
F 3 "" H 1250 6150 50  0001 C CNN
	1    1250 6150
	0    1    1    0   
$EndComp
Wire Wire Line
	1300 5950 1300 6150
Wire Wire Line
	1300 5950 1400 5950
Wire Wire Line
	1400 6350 1300 6350
Wire Wire Line
	1850 6350 1600 6350
$Comp
L Device:C_Small C3
U 1 1 5E6B4D27
P 1500 6350
F 0 "C3" V 1550 6450 50  0000 C CNN
F 1 "22pF" V 1550 6200 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1500 6350 50  0001 C CNN
F 3 "~" H 1500 6350 50  0001 C CNN
	1    1500 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	1600 5950 1850 5950
$Comp
L Device:C_Small C2
U 1 1 5E6AC17F
P 1500 5950
F 0 "C2" V 1450 6050 50  0000 C CNN
F 1 "22pF" V 1450 5800 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1500 5950 50  0001 C CNN
F 3 "~" H 1500 5950 50  0001 C CNN
	1    1500 5950
	0    1    1    0   
$EndComp
Wire Wire Line
	1850 6050 1850 5950
Wire Wire Line
	2550 5700 3000 5700
Wire Wire Line
	2550 5600 3000 5600
Text Label 2300 5950 2    50   ~ 0
OSC_IN
Wire Wire Line
	2300 5950 1850 5950
Connection ~ 1850 5950
Wire Wire Line
	1850 6350 1850 6250
Wire Wire Line
	2300 6350 1850 6350
Text Label 2300 6350 2    50   ~ 0
OSC_OUT
Connection ~ 1850 6350
$Comp
L power:VCC #PWR033
U 1 1 5E6924E1
P 8950 3900
F 0 "#PWR033" H 8950 3750 50  0001 C CNN
F 1 "VCC" H 8967 4073 50  0000 C CNN
F 2 "" H 8950 3900 50  0001 C CNN
F 3 "" H 8950 3900 50  0001 C CNN
	1    8950 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 3900 8950 4100
$Comp
L Device:Jumper_NC_Small JP2
U 1 1 5E6A08FD
P 8750 6350
F 0 "JP2" H 8750 6450 50  0000 C CNN
F 1 "CONF_FIX_JP" H 8450 6300 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 8750 6350 50  0001 C CNN
F 3 "~" H 8750 6350 50  0001 C CNN
	1    8750 6350
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NC_Small JP3
U 1 1 5E6A1CD5
P 9150 6350
F 0 "JP3" H 9150 6450 50  0000 C CNN
F 1 "CONF_FIX_JP" H 9450 6300 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 9150 6350 50  0001 C CNN
F 3 "~" H 9150 6350 50  0001 C CNN
	1    9150 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 6350 8600 6350
Wire Wire Line
	9250 6350 9300 6350
Wire Wire Line
	9050 6350 8950 6350
$Comp
L power:VCC #PWR035
U 1 1 5E6CAA29
P 8950 6000
F 0 "#PWR035" H 8950 5850 50  0001 C CNN
F 1 "VCC" H 8967 6173 50  0000 C CNN
F 2 "" H 8950 6000 50  0001 C CNN
F 3 "" H 8950 6000 50  0001 C CNN
	1    8950 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 5000 9300 6350
Connection ~ 9300 5000
Wire Wire Line
	8600 5000 8600 6350
Connection ~ 8600 5000
$Comp
L Device:R_Small_US R6
U 1 1 5E70F936
P 8950 6150
F 0 "R6" H 9000 6250 50  0000 L CNN
F 1 "22k" H 8800 6250 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8950 6150 50  0001 C CNN
F 3 "~" H 8950 6150 50  0001 C CNN
	1    8950 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 6050 8950 6000
Wire Wire Line
	8950 6250 8950 6350
Connection ~ 8950 6350
Wire Wire Line
	8950 6350 8850 6350
Wire Wire Line
	1650 1000 1100 1000
$Comp
L power:VCC #PWR021
U 1 1 5E748F80
P 5850 2700
F 0 "#PWR021" H 5850 2550 50  0001 C CNN
F 1 "VCC" H 5867 2873 50  0000 C CNN
F 2 "" H 5850 2700 50  0001 C CNN
F 3 "" H 5850 2700 50  0001 C CNN
	1    5850 2700
	1    0    0    -1  
$EndComp
$Comp
L power:+15V #PWR023
U 1 1 5E757DD7
P 6250 2700
F 0 "#PWR023" H 6250 2550 50  0001 C CNN
F 1 "+15V" H 6265 2873 50  0000 C CNN
F 2 "" H 6250 2700 50  0001 C CNN
F 3 "" H 6250 2700 50  0001 C CNN
	1    6250 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3250 6250 3050
Wire Wire Line
	5850 3250 5850 3050
$Comp
L Device:LED D5
U 1 1 5E7B0D52
P 6250 3400
F 0 "D5" H 6400 3450 50  0000 R CNN
F 1 "LED_R" H 6150 3450 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 6250 3400 50  0001 C CNN
F 3 "~" H 6250 3400 50  0001 C CNN
	1    6250 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D4
U 1 1 5E7AF969
P 5850 3400
F 0 "D4" H 6000 3450 50  0000 R CNN
F 1 "LED_Y" H 5750 3450 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 5850 3400 50  0001 C CNN
F 3 "~" H 5850 3400 50  0001 C CNN
	1    5850 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small_US R5
U 1 1 5E784AD2
P 6250 2950
F 0 "R5" V 6200 3000 50  0000 L CNN
F 1 "1k" V 6200 2750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6250 2950 50  0001 C CNN
F 3 "~" H 6250 2950 50  0001 C CNN
	1    6250 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R4
U 1 1 5E7590C5
P 5850 2950
F 0 "R4" V 5800 3000 50  0000 L CNN
F 1 "300" V 5800 2750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5850 2950 50  0001 C CNN
F 3 "~" H 5850 2950 50  0001 C CNN
	1    5850 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 2850 6250 2700
Wire Wire Line
	5850 2850 5850 2700
$Comp
L power:GND #PWR022
U 1 1 5E82573C
P 5850 3850
F 0 "#PWR022" H 5850 3600 50  0001 C CNN
F 1 "GND" H 5855 3677 50  0000 C CNN
F 2 "" H 5850 3850 50  0001 C CNN
F 3 "" H 5850 3850 50  0001 C CNN
	1    5850 3850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack J4
U 1 1 5E882DA2
P 7200 2650
F 0 "J4" H 7257 2975 50  0000 C CNN
F 1 "VMOT_IN_5mm" H 7257 2884 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 7250 2610 50  0001 C CNN
F 3 "~" H 7250 2610 50  0001 C CNN
	1    7200 2650
	1    0    0    -1  
$EndComp
$Comp
L power:+15V #PWR028
U 1 1 5E882DA8
P 7950 2350
F 0 "#PWR028" H 7950 2200 50  0001 C CNN
F 1 "+15V" H 7965 2523 50  0000 C CNN
F 2 "" H 7950 2350 50  0001 C CNN
F 3 "" H 7950 2350 50  0001 C CNN
	1    7950 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 2550 7750 2550
Wire Wire Line
	7750 2550 7750 2400
Wire Wire Line
	7750 2400 7950 2400
Wire Wire Line
	7500 2750 7750 2750
Wire Wire Line
	7750 2750 7750 2900
$Comp
L Device:CP1_Small C14
U 1 1 5E882DB3
P 8100 2650
F 0 "C14" H 8191 2696 50  0000 L CNN
F 1 "100uF/25V" H 8191 2605 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_6.3x5.2" H 8100 2650 50  0001 C CNN
F 3 "~" H 8100 2650 50  0001 C CNN
	1    8100 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 2550 8100 2400
Wire Wire Line
	8100 2750 8100 2900
Wire Wire Line
	7950 2350 7950 2400
Connection ~ 7950 2400
Wire Wire Line
	7950 2400 8100 2400
Wire Wire Line
	7750 2900 7950 2900
Wire Wire Line
	7950 2950 7950 2900
Connection ~ 7950 2900
Wire Wire Line
	7950 2900 8100 2900
Text Notes 6750 2200 0    100  ~ 20
MOTOR_PWR
Text Label 5650 4000 1    50   ~ 0
SWDIO
$Comp
L Device:LED D3
U 1 1 5E8B0E84
P 5650 3400
F 0 "D3" H 5800 3450 50  0000 R CNN
F 1 "LED_G" H 5550 3450 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 5650 3400 50  0001 C CNN
F 3 "~" H 5650 3400 50  0001 C CNN
	1    5650 3400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5650 4000 5650 3550
$Comp
L Device:R_Small_US R3
U 1 1 5E8EC9A0
P 5650 2950
F 0 "R3" V 5600 3000 50  0000 L CNN
F 1 "300" V 5600 2750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5650 2950 50  0001 C CNN
F 3 "~" H 5650 2950 50  0001 C CNN
	1    5650 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3050 5650 3250
$Comp
L power:VCC #PWR020
U 1 1 5E8FBF8F
P 5650 2700
F 0 "#PWR020" H 5650 2550 50  0001 C CNN
F 1 "VCC" H 5667 2873 50  0000 C CNN
F 2 "" H 5650 2700 50  0001 C CNN
F 3 "" H 5650 2700 50  0001 C CNN
	1    5650 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 2700 5650 2850
Text Notes 5850 2400 2    100  ~ 20
INDICATOR
Text Notes 5750 4150 0    50   ~ 0
All LEDs are 2012 metric
$Comp
L Switch:SW_Push SW1
U 1 1 5E97A8C1
P 2450 5000
F 0 "SW1" H 2450 5285 50  0000 C CNN
F 1 "SW_RESET" H 2450 5194 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SMD_TS1107" H 2450 5200 50  0001 C CNN
F 3 "~" H 2450 5200 50  0001 C CNN
	1    2450 5000
	1    0    0    -1  
$EndComp
Text Notes 3650 3200 0    100  ~ 20
BOOT0
Text Label 3650 3400 0    50   ~ 0
BOOT0
$Comp
L Device:R_US R2
U 1 1 5E99E7B0
P 4450 3700
F 0 "R2" H 4518 3746 50  0000 L CNN
F 1 "22k" H 4518 3655 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 4490 3690 50  0001 C CNN
F 3 "~" H 4450 3700 50  0001 C CNN
	1    4450 3700
	-1   0    0    1   
$EndComp
Wire Wire Line
	3650 3400 4450 3400
Connection ~ 4450 3400
$Comp
L power:GND #PWR014
U 1 1 5E9FBF30
P 4450 3900
F 0 "#PWR014" H 4450 3650 50  0001 C CNN
F 1 "GND" H 4455 3727 50  0000 C CNN
F 2 "" H 4450 3900 50  0001 C CNN
F 3 "" H 4450 3900 50  0001 C CNN
	1    4450 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3400 4450 3550
$Comp
L power:VDD #PWR013
U 1 1 5EA4D38C
P 4450 2750
F 0 "#PWR013" H 4450 2600 50  0001 C CNN
F 1 "VDD" H 4467 2923 50  0000 C CNN
F 2 "" H 4450 2750 50  0001 C CNN
F 3 "" H 4450 2750 50  0001 C CNN
	1    4450 2750
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NC_Small JP1
U 1 1 5EA4D988
P 4450 2900
F 0 "JP1" V 4496 2852 50  0000 R CNN
F 1 "BOOT0_JP" V 4405 2852 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4450 2900 50  0001 C CNN
F 3 "~" H 4450 2900 50  0001 C CNN
	1    4450 2900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4450 2750 4450 2800
Text Notes 3400 3600 0    50   ~ 0
Default configuration\nis pull-down (from FLASH)
$Comp
L power:GNDPWR #PWR029
U 1 1 5EAB003B
P 7950 2950
F 0 "#PWR029" H 7950 2750 50  0001 C CNN
F 1 "GNDPWR" H 7954 2796 50  0000 C CNN
F 2 "" H 7950 2900 50  0001 C CNN
F 3 "" H 7950 2900 50  0001 C CNN
	1    7950 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 5200 8100 5550
Wire Wire Line
	9800 5200 9800 5550
$Comp
L power:GNDPWR #PWR027
U 1 1 5EAC0146
P 7900 5450
F 0 "#PWR027" H 7900 5250 50  0001 C CNN
F 1 "GNDPWR" H 7904 5296 50  0000 C CNN
F 2 "" H 7900 5400 50  0001 C CNN
F 3 "" H 7900 5400 50  0001 C CNN
	1    7900 5450
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR038
U 1 1 5EACF646
P 10000 5400
F 0 "#PWR038" H 10000 5200 50  0001 C CNN
F 1 "GNDPWR" H 10004 5246 50  0000 C CNN
F 2 "" H 10000 5350 50  0001 C CNN
F 3 "" H 10000 5350 50  0001 C CNN
	1    10000 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal_GND24_Small Y1
U 1 1 5E6882FB
P 1850 6150
F 0 "Y1" V 1800 5950 50  0000 L CNN
F 1 "24MHz" V 1900 5850 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_SeikoEpson_FA238V-4Pin_3.2x2.5mm_HandSoldering" H 1850 6150 50  0001 C CNN
F 3 "~" H 1850 6150 50  0001 C CNN
	1    1850 6150
	0    1    1    0   
$EndComp
Wire Wire Line
	1750 6150 1300 6150
$Comp
L power:GND #PWR06
U 1 1 5EB655F6
P 2200 6150
F 0 "#PWR06" H 2200 5900 50  0001 C CNN
F 1 "GND" V 2205 6022 50  0000 R CNN
F 2 "" H 2200 6150 50  0001 C CNN
F 3 "" H 2200 6150 50  0001 C CNN
	1    2200 6150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2200 6150 1950 6150
$Comp
L power:GNDPWR #PWR024
U 1 1 5EB96F67
P 6250 3850
F 0 "#PWR024" H 6250 3650 50  0001 C CNN
F 1 "GNDPWR" H 6254 3696 50  0000 C CNN
F 2 "" H 6250 3800 50  0001 C CNN
F 3 "" H 6250 3800 50  0001 C CNN
	1    6250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3550 6250 3850
Wire Wire Line
	5850 3550 5850 3850
Wire Wire Line
	4450 3850 4450 3900
Wire Wire Line
	4450 3000 4450 3400
$EndSCHEMATC
