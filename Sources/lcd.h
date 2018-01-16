/* lcd.h
   Paul Michaud, May 25, 2011
   Use with LCD_EEPROM.S19 loaded into the EEPROM of the HCS12 Workstation */

/* This file can be included in any C file that wants to use the LCD utility routines. 
   Documentation for each of these functions can be found in 
   the "LCD_Utility_Routines" document. */ 
  

/* Structure that defines the functions in the LCD module function table. 
   Also provides a function prototype for each function.*/

typedef struct {
                void   (*LCDUser_clear)(char);  //	Send commands to clear LCD		  $0FE4
                char * (*LCDUser_puts)(char *); //	Send a NULL terminated string on the LCD  $0FE6
                void   (*LCDUser_putc)(char);   //	Send a character to the LCD		  $0FE8
                void   (*LCDUser_cmd)(char);    //	Send a command to LCD controller	  $0FEA
                void   (*LCDUser_init)(void);   //	Initialize LCD display			  $0FEC
                void   (*LCDUser_banner)(void); //	Initalize LCD and display Humber banner	  $0FEE
                void   (*LCDUser_printd)(int);
             }LCDUserFN, *LCDUserFNP;

/* define a pointer to the start of LCD function pointer table
   which begins at $0fe4 */
#define LCDFNP ((LCDUserFNP)0x0fe4) 

/* The following #defines are to allow use of the function names as documented in "LCD_Utility_Routines". */

#define lcd_clear   LCDFNP->LCDUser_clear
#define lcd_puts    LCDFNP->LCDUser_puts
#define lcd_putc    LCDFNP->LCDUser_putc
#define lcd_cmd     LCDFNP->LCDUser_cmd
#define lcd_init    LCDFNP->LCDUser_init
#define lcd_banner  LCDFNP->LCDUser_banner
#define lcd_printd  LCDFNP->LCDUser_printd
