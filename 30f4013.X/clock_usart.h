void clock_usart(void)
	{
// Configuro clock a 30 MIPS
	// EL PLL =16
	// Configure Oscillator to operate the device at 30Mhz
	// Fosc= (7.37MHz+(7.37MHz*0.015))*16, Fcy=Fosc/4
	// Fosc= 120MHz, Fcy=30MHz, 30MIPS
    
    OSCTUN=0;             		// Tune FRC oscillator le agrego 1.5% a la frecuencia

	// Disable Watch Dog Timer
    RCONbits.SWDTEN=0;

//	// clock switching to incorporate PLL
//    __builtin_write_OSCCONH(0x03);   	// Initiate Clock Switch to Primary
//
//    ESTE BLOQUE NI IDEA QUE HACE         	// Oscillator with PLL (NOSC=0b011)
//    __builtin_write_OSCCONL(0x01);     	// Start clock switching
//    while (OSCCONbits.COSC != 0b011); 	// Wait for Clock switch to occur
//
//				// Wait for PLL to lock
//    while(OSCCONbits.LOCK!=1) {};


	
	//Programacion UART1 a 19200
    U1MODEbits.UARTEN = 0; // Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled

    U1BRG = 97;            //19133 bauds    BRGH = 1, Fcy = 30MHz, BRate = 30.000.000/(16*(U1BRG+1))
   
    U1STAbits.UTXISEL = 0;         // Interrupt after one Tx character is transmitted
    U1STAbits.URXISEL = 0;         // Interrupt after one RX character is received
    U1MODEbits.UARTEN = 1;     // Enable UART
    U1STAbits.UTXEN = 1;         // Enable UART Tx
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;            //Desactivo interrupciones de transmision y recepcion UART1

	}
