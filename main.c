/*--------------------------------------------------------------------------*
* Temperature and Humidity measure from DHT11 connected to PB3
* with Nokia LCD 5110 Interface connected to ATTINY85 pins:
* PB0 = RESET
* PB1 = DC
* PB2 = DIN / DATA IN
* PB4 = CLK
* GND connected to CS, BL and GND on NOKIA5110 display
* the VCC is 3V ( 2xAA battery), but also works with 4.5V (3xAA) battery
* ATTINY85 has factory fusebits - 8 MHz clock with DIV to 1MHz
* --------------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


// DHT11 sensor settings

#define DHT_PIN            PB3

#define DHT_ERR_OK         (0)
#define DHT_ERR_TIMEOUT    (-1)

#define DHT_PIN_INPUT()    (DDRB &= ~_BV(DHT_PIN))
#define DHT_PIN_OUTPUT()   (DDRB |= _BV(DHT_PIN))
#define DHT_PIN_LOW()      (PORTB &= ~_BV(DHT_PIN))
#define DHT_PIN_HIGH()     (PORTB |= _BV(DHT_PIN))
#define DHT_PIN_READ()     (PINB & _BV(DHT_PIN))
#define DHT_TIMEOUT        (80)


static void dht_init(void);
int8_t dht_read(uint8_t *temperature, uint8_t *humidity);

 
/** Glyph data (stored in PROGMEM)
 */
extern const uint8_t BIG_FONT[] PROGMEM;


// Elegant 11x8x2 font digits
 
const uint8_t BIG_FONT[] PROGMEM = {
   0x00,0xF0,0xFC,0xFE,0x06,0x02,0x06,0xFE,0xFC,0xF0,0x00, // 0
   0x00,0x07,0x1F,0x3F,0x30,0x20,0x30,0x3F,0x1F,0x07,0x00,
   0x00,0x00,0x08,0x0C,0xFC,0xFE,0xFE,0x00,0x00,0x00,0x00, // 1
   0x00,0x20,0x20,0x20,0x3F,0x3F,0x3F,0x20,0x20,0x20,0x00,
   0x00,0x0C,0x0E,0x06,0x02,0x02,0x86,0xFE,0x7C,0x38,0x00, // 2
   0x00,0x30,0x38,0x3C,0x36,0x33,0x31,0x30,0x30,0x38,0x00,
   0x00,0x0C,0x0E,0x86,0x82,0x82,0xC6,0xFE,0x7C,0x38,0x00, // 3
   0x00,0x18,0x38,0x30,0x20,0x20,0x31,0x3F,0x1F,0x0E,0x00,
   0x00,0x00,0xC0,0x20,0x18,0x04,0xFE,0xFE,0xFE,0x00,0x00, // 4
   0x00,0x03,0x02,0x02,0x02,0x22,0x3F,0x3F,0x3F,0x22,0x02,
   0x00,0x00,0x7E,0x7E,0x46,0x46,0xC6,0xC6,0x86,0x00,0x00, // 5
   0x00,0x18,0x38,0x30,0x20,0x20,0x30,0x3F,0x1F,0x0F,0x00,
   0x00,0xC0,0xF0,0xF8,0xFC,0x4C,0xC6,0xC2,0x82,0x00,0x00, // 6
   0x00,0x0F,0x1F,0x3F,0x30,0x20,0x30,0x3F,0x1F,0x0F,0x00,
   0x00,0x06,0x06,0x06,0x06,0x06,0xC6,0xF6,0x3E,0x0E,0x00, // 7
   0x00,0x00,0x00,0x30,0x3C,0x0F,0x03,0x00,0x00,0x00,0x00,
   0x00,0x38,0x7C,0xFE,0xC6,0x82,0xC6,0xFE,0x7C,0x38,0x00, // 8
   0x00,0x0E,0x1F,0x3F,0x31,0x20,0x31,0x3F,0x1F,0x0E,0x00,
   0x00,0x78,0xFC,0xFE,0x86,0x02,0x86,0xFE,0xFC,0xF8,0x00, // 9
   0x00,0x00,0x00,0x21,0x21,0x31,0x1D,0x1F,0x0F,0x03,0x00,
   0xF0,0xF8,0x0C,0x06,0x02,0x02,0x02,0x02,0x0E,0x0C,0x00, // C
   0x03,0x07,0x0C,0x18,0x10,0x10,0x10,0x10,0x1C,0x0C,0x00,
   0x00,0x06,0x06,0x09,0x09,0x09,0x09,0x06,0x06,0x00,0x00, // degrees
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
   0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00, // minus sign
   0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00 
};

// SMALL FONT 5x7 

extern const uint8_t SMALL_FONT[] PROGMEM;

const uint8_t SMALL_FONT[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, // 20
  0x00, 0x00, 0x5f, 0x00, 0x00, // 21 !
  0x00, 0x07, 0x00, 0x07, 0x00, // 22 "
  0x14, 0x7f, 0x14, 0x7f, 0x14, // 23 #
  0x24, 0x2a, 0x7f, 0x2a, 0x12, // 24 $
  0x23, 0x13, 0x08, 0x64, 0x62, // 25 %
  0x36, 0x49, 0x55, 0x22, 0x50, // 26 &
  0x00, 0x05, 0x03, 0x00, 0x00, // 27 '
  0x00, 0x1c, 0x22, 0x41, 0x00, // 28 (
  0x00, 0x41, 0x22, 0x1c, 0x00, // 29 )
  0x14, 0x08, 0x3e, 0x08, 0x14, // 2a *
  0x08, 0x08, 0x3e, 0x08, 0x08, // 2b +
  0x00, 0x50, 0x30, 0x00, 0x00, // 2c ,
  0x08, 0x08, 0x08, 0x08, 0x08, // 2d -
  0x00, 0x60, 0x60, 0x00, 0x00, // 2e .
  0x20, 0x10, 0x08, 0x04, 0x02, // 2f /
  0x3e, 0x51, 0x49, 0x45, 0x3e, // 30 0
  0x00, 0x42, 0x7f, 0x40, 0x00, // 31 1
  0x42, 0x61, 0x51, 0x49, 0x46, // 32 2
  0x21, 0x41, 0x45, 0x4b, 0x31, // 33 3
  0x18, 0x14, 0x12, 0x7f, 0x10, // 34 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 35 5
  0x3c, 0x4a, 0x49, 0x49, 0x30, // 36 6
  0x01, 0x71, 0x09, 0x05, 0x03, // 37 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 38 8
  0x06, 0x49, 0x49, 0x29, 0x1e, // 39 9
  0x00, 0x36, 0x36, 0x00, 0x00, // 3a :
  0x00, 0x56, 0x36, 0x00, 0x00, // 3b ;
  0x08, 0x14, 0x22, 0x41, 0x00, // 3c <
  0x14, 0x14, 0x14, 0x14, 0x14, // 3d =
  0x00, 0x41, 0x22, 0x14, 0x08, // 3e >
  0x02, 0x01, 0x51, 0x09, 0x06, // 3f ?
  0x32, 0x49, 0x79, 0x41, 0x3e, // 40 @
  0x7e, 0x11, 0x11, 0x11, 0x7e, // 41 A
  0x7f, 0x49, 0x49, 0x49, 0x36, // 42 B
  0x3e, 0x41, 0x41, 0x41, 0x22, // 43 C
  0x7f, 0x41, 0x41, 0x22, 0x1c, // 44 D
  0x7f, 0x49, 0x49, 0x49, 0x41, // 45 E
  0x7f, 0x09, 0x09, 0x09, 0x01, // 46 F
  0x3e, 0x41, 0x49, 0x49, 0x7a, // 47 G
  0x7f, 0x08, 0x08, 0x08, 0x7f, // 48 H
  0x00, 0x41, 0x7f, 0x41, 0x00, // 49 I
  0x20, 0x40, 0x41, 0x3f, 0x01, // 4a J
  0x7f, 0x08, 0x14, 0x22, 0x41, // 4b K
  0x7f, 0x40, 0x40, 0x40, 0x40, // 4c L
  0x7f, 0x02, 0x0c, 0x02, 0x7f, // 4d M
  0x7f, 0x04, 0x08, 0x10, 0x7f, // 4e N
  0x3e, 0x41, 0x41, 0x41, 0x3e, // 4f O
  0x7f, 0x09, 0x09, 0x09, 0x06, // 50 P
  0x3e, 0x41, 0x51, 0x21, 0x5e, // 51 Q
  0x7f, 0x09, 0x19, 0x29, 0x46, // 52 R
  0x46, 0x49, 0x49, 0x49, 0x31, // 53 S
  0x01, 0x01, 0x7f, 0x01, 0x01, // 54 T
  0x3f, 0x40, 0x40, 0x40, 0x3f, // 55 U
  0x1f, 0x20, 0x40, 0x20, 0x1f, // 56 V
  0x3f, 0x40, 0x38, 0x40, 0x3f, // 57 W
  0x63, 0x14, 0x08, 0x14, 0x63, // 58 X
  0x07, 0x08, 0x70, 0x08, 0x07, // 59 Y
  0x61, 0x51, 0x49, 0x45, 0x43, // 5a Z
  0x00, 0x7f, 0x41, 0x41, 0x00, // 5b [
  0x02, 0x04, 0x08, 0x10, 0x20, // 5c �
  0x00, 0x41, 0x41, 0x7f, 0x00, // 5d ]
  0x04, 0x02, 0x01, 0x02, 0x04, // 5e ^
  0x40, 0x40, 0x40, 0x40, 0x40, // 5f _
  0x00, 0x01, 0x02, 0x04, 0x00, // 60 `
  0x20, 0x54, 0x54, 0x54, 0x78, // 61 a
  0x7f, 0x48, 0x44, 0x44, 0x38, // 62 b
  0x38, 0x44, 0x44, 0x44, 0x20, // 63 c
  0x38, 0x44, 0x44, 0x48, 0x7f, // 64 d
  0x38, 0x54, 0x54, 0x54, 0x18, // 65 e
  0x08, 0x7e, 0x09, 0x01, 0x02, // 66 f
  0x0c, 0x52, 0x52, 0x52, 0x3e, // 67 g
  0x7f, 0x08, 0x04, 0x04, 0x78, // 68 h
  0x00, 0x44, 0x7d, 0x40, 0x00, // 69 i
  0x20, 0x40, 0x44, 0x3d, 0x00, // 6a j
  0x7f, 0x10, 0x28, 0x44, 0x00, // 6b k
  0x00, 0x41, 0x7f, 0x40, 0x00, // 6c l
  0x7c, 0x04, 0x18, 0x04, 0x78, // 6d m
  0x7c, 0x08, 0x04, 0x04, 0x78, // 6e n
  0x38, 0x44, 0x44, 0x44, 0x38, // 6f o
  0x7c, 0x14, 0x14, 0x14, 0x08, // 70 p
  0x08, 0x14, 0x14, 0x18, 0x7c, // 71 q
  0x7c, 0x08, 0x04, 0x04, 0x08, // 72 r
  0x48, 0x54, 0x54, 0x54, 0x20, // 73 s
  0x04, 0x3f, 0x44, 0x40, 0x20, // 74 t
  0x3c, 0x40, 0x40, 0x20, 0x7c, // 75 u
  0x1c, 0x20, 0x40, 0x20, 0x1c, // 76 v
  0x3c, 0x40, 0x30, 0x40, 0x3c, // 77 w
  0x44, 0x28, 0x10, 0x28, 0x44, // 78 x
  0x0c, 0x50, 0x50, 0x50, 0x3c, // 79 y
  0x44, 0x64, 0x54, 0x4c, 0x44, // 7a z
  0x00, 0x08, 0x36, 0x41, 0x00, // 7b {
  0x00, 0x00, 0x7f, 0x00, 0x00, // 7c |
  0x00, 0x41, 0x36, 0x08, 0x00, // 7d ,
  0x10, 0x08, 0x08, 0x10, 0x08, // 7e left arrow
  0x78, 0x46, 0x41, 0x46, 0x78, // 7f right arrow
  };

// Nokia LCD pin numbers
#define LCD_MOSI  PINB2
#define LCD_SCK   PINB4
#define LCD_CD    PINB1
#define LCD_RESET PINB0

/** Number of columns */
#define LCD_COL 84

/** Number of text rows */
#define LCD_ROW 6
/** Width of the font glyphs (in bytes)
 */
#define DATA_WIDTH 5

/** Width of the displayed character
 *
 * An extra byte is added to the font data for character spacing.
 */
#define CHAR_WIDTH (DATA_WIDTH + 1)



// -------------------------------------------------------------------------------------------------------
// ---------------------------------------- DHT11  library CODE ------------------------------------------
// -------------------------------------------------------------------------------------------------------


void  dht_init(void)
{
   DHT_PIN_INPUT();
   DHT_PIN_HIGH();
}

static int8_t   dht_await_state(uint8_t state)
{
    uint8_t counter = 0;
    while ((!DHT_PIN_READ() == state) && (++counter < DHT_TIMEOUT)) 
     { 
     // Delay 1 cycles
     // 1us at 1.0 MHz CLOCK
     asm volatile (
        "    nop"	"\n"
        );
     };
    return counter;
}


int8_t  dht_read(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t i, j, data[5] = {0, 0, 0, 0, 0};

    // send start sequence 1 LOW pulse for 2 miliseconds
    DHT_PIN_OUTPUT();
    DHT_PIN_LOW();

    // Delay 20 000 cycles
    // 20ms at 1 MHz
    asm volatile (
         "    ldi  r18, 26"	"\n"
         "    ldi  r19, 249"	"\n"
         "1:  dec  r19"	"\n"
         "    brne 1b"	"\n"
         "    dec  r18"	"\n"
         "    brne 1b"	"\n"
          );

    DHT_PIN_HIGH();
    DHT_PIN_INPUT();

    // read response sequence - 1 pulse  as preparation before sending measurement
    if (dht_await_state(0) < 0) return DHT_ERR_TIMEOUT;
    if (dht_await_state(1) < 0) return DHT_ERR_TIMEOUT;
    if (dht_await_state(0) < 0) return DHT_ERR_TIMEOUT;
    

    // read data - 40 bits - HOGH pulse length goves info aboit 0s and 1s 
    for (i = 0; i < 5; ++i) {
        for (j = 0; j < 8; ++j) 
          {
         // wait for next BIT to come
         dht_await_state(1);
         // do left shift
         data[i] <<= 1;
         // received BIT is "1" if waiting for LOW longer than 28 microseconds - 
         // the value = 1 in comparision is experimental for 1MHz clock, 20 for 8MHz
         if (dht_await_state(0) > 1)  data[i] |= 1;   
          };
    };

    // check if checksum matches
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF) ) 
     { 
      *temperature = data[2];
      *humidity = data[0];
    }
    else
    {
      *temperature = 0;
      *humidity = 0;
    };
     
    return DHT_ERR_OK;
}

// -------------------------------------------------------------------------------------------------------
// ----------------------   NOKIA 5110 LCD DISPLAY   library CODE ----------------------------------------
// -------------------------------------------------------------------------------------------------------

/** Transfer data to a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutMSB(uint8_t sck, uint8_t mosi, uint16_t data, uint8_t bits) {
  uint16_t mask = (1 << (bits - 1));
  uint8_t output = (1 << mosi);
  uint8_t clock = (1 << sck);
  while(bits) {
    // Set data
    if(data&mask)
      PORTB |= output;
    else
      PORTB &= ~output;
    // Bring the clock high
    PORTB |= clock;
    // Move to the next bit
    mask = mask >> 1;
    bits--;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  }



/** Send a data byte to the LCD
 *
 * @param data the data byte to send.
 */
void lcdData(uint8_t data) {
  // Bring CD high
  PORTB |= (1 << LCD_CD);
  // Send the data
  sspiOutMSB(LCD_SCK, LCD_MOSI, data, 8);
  }

/** Send a command byte to the LCD
 *
 * @param cmd the command byte to send.
 */
void lcdCommand(uint8_t cmd) {
  // Bring CD low
  PORTB &= ~(1 << LCD_CD);
  // Send the data
  sspiOutMSB(LCD_SCK, LCD_MOSI, cmd, 8);
  }

/** Initialise the LCD
 *
 * Sets up the pins required to communicate with the LCD screen and then does
 * the actual chipset initialisation. The pin numbers to use are defined in
 * @ref hardware.h.
 */
void lcdInit() {
  // Set up the output pins, ensuring they are all 'low' to start
  uint8_t val = (1 << LCD_SCK) | (1 << LCD_MOSI) | (1 << LCD_RESET) | (1 << LCD_CD);
  PORTB &= ~val;
  DDRB |= val;

  // Delay 10 000 cycles
  // 10ms at 1 MHz
   asm volatile (
       "    ldi  r18, 13"	"\n"
       "    ldi  r19, 252"	"\n"
       "1:  dec  r19"	"\n"
       "    brne 1b"	"\n"
       "    dec  r18"	"\n"
       "    brne 1b"	"\n"
       "    nop"	"\n"
      );

  // Do a hard reset on the LCD
  PORTB |= (1 << LCD_RESET);

  // Initialise the LCD
  lcdCommand(0x21);  // LCD Extended Commands.
  lcdCommand(0xA1);  // Set LCD Vop (Contrast).
  lcdCommand(0x04);  // Set Temp coefficent.
  lcdCommand(0x14);  // LCD bias mode 1:48.
  lcdCommand(0x20);  // LCD Normal commands
  lcdCommand(0x0C);  // Normal display, horizontal addressing
  }

/** Clear the screen
 *
 * Clear the entire display.
 *
 * @param invert if true the colors are inverted and the screen will be filled
 *               with black.
 */
void lcdClear(bool invert) {
  uint8_t fill = invert?0xFF:0x00;
  // Set the position
  lcdCommand(0x80);
  lcdCommand(0x40);
  // Fill in the whole display
  for(uint16_t index = 0; index < (LCD_COL * LCD_ROW); index++)
    lcdData(fill);
  }



/** Write a single character BIG DIGIT 11x2x8
 *
 * Display a single ASCII character at the position described by the row and
 * column parameters. Note that the row indicates an 8 pixel high character
 * row while the column represents individual pixels. This code uses a built
 * in character set where each character is 5 pixels wide and adds a single
 * column of pixels as spacing giving a total width of 6 pixels.
 *
 * @param row the row number (0 to 5) to display the character.
 * @param col the column position (0 to 83) for the start of the left side of
 *            the character.
 * @param ch  the character to display. If the character is out of range it
 *            will be replaced with the '?' character.
 * @param invert if true the colors are inverted and the character will be
 *               displayed as white on black.
 */
void lcdPrintBChar(uint8_t row, uint8_t col, uint8_t ch, bool big, bool invert) {

  // Set the starting address
  const uint8_t *chdata = BIG_FONT + (ch * 22);
 
  for(uint8_t rowused = row; rowused < row+2; rowused++) {
    lcdCommand(0x80 | col);
    lcdCommand(0x40 | (rowused % LCD_ROW));
    // And send the column data
   for(uint8_t pixels = 0; pixels < 11; pixels++, chdata++) {
    uint8_t data = pgm_read_byte_near(chdata);
    //double sized font
    lcdData(invert?~data:data);
    if (big) lcdData(invert?~data:data);
    };
  }
  // Add the padding byte
 if(col < LCD_COL)
   lcdData(invert?0xFF:0x00);
  }


// Write small character 5x7

void lcdPrintChar(uint8_t row, uint8_t col, uint8_t ch, bool invert) {

  // Make sure it is on the screen
    if((row>=LCD_ROW)||(col>=LCD_COL))
    return;

  // If the character is invalid replace it with the '?'
  if((ch<0x20)||(ch>0x7f))
    ch = '?'; 

  // Set the starting address
  lcdCommand(0x80 | col);
  lcdCommand(0x40 | (row % LCD_ROW));

  // And send the column data
  const uint8_t *chdata = SMALL_FONT + ((ch - 0x20) * 5);
  for(uint8_t pixels = 0; (pixels < DATA_WIDTH) && (col < LCD_COL); pixels++, col++, chdata++) {
    uint8_t data = pgm_read_byte_near(chdata);
    lcdData(invert?~data:data);
    }
  // Add the padding byte
  if(col < LCD_COL)
    lcdData(invert?0xFF:0x00);
  }




// Print text of small fonts
void lcdPrint(uint8_t row, uint8_t col, const char *str, bool invert) {
  for(;(*str!='\0')&&(col<LCD_COL);col+=CHAR_WIDTH,str++)
    lcdPrintChar(row, col, *str, invert);
   }


// -------------------------------------------------------------------------------------------------------
// ------------------------------------   WATCHDOG INTERRUPT CODE ----------------------------------------
// ------------------------------------   main functions of the program ----------------------------------
// -------------------------------------------------------------------------------------------------------


// Watchdog generated interrupt in program
// used for polling ADC data and displaying LCD info
//
ISR(WDT_vect) {


        // ReEnable the watchdog interrupt, as this gets reset when entering this ISR and 
        // automatically enables the WDE signal that resets the MCU the next time the  timer overflows */
          WDTCR |= (1<<WDIE);       

}



// -------------------------------------------------------------------------------------------------------
// ------------------------------------   MAIN PROGRAM CODE ----------------------------------------
// -------------------------------------------------------------------------------------------------------

int main(void)
{
          const char * my_const_str;
          uint8_t firstdig, seconddig, humidity, temperature;
          uint8_t lasttemperature = 0;
          uint8_t lasthumidity = 0;

          // LCD INITIALIZATION
          lcdInit();


          // LCD CLEAR
          lcdClear(0);

          // print start page
          my_const_str = " Termometr na "; 
          lcdPrint(0, 0, my_const_str, 0);
          my_const_str = "czujniku DHT11"; 
          lcdPrint(1, 0, my_const_str, 0);
          my_const_str = " i kontrolerze"; 
          lcdPrint(2, 0, my_const_str, 0);
          my_const_str = "ATTINY85, temp"; 
          lcdPrint(3, 0, my_const_str, 0);
          my_const_str = "od 0 do +100C"; 
          lcdPrint(4, 0, my_const_str, 0);
          my_const_str = "zasilanie 3V"; 
          lcdPrint(5, 0, my_const_str, 0);


	// Disable all interrupts
	cli();
	
	/* Clear MCU Status Register. Not really needed here as we don't need to know why the MCU got reset. page 44 of datasheet */		
	MCUSR = 0;
	
	/* Disable and clear all Watchdog settings. Nice to get a clean slate when dealing with interrupts */

	WDTCR = (1<<WDCE)|(1<<WDE);	
	WDTCR = 0;

	
         //set timer to 1 sec for measurements of LM35
          // WDTCR = (1<<WDIE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);

          // set timer to 0.5s
          // WDTCR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0);

          // set timer to 4 sec
          WDTCR = (1<<WDIE) | (1<<WDP3);

	// Enable all interrupts.
	sei();

        set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
    
        for (;;) 
        {
        
        sleep_mode();   // go to sleep and wait 4 seconds for WATCHDOG interrupt...


	// Disable all interrupts
	cli();


       // initialize DHT11 temperature & humidity sensor
          dht_init();


        // read value from DHT11 sensor 
          dht_read(&temperature, &humidity);

        // if results are different than last readings, then print the output
          if ((temperature != lasttemperature) || (humidity != lasthumidity))
            {

              // clear the LCD
              lcdClear(0);

              // put text info about temperature
              my_const_str = "  TEMPERATURA"; 
              lcdPrint(0, 0, my_const_str, 0);

              // calculate temperature digits
              firstdig = temperature / 10;
              seconddig = temperature % 10;
 
              // display temperature digits and Celsius sign
              lcdPrintBChar(1, 12, firstdig, 1, 0);
              lcdPrintBChar(1, 34, seconddig, 1, 0);
              lcdPrintBChar(1, 56, 11, 0, 0);
              lcdPrintBChar(1, 66, 10, 0, 0);

              // put text info about Humidity
              my_const_str = "  WILGOTNOSC"; 
              lcdPrint(3, 0, my_const_str, 0);

              // calculate humidity digits
              firstdig = humidity / 10;
              seconddig = humidity % 10;

              // display humidity digits 
              lcdPrintBChar(4, 12, firstdig, 1, 0);
              lcdPrintBChar(4, 34, seconddig, 1, 0);
              my_const_str = "%"; 
              lcdPrint(5, 63, my_const_str, 0);
              
              // copy results to buffers
              lasthumidity = humidity;
              lasttemperature = temperature;

           };   // end of IF
            
        //  next interrupt in 4 seconds 
          sei();  

	}; // End of FOR neverending loop 

};  // end of MAIN

 
