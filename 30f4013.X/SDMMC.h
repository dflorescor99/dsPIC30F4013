/*
** 
** SD card interface 
**
*/

#define FALSE   0
#define TRUE    !FALSE
#define FAIL    0

// IO definitions

#define READ_LED                    _RF0	// Led indicador de lectura (output) 
#define SDCD    		_RF1    	// Card Detect (input) 
#define SDCS    		_RF4    	// Card Select (output)
#define WRITE_LED                   _RF5	// Led indicador de escritura (output)
#define POWER_SD                    _RF12	// Card Power (output)
#define SD_WRITE_PROTECT            _RF13	// Card WRITE PROTECT (input)

// SD card commands
#define RESET                       0 // a.k.a. GO_IDLE (CMD0)
#define INIT                        1 // a.k.a. SEND_OP_COND (CMD1)
#define SEND_CSD                    9
#define SEND_CID                    10
#define SET_BLEN                    16
#define READ_SINGLE                 17
#define WRITE_SINGLE                24
#define APP_CMD                     55
#define SEND_APP_OP                 41

// SD card responses
#define DATA_START                  0xFE
#define DATA_ACCEPT                 0x05

#define readSPI()   writeSPI( 0xFF)
#define clockSPI()  writeSPI( 0xFF)
#define disableSD() SDCS = 1; clockSPI()
#define enableSD()  SDCS = 0

typedef unsigned long LBA;      // logic block address, 32 bit wide

void initSD( void);     // initializes the I/O pins for the SD/MMC interface 
int initMedia( void);   // initializes the SD/MMC memory device


int detectSD( void);    // detects the card presence 
int detectWP( void);    // detects the position of the write protect switch

int readSECTOR ( LBA, char *);  // reads a block of data 
int writeSECTOR( LBA, char *);  // writes a block of data


void initSPI( void)
{
  
    SDCS = 1;
       
    // init the spi module for a slow (safe) clock speed first
    SPI1CON1 = 0x013c;  // CKE=1; CKP=0, sample middle, prescale 1:64

    SPI1STAT = 0x8000;  // enable the peripheral

}   // initSD


// send one byte of data and receive one back at the same time
unsigned char writeSPI( unsigned char b)
{
    SPI1BUF = b;					// write to buffer for TX
    while( !SPI1STATbits.SPIRBF);	// wait for transfer to complete
    return SPI1BUF;    				// read the received value
}// writeSPI




int detectSD( void)
{
    return ( !SDCD);  
} // detect SD

int detectWP( void)
{
    return ( !SD_WRITE_PROTECT);  
} // detect WP


int sendSDCmd( unsigned char c, LBA a)
{
    int i, r;

    // enable SD card
    enableSD();

    // send a comand packet (6 bytes)
    writeSPI( c | 0x40);    // send command 
    writeSPI( a>>24);       // msb of the address
    writeSPI( a>>16);       
    writeSPI( a>>8);
    writeSPI( a);           // lsb
    
    writeSPI( 0x95);        // send CMD0 CRC 

    // now wait for a response (allow for up to 8 bytes delay)
    i = 9;
    do {
        r = readSPI();      // check if ready   
        if ( r != 0xFF) break;
    } while ( --i > 0);

    return ( r);         

/* return response
    FF - timeout 
    00 - command accepted
    01 - command received, card in idle state after RESET

other codes:
    bit 0 = Idle state
    bit 1 = Erase Reset
    bit 2 = Illegal command
    bit 3 = Communication CRC error
    bit 4 = Erase sequence error
    bit 5 = Address error
    bit 6 = Parameter error
    bit 7 = Always 0
*/
    // NOTE CSCD is still low
} // sendSDCmd


int readSECTOR( LBA a, char *p)
// a        LBA of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    int r, i;
    
    READ_LED = 1;

    r = sendSDCmd( READ_SINGLE, ( a << 9));
    if ( r == 0)    // check if command was accepted
    {  
        // wait for a response
        i = 10000;
        do{
            r = readSPI();     
            if ( r == DATA_START) break;
        }while( --i>0);

        // if it did not timeout, read a 512 byte sector of data
        if ( i)
        {
//  Compiler version 3.01b Work around

           register unsigned i;
           register char * q;
 //        register unsigned i asm( "w5");
 //        register char * q asm("w6");
           q = p;
           i = 512;

            do{ 
                SPI1BUF = 0xFF;					// write to buffer for TX
                while( !(SPI1STAT&1));	// wait for transfer to complete
                *q++ = SPI1BUF;    				// read the received value
            } while (--i>0);

            // ignore CRC
            readSPI();
            readSPI();

        } // data arrived

    } // command accepted

    // remember to disable the card
    disableSD();
    READ_LED = 0;

    return ( r == DATA_START);      // return TRUE if successful
} // readSECTOR


int writeSECTOR( LBA a, char *p)
// a        LBA of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    unsigned r, i;
    
    WRITE_LED = 1;

    r = sendSDCmd( WRITE_SINGLE, ( a << 9));
    if ( r == 0)    // check if command was accepted
    {  
        writeSPI( DATA_START);
        for( i=0; i<512; i++)
            writeSPI( *p++);

        // send dummy CRC
        clockSPI();
        clockSPI();
    
        // check if data accepted
        if ( (r = readSPI() & 0xf) == DATA_ACCEPT)
        {   
            for( i=0xffff; i>0; i--)
            { // wait for end of write
                if ( (r = readSPI()))
                    break;
            } 
        } // accepted
        else
            printf("\r\n\nFAIL");

    } // command accepted

    // remember to disable the card
    disableSD();
    WRITE_LED = 0;

    return ( r);      // return TRUE if successful

} // writeSECTOR



int initMedia( void)
{
    int i, r;

    // 1. with the card not selected     
    disableSD(); 

    // 2. send 80 clock cycles start up
    for ( i=0; i<10; i++)
        clockSPI();

    // 3. now select the card
    enableSD();

    // 4. send a reset command
    r = sendSDCmd( RESET, 0); disableSD();
    if ( r != 1)  
        return 0x84;

    // 5. send repeatedly INIT until receive a 0 
    i = 10000;  // up to .3 sec
    do {
        r = sendSDCmd( INIT, 0); disableSD();
        if ( !r) break; 
    } while( --i > 0);
    if ( i==0)   
        return 0x85;       // timed out 
    
    // 10. increase speed 
    SPI1STAT = 0;           // disable momentarily the SPI1 module
//    SPI1CON1 = 0x013E;      // change prescaler to 1:4
    SPI1CON1 = 0x013C;      // change prescaler to 1:64
    SPI1STAT = 0x8000;      // re-enable the SPI1 module
    
    return 0;           

} // init media
