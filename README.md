# EPaper Barcodes
This project creates [Code128B](https://en.wikipedia.org/wiki/Code_128) Barcodes on EPaper displays

This uses the [Adafruit 2.13" HD Tri-Color eInk / ePaper Display FeatherWing - 250x122 RW Panel with SSD1680](https://www.adafruit.com/product/4814) along with an [Adafruit HUZZAH32 – ESP32 Feather Board](https://www.adafruit.com/product/3405)

![A barcode on a display](https://raw.githubusercontent.com/Benjamin-C/EPaperBarcode/main/demo.jpg)

### Usage
Simply send a string of < 17 characters over serial at 115200 baud, and a barcode will be generated and displayed on the screen.

### Configuration
Configuration is done near the top of `main.cpp`
* `PRINT_TEXT` controls wether the text of the barcode is printed under it. Enabled by default. Comment to disable.
* `DEBUG_GEN` controls wether extra debug tools are printed around the barcode. These include blocks at the top showing which bars go with which letters and a 1px dotted line at the bottom. Disabled by default. Uncomment to enable.
* `BARCODE_TOP_BUFF` controls how many pixels of space to have above the barcode. Debug information (if present) goes in this space. Defaults to 0px when not debugging and 8px when debugging.
* `BARCODE_BOTTOM_BUFF` controls how many pixels of space to have below the barcode. The printed text (if present) goes in this space. Defaults to 0px when not printing and 10px when printing.

### Files
* `src/main.cpp` contains the main Arduino source code
* `src/barcode.h` contains the barcode encoding information for each character
* `codes.csv` contains a version of the table from Wikipedia describing the encoding of each character. 
* `barcode.py` converts `codes.csv` into `src/barcode.h`
* `lib/` contains the Adafruit libraries required to compile the project

This project was set up using PlatformIO in VSCode

### License
This project is licensed under the GNU General Public License v3.0. See `LICENSE` for more information.

The Adafruit libraries were originally licensed under The MIT License. See `lib/Adafruit*/LICENSE` for details. 