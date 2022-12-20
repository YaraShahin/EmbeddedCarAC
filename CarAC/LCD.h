// Function prototypes defined in header file for the LCD library
// .h files should be included in main app in order to use the library

#define LCD_Dir  DDRD			/* Define LCD data port direction */
#define LCD_Port PORTD		/* Define LCD data port */

#define RS_EN_Dir  DDRB		/* Define RS and En data port direction */
#define RS_EN_Port PORTB  /* Define RS and En port */
#define RS PB0				    /* Define Register Select pin */
#define EN PB1 				    /* Define Enable signal pin */

void LCD_Command( unsigned char cmnd );
void LCD_Char( unsigned char data );
void LCD_Init (void);
void LCD_String (char *str);
void LCD_String_xy (char row, char pos, char *str);
void LCD_Clear();