/* Code128 Barcode generator for Adafruit 2.13" HD Tri-Color eInk / ePaper Display FeatherWing and Adafruit ESP32 Feather
 * Reads strings from the serial port and creates them as barcodes
 * All code in this file by Benjamin Crall - GNU General Public License v3.0
 * All libraries by Adafruit - used under The MIT License */

#include <Arduino.h>

#include "Adafruit_ThinkInk.h"
#include "codes.h" // the Code128 barcodes, generated from the Wikipedia table copied into codes.csv by barcode.py

/* ================================
 *   Start Configuration
 * ================================ */
// Comment the next line to disable printing the text below the barcode
#define PRINT_TEXT // Flag to say if we are printing the text below the barcode
// Sets the buffer in px above and below the barcode. Printed text is in bottom buff
#ifndef PRINT_TEXT
// The value if we are not printing the text
#define BARCODE_BOTTOM_BUFF 0 // The distance from the bottom of the barcode to the bottom of the screen
#else
// The value if we are printing the text. Leave space for the text
#define BARCODE_BOTTOM_BUFF 10 // The distance from the bottom of the barcode to the bottom of the screen
#endif

// Comment the next line to disable drawing the debug extra content
// #define DEBUG_GEN // Flag to set if we draw more debug info when generating barcodes
// Sets the buffer in px above and below the barcode. Printed text is in bottom buff
#ifndef DEBUG_GEN
// The value if we are not debugging
#define BARCODE_TOP_BUFF 0 // The distance from the top of the screen to the top of the barcode
#else
// The value if we are debugging. Leave space for the blocks above the code
#define BARCODE_TOP_BUFF 8 // The distance from the top of the screen to the top of the barcode
#endif
/* ================================
 *   End Configuration
 * ================================ */

// Pin definitions
#define SRAM_CS     32
#define EPD_CS      15
#define EPD_DC      33  
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)
ThinkInk_213_Tricolor_RW display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/** Draw some text
 *  @param text  The text to draw
 *  @param color The color to draw in. Use EPD_BLACK, EPD_WHITE, EPD_RED
 *  @param x     The x position of the top left corner of the text in px
 *  @param y     The y position of the top left corner of the text in px
 */
void drawText(const char *text, uint16_t color, int x, int y) {
    display.setCursor(x, y);
    display.setTextColor(color);
    display.setTextWrap(true);
    display.print(text);
}

/** Converts the lower 4 bits of num to an ascii character
 * @param num The number to convert (lower 4 bits only)
 * @return    The number 0-9 or the letter A-F  
 */
char hexChar(uint8_t num) {
    num = num & 0xF;
    if(num < 0xA) {
        return num + '0';
    } else {
        return num + 0x37; // 0x37 is the offset for 'A' - 10
    }
}

/** Draws a byte as hex on the screen
 *  @param num   The number to draw
 *  @param color The color to draw in. Use EPD_BLACK, EPD_WHITE, EPD_RED
 *  @param x     The x position of the top left corner of the text in px
 *  @param y     The y position of the top left corner of the text in px
 */
void drawHex(uint8_t num, uint16_t color, int x, int y) {
    char chars[] = {'x', 'x', 0x00};
    chars[1] = hexChar(num);
    chars[0] = hexChar(num >> 4);
    drawText(chars, color, x, y);
}

/** Draws a Code128B barcode on the screen
 *  The barcode will be centered horizontally and positioned vertically by BARCODE_BOTTOM_BUFF and BARCODE_TOP_BUFF
 *  @param text the text to print, max 17 characters. Any printable ascii character is allowed. Additional characters will be ignored
 */
void drawCode128(const char* text) {
    Serial.print("Drawing barcode for text \"");
    Serial.print(text);
    Serial.println("\"");
    // There can be a max of 22 characters that fit on the screen. 3 are reserved leaving 17 for the data. An additional space is added for a trailing null;
    uint16_t code[23]; // The symbols to draw
    int cp = 0; // The position in the code we are
    // Add the start code
    code[cp++] = 0x690; // Start code B so we can use all the letters and numbers
    // Set up the checksum
    int checksum = 104; // start code B value

    Serial.println("Generating code ..."); 
    // Add all the characters to the code
    for(int i = 0; i < 17; i++) {
        if(text[i] == 0x00) {
            break;
        }
        int val = text[i]-32;
        code[cp++] = barpatterns[val];
        checksum += val * (i+1);
#ifdef DEBUG_GEN
        Serial.print(text[i]);
        Serial.print(" ");
        Serial.print(val, DEC);
        Serial.print(" ");
        Serial.print(barpatterns[val], BIN);
        Serial.print(" ");
        Serial.print(val * i+1);
        Serial.print(" ");
        Serial.print(checksum);
        Serial.println();
#endif
    }

    Serial.print("Calculating checksum ... ");
    // Calculate the checksum and add it
    checksum %= 103;
    Serial.println(checksum, HEX);
    code[cp++] = barpatterns[checksum];
    // Add the stop code and null terminator
    code[cp++] = 0x63A; // Stop symbol
    code[cp] = 0; // Finishing null

    Serial.println("Rendering ...");
    // Render the barcode to the screen at 1:1 scale
    int x = display.width()-(((display.width()-1)-(cp*11)+2)/2); // X pos on screen
    display.clearBuffer();
    // Draw the ending bar
    for(int i = 0; i < 2; i++) {
        display.drawLine(x, BARCODE_TOP_BUFF, x, display.height()-BARCODE_BOTTOM_BUFF, EPD_BLACK);
        x--;
    }
    for(int i = cp-1; i >= 0; i--) {
        // Render each bar
        uint16_t s = code[i];
        for(int j = 0; j < 11; j++) {
            if(s & 1) {
                display.drawLine(x, BARCODE_TOP_BUFF, x, display.height()-BARCODE_BOTTOM_BUFF, EPD_BLACK);
            }
            s >>= 1;
            x--;
        }
#ifdef DEBUG_GEN
        // Draw a bar at the top which has the letter in each spot
        if(i % 2 == 0) {
            for(int j = 0; j < BARCODE_TOP_BUFF; j++) {
                display.drawLine(x+1, j, x+11, j, EPD_BLACK);
            }
        }
        if(i < cp-2 && i > 0) {
            display.drawChar(x+1, 0, text[i-1], EPD_RED, EPD_RED, 1);
            Serial.print(text[i-1]);
        }
#endif
    }
    Serial.println("\nAdding text to bottom");
#ifdef PRINT_TEXT
    drawText(text, EPD_RED, ((display.width()-1)-(cp*6)+2)/2, display.height()-8);
#endif
#ifdef DEBUG_GEN
    for(int i = 0; i < display.width(); i+=2) {
        display.drawPixel(i, display.height()-BARCODE_BOTTOM_BUFF, EPD_RED);
    }
#endif
    Serial.println("Showing");
    display.display();
    Serial.println("Done");
}

/** Initialization of the microcontroller and screen.
 *  Doesn't clear the screen now
 */
void setup() {
    Serial.begin(115200);
    Serial.println("2.13 inch EInk Featherwing Code128 Barcode Generator");
    display.begin();
    delay(100);
}

#define MAX_STRING_LENGTH 18   // The maximum printable length of a string + a null terminator
char input[MAX_STRING_LENGTH]; // Buffer to hold the string 

/** Main program loop
 *  Waits for user input over serial then creates that barcode and shows it
 */
void loop() {
    // Read the string from the serial input
    int i = 0;
    while(Serial.available() > 0 && i < MAX_STRING_LENGTH - 1) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            // End the input string if newline or carriage return is received
            input[i] = '\0';
            break;
        }
        // Add the received character to the string
        input[i] = c;
        i++;
        // Don't check too often, not that it matters
        delay(10);
    }

    if(i > 0) {
        // Draw the barcode but only if we actually have a string to show
        drawCode128(input);
    }
}