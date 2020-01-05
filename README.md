# thermometer-ATTINY85-Nokia5110LCD-DHT11
simple thermometer and hygrometer based on ATTINY85/45, Nokia 5110 LCD module and DHT11 sensor

Temperature and Humidity measure from DHT11 connected to PB3

with Nokia LCD 5110 Interface connected to ATTINY85 pins:

PB0 = RESET

PB1 = DC

PB2 = DIN / DATA IN

PB4 = CLK

GND connected to CS, BL and GND on NOKIA5110 display

the VCC is 3V ( 2xAA battery), but also works with 4.5V (3xAA) battery

ATTINY85 has factory fusebits - 8 MHz clock with DIV to 1MHz
