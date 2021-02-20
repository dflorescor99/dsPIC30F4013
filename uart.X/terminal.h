// codigos utiles para la terminal

#define ESC 		0x1B
#define TERM_reset 		printf("%c[0m",ESC)
#define TERM_clear		printf("%c[2J",ESC)
#define TERM_HOME 		printf("%c[H",ESC)

#define TERM_FORE_bright 	printf("%c[1m",ESC)
#define TERM_FORE_underscore	printf("%c[4m",ESC)
#define TERM_FORE_blink 	printf("%c[5m",ESC)
#define TERM_FORE_reverse	printf("%c[7m",ESC)


#define TERM_CURSOR_X_Y(X,Y) 	printf("%c[%u;%uf",0x1B,X,Y)
	
#define TERM_BACK_black 	printf("%c[40m",ESC)
#define TERM_BACK_red 		printf("%c[41m",ESC)
#define TERM_BACK_green 	printf("%c[42m",ESC)
#define TERM_BACK_yellow 	printf("%c[43m",ESC)
#define TERM_BACK_blue 		printf("%c[44m",ESC)
#define TERM_BACK_purple 	printf("%c[45m",ESC)
#define TERM_BACK_cyan 		printf("%c[46m",ESC)
#define TERM_BACK_white 	printf("%c[47m",ESC)



#define TERM_FORE_black 	printf("%c[30m",ESC)
#define TERM_FORE_red 		printf("%c[31m",ESC)
#define TERM_FORE_green 	printf("%c[32m",ESC)
#define TERM_FORE_yellow 	printf("%c[33m",ESC)
#define TERM_FORE_blue 		printf("%c[34m",ESC)
#define TERM_FORE_purple 	printf("%c[35m",ESC)
#define TERM_FORE_cyan 		printf("%c[36m",ESC)
#define TERM_FORE_white 	printf("%c[37m",ESC)
#define TERM_FORE_reverse 	printf("%c[7m",ESC)
