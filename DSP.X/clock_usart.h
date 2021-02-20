void clock_usart(void)
	{
// Configuro clock a 40 MIPS
	// Configure PLL prescaler, PLL postscaler, PLL divisor
	// Configure Oscillator to operate the device at 40Mhz
	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	// Fosc= 8M*40/(2*2)=80Mhz for 8M input clock

    PLLFBD=38;                    // M=40

    CLKDIVbits.PLLPOST=0;        	// N1=2
    CLKDIVbits.PLLPRE=0;        	// N2=2
    OSCTUN=0;             		// Tune FRC oscillator, if FRC is used

	// Disable Watch Dog Timer
    RCONbits.SWDTEN=0;

	// clock switching to incorporate PLL
    __builtin_write_OSCCONH(0x03);   	// Initiate Clock Switch to Primary

                                       	// Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(0x01);     	// Start clock switching
    while (OSCCONbits.COSC != 0b011); 	// Wait for Clock switch to occur

				// Wait for PLL to lock
    while(OSCCONbits.LOCK!=1) {};


	
	//Programacion UART1 a 115200
    U1MODEbits.UARTEN = 0; // Disable UART

    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1MODEbits.BRGH = 1; // Low Speed mode 115200
	//U1MODEbits.BRGH = 0; // 19200

    U1BRG = 86;            //114942 bauds    BRGH = 1, Fcy = 40MHz, BRate = 40.000.000/(4*(U1BRG+1))
	//U1BRG = 129;	// 19200
   
    U1STAbits.UTXISEL0 = 0;         // Interrupt after one Tx character is transmitted
    U1STAbits.UTXISEL1 = 0;
    U1STAbits.URXISEL = 0;         // Interrupt after one RX character is received
    U1MODEbits.UARTEN = 1;     // Enable UART
    U1STAbits.UTXEN = 1;         // Enable UART Tx
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;            //Desactivo interrupciones de transmision y recepcion UART1

	}
