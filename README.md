# thermometer-ATTINY85-Nokia5110LCD-DHT11
simple thermometer and hygrometer based on ATTINY85/45, Nokia 5110 LCD module and DHT11 sensor

Temperature and Humidity measurements every 4 seconds, meanwhile power saving mode - works up to 1 month on single battery set. The code is 2KB long so it will fit into smaller ATTINY's also.

DHT11 - DATA pin connected to PB3 of ATTINY85

Nokia LCD 5110 Interface connected to following ATTINY85 pins:

PB0 = RESET

PB1 = DC

PB2 = DIN / DATA IN

PB4 = CLK

GND connected to CS, BL and GND on NOKIA5110 display. Between GND and VCC some small capacitors like 100nF and 10uF should be put for blocking interferences.

the VCC is 3V ( 2xAA battery), but also works with 4.5V (3xAA) battery

ATTINY85 has factory fusebits - 8 MHz clock with DIV to 1MHz

"main.c" and "compileatmega" files are for 1MHz clock of ATTINY85/45 ( ultra low power consumption)

"main2.c" and "compileatmega2" files are for 8MHz clock of ATTINY85/45

to see how it works - look here https://www.youtube.com/watch?v=r6nF1iJ2r-M
