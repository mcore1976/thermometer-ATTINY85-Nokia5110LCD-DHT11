# thermometer-ATTINY85-Nokia5110LCD-DHT11
simple thermometer and hygrometer based on ATTINY85/45, Nokia 5110 LCD module and DHT11 or DHT22 sensor

Temperature and Humidity measurements are done every 4 seconds, meanwhile power saving mode - works up to 1 month on single 2xAA  battery set. 
The code is 2KB long so it will fit into smaller ATTINY's also (it may fit to ATTINY25 ora ATTINY2313 after some small changes   like not checking CRC from DHT sensor).  
DHT11 sensor operates within positive temperature area 0 - 50 Celsius degrees ( indoor)  
while  DHT22 operates within temperature area  -40 to +80 degrees ( outdoor area). 
Choose appropriate version of main file for your sensor.

DHT11 or DHT22 - DATA pin connected to PB3 of ATTINY85

Nokia LCD 5110 Interface connected to following ATTINY85 pins:

PB0 = RESET

PB1 = DC

PB2 = DIN / DATA IN

PB4 = CLK

GND connected to CS, BL and GND on NOKIA5110 display. Between GND and VCC some small capacitors like 100nF and 10uF should be put for blocking interferences.

the VCC is 3V ( 2xAA battery), but also works with 4.5V (3xAA) battery

ATTINY85 has factory fusebits - 8 MHz clock with DIV to 1MHz

"main.c" and "compileatmega" files are for DHT11 sensor and 1MHz clock of ATTINY85/45 ( ultra low power consumption)

"main2.c" and "compileatmega2" files are for DHT11 sensor and 8MHz clock of ATTINY85/45

"main3.c" and "compileatmega3" files are for DHT22 sensor and 1MHz clock of ATTINY85/45 ( ultra low power consumption)



to see how it works - look here https://www.youtube.com/watch?v=r6nF1iJ2r-M
