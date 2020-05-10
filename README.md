# thermometer-ATTINY85-Nokia5110LCD-DHT11
simple thermometer and hygrometer based on ATTINY85/45, Nokia 5110 LCD module and DHT11 or DHT22 sensor

Temperature and Humidity measurements are done every 4 seconds, meanwhile power saving mode - works up to 1 month on single 2xAA  battery set. 
The code is 2KB long so it will fit into smaller ATTINY's also (it may fit to ATTINY25 ora ATTINY2313 after some small changes   like not checking CRC from DHT sensor).  
DHT11 sensor operates within positive temperature area 0 - 50 Celsius degrees ( indoor)  
while  DHT22 operates within temperature area  -40 to +80 degrees ( outdoor area). 
Choose appropriate version of main file for your sensor.


to see how it works - look here https://www.youtube.com/watch?v=r6nF1iJ2r-M


----------------------------------------------------------------------------------------------------------------------------

CONNECTIONS TO BE MADE :

DHT11 or DHT22 - DATA pin connected to PB3 of ATTINY85

Nokia LCD 5110 Interface connected to following ATTINY85 pins:

PB0 = RESET

PB1 = DC

PB2 = DIN / DATA IN

PB4 = CLK

GND connected to CS, BL and GND on NOKIA5110 display. Between GND and VCC some small capacitors like 100nF and 10uF should be put for blocking interferences.

the VCC is 3V ( 2xAA battery) or Solar Cell 4V, it can work with higher voltage like 4.5V (3xAA) battery but it is a bit rysky for LCD 5110 component..

------------------------------------------------------------------------------------------------------------------------

SOURCE FILE OPTIONS :

ATTINY85 has factory fusebits - 8 MHz clock with DIV to 1MHz

"main.c" and "compileatmega" files are for DHT11 sensor and 1MHz clock of ATTINY85/45 ( ultra low power consumption)

"main2.c" and "compileatmega2" files are for DHT11 sensor and 8MHz clock of ATTINY85/45

"main3.c" and "compileatmega3" files are for DHT22 sensor and 1MHz clock of ATTINY85/45 ( ultra low power consumption)

Link to video how to program the chip : https://www.youtube.com/watch?v=7klgyNzZ2TI


------------------------------------------------------------------------------------------------------

COMPILATION ON LINUX PC :

To upload program code to the chip using cheapest USBASP programmer (less than 2 USD on eBay/Aliexpress) 
look at this page : http://www.learningaboutelectronics.com/Articles/Program-AVR-chip-using-a-USBASP-with-10-pin-cable.php

The script attached in repository ( "compileatmegaX") can be used to upload data to the chip if you have Linux machine with following packages : "gcc-avr", "binutils-avr" (or sometimes just "binutils"), "avr-libc", "avrdude" and optionally "gdb-avr"(debugger only if you really need it) . 
For example in Ubuntu download these packages using command : "sudo apt-get install gcc-avr binutils-avr avr-libc gdb-avr avrdude". 
After doing it you will be able to run compilation the script from the directory you have downloaded github files by commands: "sudo chmod +rx compileattiny" and "sudo ./compileattiny"  

------------------------------------------------------------------------------------------------------

COMPILATION ON WINDOWS 10 PC :

If you have Windows 10 machine - follow this tutorial to download and install full AVR-GCC environment : http://fab.cba.mit.edu/classes/863.16/doc/projects/ftsmin/windows_avr.html
and use "compileattinyX.bat" files for compilaton in the directory where you have downloaded mainX.c files. You have to be logged as Windows Administrator and run "cmd" from search window to do that. Then use commands like "cd XXXXX" to change working directory to get to downloaded source files.


If you are having problems with C code compilation or USBASR programmer you may also look at these tutorials  :  http://www.linuxandubuntu.com/home/setting-up-avr-gcc-toolchain-and-avrdude-to-program-an-avr-development-board-in-ubuntu 

https://blog.podkalicki.com/how-to-compile-and-burn-the-code-to-avr-chip-on-linuxmacosxwindows/  



