///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                              ST7789.c                             ////
////                                                                   ////
////              ST7789 display driver for mikroC compiler            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////               This is a free software with NO WARRANTY.           ////
////                     https://simple-circuit.com/                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#define ST_CMD_DELAY      0x80    // special signifier for command lists

#define ST77XX_NOP        0x00
#define ST77XX_SWRESET    0x01
#define ST77XX_RDDID      0x04
#define ST77XX_RDDST      0x09

#define ST77XX_SLPIN      0x10
#define ST77XX_SLPOUT     0x11
#define ST77XX_PTLON      0x12
#define ST77XX_NORON      0x13

#define ST77XX_INVOFF     0x20
#define ST77XX_INVON      0x21
#define ST77XX_DISPOFF    0x28
#define ST77XX_DISPON     0x29
#define ST77XX_CASET      0x2A
#define ST77XX_RASET      0x2B
#define ST77XX_RAMWR      0x2C
#define ST77XX_RAMRD      0x2E

#define ST77XX_PTLAR      0x30
#define ST77XX_COLMOD     0x3A
#define ST77XX_MADCTL     0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1      0xDA
#define ST77XX_RDID2      0xDB
#define ST77XX_RDID3      0xDC
#define ST77XX_RDID4      0xDD

// Some ready-made 16-bit ('565') color settings:
#define   ST7789_BLACK   0x0000
#define   ST7789_BLUE    0x001F
#define   ST7789_RED     0xF800
#define   ST7789_GREEN   0x07E0
#define   ST7789_CYAN    0x07FF
#define   ST7789_MAGENTA 0xF81F
#define   ST7789_YELLOW  0xFFE0
#define   ST7789_WHITE   0xFFFF

uint8_t
  _width,     ///< Display width as modified by current rotation
  _height,    ///< Display height as modified by current rotation
  _xstart,    ///< Internal framebuffer X offset
  _ystart,    ///< Internal framebuffer Y offset
  _colstart,  ///< Some displays need this changed to offset
  _rowstart,  ///< Some displays need this changed to offset
  rotation;   ///< Display rotation (0 thru 3)


// SCREEN INITIALIZATION ***************************************************

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.

#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 80

const uint8_t
  cmd_240x240[] =  {                // Init commands for 7789 screens
    9,                              //  9 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      150,                          //    150 ms delay
    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      255,                          //     255 = 500 ms delay
    ST77XX_COLMOD , 1+ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    ST77XX_MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST77XX_CASET  , 4,              //  5: Column addr set, 4 args, no delay:
      0x00,
      ST7789_240x240_XSTART,        //     XSTART = 0
      (240+ST7789_240x240_XSTART)>>8,
      (240+ST7789_240x240_XSTART)&0xFF,  //     XEND = 240
    ST77XX_RASET  , 4,              //  6: Row addr set, 4 args, no delay:
      0x00,
      ST7789_240x240_YSTART,             //     YSTART = 0
      (240+ST7789_240x240_YSTART)>>8,
      (240+ST7789_240x240_YSTART)&0xFF,  //     YEND = 240
    ST77XX_INVON  ,   ST_CMD_DELAY,  //  7: hack
      10,
    ST77XX_NORON  ,   ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
    255 };                          //     255 = max (500 ms) delay

//*************************** User Functions ***************************//
void tft_init(void);

void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
void drawVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void fillScreen(uint16_t color);
void setRotation(uint8_t m);
void invertDisplay(bool i);
void pushColor(uint16_t color);

//************************* Non User Functions *************************//
#ifdef ST7789_SOFT_SPI
void ST7789_SPI_Write(uint8_t d);
#endif
void displayInit(const uint8_t *addr);
void writeCommand(uint8_t cmd);
void setAddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/////////////////////////////////////////////////////////////////////////

#ifdef ST7789_SOFT_SPI
void ST7789_SPI_Write(uint8_t d) {
  uint8_t bit_n;
  for (bit_n = 0x80; bit_n; bit_n >>= 1) {
    TFT_SCK = 0;
    if (d & bit_n) TFT_DIN = 1;
    else           TFT_DIN = 0;
    TFT_SCK = 1;
  }
}
#endif

/**************************************************************************/
/*!
    @brief  Write a single command byte to the display. Chip-select and
            transaction must have been previously set -- this ONLY sets
            the device to COMMAND mode, issues the byte and then restores
            DATA mode. There is no corresponding explicit writeData()
            function -- just use ST7789_SPI_Write().
    @param  cmd  8-bit command to write.
*/
/**************************************************************************/
void writeCommand(uint8_t cmd) {
  TFT_DC = 0;
  ST7789_SPI_Write(cmd);
  TFT_DC = 1;
}

/**************************************************************************/
/*!
    @brief  Companion code to the initiliazation tables. Reads and issues
            a series of LCD commands stored in ROM byte array.
    @param  addr  Flash memory array with commands and data to send
*/
/**************************************************************************/
void displayInit(const uint8_t *addr){
  uint8_t  numCommands, numArgs;
  uint16_t ms;

  #ifdef TFT_CS
    TFT_CS = 0;
  #endif

  numCommands = *addr++;   // Number of commands to follow
  
  while(numCommands--) {                 // For each command...

    writeCommand(*addr++); // ReaST7789_SPI_Writed
    numArgs  = *addr++;    // Number of args to follow
    ms       = numArgs & ST_CMD_DELAY;   // If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;            // Mask out delay bit
    while(numArgs--) {                   // For each argument...
      ST7789_SPI_Write(*addr++);   // Read, issue argument
    }

    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      while(ms--) delay_ms(1);
    }
  }
  #ifdef TFT_CS
    TFT_CS = 1;
  #endif
}

/**************************************************************************/
/*!
    @brief  Initialization code for ST7789 display
*/
/**************************************************************************/
void tft_init(void) {
  #ifdef TFT_RST
    TFT_RST = 1;
    #ifdef TFT_RST_DIR
      TFT_RST_DIR = 0;
    #endif
    delay_ms(100);
    TFT_RST = 0;
    delay_ms(100);
    TFT_RST = 1;
    delay_ms(200);
  #endif

  #ifdef TFT_CS
    TFT_CS = 1;
    #ifdef TFT_CS_DIR
      TFT_CS_DIR = 0;
    #endif
  #endif

  #ifdef TFT_DC_DIR
    TFT_DC_DIR = 0;
  #endif
  
  #ifdef TFT_DIN_DIR
    TFT_DIN_DIR = 0;
  #endif
  #ifdef TFT_SCK_DIR
    TFT_SCK_DIR = 0;
  #endif

  displayInit(cmd_240x240);

  _colstart = ST7789_240x240_XSTART;
  _rowstart = ST7789_240x240_YSTART;
  _height   = 240;
  _width    = 240;
  setRotation(2);
}

/**************************************************************************/
/*!
  @brief  SPI displays set an address window rectangle for blitting pixels
  @param  x  Top left corner x coordinate
  @param  y  Top left corner x coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
/**************************************************************************/
void setAddrWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  x += _xstart;
  y += _ystart;

  writeCommand(ST77XX_CASET); // Column addr set
  ST7789_SPI_Write(0);
  ST7789_SPI_Write(x);
  ST7789_SPI_Write(0);
  ST7789_SPI_Write(x+w-1);

  writeCommand(ST77XX_RASET); // Row addr set
  ST7789_SPI_Write(0);
  ST7789_SPI_Write(y);
  ST7789_SPI_Write(0);
  ST7789_SPI_Write(y+h-1);

  writeCommand(ST77XX_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief  Set origin of (0,0) and orientation of TFT display
    @param  m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void setRotation(uint8_t m) {
  uint8_t madctl = 0;

  rotation = m & 3; // can't be higher than 3

  switch (rotation) {
   case 0:
     madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
     _xstart = _colstart;
     _ystart = _rowstart;
     break;
   case 1:
     madctl  = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
     _xstart = _rowstart;
     _ystart = _colstart;
     break;
  case 2:
     madctl  = ST77XX_MADCTL_RGB;
     _xstart = 0;
     _ystart = 0;
     break;
   case 3:
     madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
     _xstart = 0;
     _ystart = 0;
     break;
  }

  #ifdef TFT_CS
    TFT_CS = 0;
  #endif
  writeCommand(ST77XX_MADCTL);
  ST7789_SPI_Write(madctl);
  #ifdef TFT_CS
    TFT_CS = 1;
  #endif
}

void drawPixel(uint8_t x, uint8_t y, uint16_t color) {
  if((x < _width) && (y < _height)) {
    #ifdef TFT_CS
      TFT_CS = 0;
    #endif
    setAddrWindow(x, y, 1, 1);
    ST7789_SPI_Write(color >> 8);
    ST7789_SPI_Write(color & 0xFF);
    #ifdef TFT_CS
      TFT_CS = 1;
    #endif
  }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color) {
  if( (x < _width) && (y < _height) && w) {   
    uint8_t hi = color >> 8, lo = color;

    if((x + w - 1) >= _width)  
      w = _width  - x;
    #ifdef TFT_CS
      TFT_CS = 0;
    #endif
    setAddrWindow(x, y, w, 1);
    while (w--) {
    ST7789_SPI_Write(hi);
    ST7789_SPI_Write(lo);
    }
    #ifdef TFT_CS
      TFT_CS = 1;
    #endif
  }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly vertical line (this is often optimized in a subclass!)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color) {
  if( (x < _width) && (y < _height) && h) {  
    uint8_t hi = color >> 8, lo = color;
    if((y + h - 1) >= _height) 
      h = _height - y;
    #ifdef TFT_CS
      TFT_CS = 0;
    #endif
    setAddrWindow(x, y, 1, h);
    while (h--) {
      ST7789_SPI_Write(hi);
      ST7789_SPI_Write(lo);
    }
    #ifdef TFT_CS
      TFT_CS = 1;
    #endif
  }
}

/**************************************************************************/
/*!
   @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  if( (x < _width) && (y < _height) && w && h) {                            // Nonzero width and height?
    uint8_t hi = color >> 8, lo = color;
    uint16_t px;
    if((x + w - 1) >= _width)  
      w = _width  - x;
    if((y + h - 1) >= _height) 
      h = _height - y;
    #ifdef TFT_CS
      TFT_CS = 0;
    #endif
    setAddrWindow(x, y, w, h);
    px = (uint16_t)w * h;
    while (px--) {
      ST7789_SPI_Write(hi);
      ST7789_SPI_Write(lo);
    }
    #ifdef TFT_CS
      TFT_CS = 1;
    #endif
  }
}

/**************************************************************************/
/*!
   @brief    Fill the screen completely with one color. Update in subclasses if desired!
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

/**************************************************************************/
/*!
    @brief  Invert the colors of the display (if supported by hardware).
            Self-contained, no transaction setup required.
    @param  i  true = inverted display, false = normal display.
*/
/**************************************************************************/
void invertDisplay(bool i) {
    #ifdef TFT_CS
      TFT_CS = 0;
    #endif
    writeCommand(i ? ST77XX_INVON : ST77XX_INVOFF);
    #ifdef TFT_CS
      TFT_CS = 1;
    #endif
}

/*!
    @brief  Essentially writePixel() with a transaction around it. I don't
            think this is in use by any of our code anymore (believe it was
            for some older BMP-reading examples), but is kept here in case
            any user code relies on it. Consider it DEPRECATED.
    @param  color  16-bit pixel color in '565' RGB format.
*/
void pushColor(uint16_t color) {
    uint8_t hi = color >> 8, lo = color;
    #ifdef TFT_CS
      TFT_CS = 0;
    #endif
    ST7789_SPI_Write(hi);
    ST7789_SPI_Write(lo);
    #ifdef TFT_CS
      TFT_CS = 1;
    #endif
}

// end of code.