/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include "lcd.h"
/* ************************************************************************** */


void LCD_ConfigurePins()
{
    // set coontrol pins as digital outputs.
    tris_LCD_DISP_RS = 0;
    tris_LCD_DISP_RW = 0;
    tris_LCD_DISP_EN = 0;
    
    // disable analog (set pins as digital))
    ansel_LCD_DISP_RS = 0;
    
    // default (IO) function for remapable pins
    rp_LCD_DISP_RS = 0;
    rp_LCD_DISP_RW = 0;
    rp_LCD_DISP_EN = 0;
    
    // data pins will be set as output or input in the corresponding write and read functions
}
void LCD_Init()
{
    LCD_ConfigurePins();
    LCD_InitSequence(displaySetOptionDisplayOn);
}

void LCD_WriteByte(unsigned char bData)
{
    DelayAprox10Us(5);  
	// Configure IO Port data pins as output.
   tris_LCD_DATA &= ~msk_LCD_DATA;
    DelayAprox10Us(5);  
	// clear RW
	lat_LCD_DISP_RW = 0;

// access data as contiguous 8 bits
  //  unsigned int oldVal = lat_LCD_DATA;
//    lat_LCD_DATA = bData | (oldVal & 0xFFFFFF00);
//unsigned int *pLCDData = (unsigned int *)(0xBF886440);
//unsigned int *pLCDData = (unsigned int *)(0xBF886430);
unsigned char *pLCDData = (unsigned char *)(0xBF886430);
*pLCDData = bData;

    DelayAprox10Us(5);   
//    lat_LCD_DATA |= bData;
//    lat_LCD_DATA = bData;


    DelayAprox10Us(5);    
//	lat_LCD_DATA = lat_LCD_DATA & ~((unsigned int)msk_LCD_DATA) | bData;

	// Set E
	lat_LCD_DISP_EN = 1;    

    DelayAprox10Us(5);
	// Clear E
	lat_LCD_DISP_EN = 0;

    DelayAprox10Us(5);
	// Set RW
	lat_LCD_DISP_RW = 1;
}

unsigned char LCD_ReadByte()
{
    unsigned char bData;
	// Configure IO Port data pins as input.
    tris_LCD_DATA |= msk_LCD_DATA;
	// Set RW
	lat_LCD_DISP_RW = 1;

	// set RW
	lat_LCD_DISP_RW = 1;    

    
	// Set E
	lat_LCD_DISP_EN = 1;

    DelayAprox10Us(50);   

    // Clear E
	lat_LCD_DISP_EN = 0;
  	bData = (unsigned char)(prt_LCD_DATA & (unsigned int)msk_LCD_DATA);



	return bData;
}

/* ------------------------------------------------------------ */
/*        LCD_ReadStatus
**
**        Synopsis:
**
**        Parameters:
**
**
**        Return Values:
**                unsigned char - the byte that was read.
**
**        Errors:
**
**
**        Description:
**				Reads the status of the CLP. It clears the RS and calls LCD_ReadByte() function.
**
**
*/
unsigned char LCD_ReadStatus()
{
	// Clear RS
	lat_LCD_DISP_RS = 0;
    
	unsigned char bStatus = LCD_ReadByte();
	return bStatus;
}

/* ------------------------------------------------------------ */
/*        LCD_WaitUntilNotBusy
**
**        Synopsis:
**				LCD_WaitUntilNotBusy()
**        Parameters:
**		
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Waits until the status of the CLP is not busy. This function relies on LCD_ReadStatus().
**
**
*/
void LCD_WaitUntilNotBusy()
{
	unsigned char bStatus;
//	bStatus = LCD_ReadStatus();
    bStatus = 0;
	while (bStatus & mskBStatus)
	{
		DelayAprox10Us(1000);    // 10 ms
		bStatus = LCD_ReadStatus();
	}
}

/* ------------------------------------------------------------ */
/*        LCD_WriteCommand
**
**        Synopsis:
**				LCD_WriteCommand(cmdLcdClear);
**        Parameters:
**				unsigned char bCmd	- the command code byte
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Writes the specified byte as command. When the device is ready it clears the RS and writes byte.
**
**
*/
void LCD_WriteCommand(unsigned char bCmd)
{
	// wait until LCD is not busy
	LCD_WaitUntilNotBusy();

	// 1. Clear RS
	lat_LCD_DISP_RS = 0;

	// 2. Write command byte
	LCD_WriteByte(bCmd);
}

/* ------------------------------------------------------------ */
/*        LCD_WriteDataByte
**
**        Synopsis:
**				LCD_WriteDataByte(pBytes[idx]);
**        Parameters:
**				unsigned char bData		- the data byte
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Writes the specified byte as data. When the device is ready it sets the RS and writes byte.
**
**
*/
void LCD_WriteDataByte(unsigned char bData)
{
	// wait until LCD is not busy
	LCD_WaitUntilNotBusy();

    DelayAprox10Us(500);    
	// 	1. Set RS 
	lat_LCD_DISP_RS = 1;

	// 2. Write data byte
	LCD_WriteByte(bData);
}

/* ------------------------------------------------------------ */
/*        LCD_DisplaySet
**
**        Synopsis:
**				LCD_DisplaySet(displaySetOptionDisplayOn | displaySetOptionCursorOn);
**        Parameters:
**				unsigned char bDisplaySetOptions	- display options
**					Possible options (to be OR-ed)
**						displaySetOptionDisplayOn - display ON
**						displaySetOptionCursorOn - cursor ON
**						displaySetBlinkOn - cursor blink ON
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Sets the display options. 
**
*/
void LCD_DisplaySet(unsigned char bDisplaySetOptions)
{
	LCD_WriteCommand(cmdLcdCtlInit | bDisplaySetOptions);
}


/* ------------------------------------------------------------ */
/*        LCD_DisplayClear
**
**        Synopsis:
**				LCD_DisplayClear();
**        Parameters:
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Clears the display and returns the cursor home (upper left corner).
**
**
*/
void LCD_DisplayClear()
{
	LCD_WriteCommand(cmdLcdClear);
}



/* ------------------------------------------------------------ */
/*        LCD_ReturnHome
**
**        Synopsis:
**				LCD_ReturnHome();
**        Parameters:
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Returns the cursor home (upper left corner).
**
**
*/
void LCD_ReturnHome()
{
	LCD_WriteCommand(cmdLcdRetHome);
}




/* ------------------------------------------------------------ */
/*        LCD_Init
**
**        Synopsis:
**				LCD_Init(displaySetOptionDisplayOn);
**        Parameters:
**				unsigned char bDisplaySetOptions	- display options
**					Possible options (to be OR-ed)
**						displaySetOptionDisplayOn - display ON
**						displaySetOptionCursorOn - cursor ON
**						displaySetBlinkOn - cursor blink ON
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**			Performs the initializing (startup) sequence. It calls LCD_DisplaySet providing bDisplaySetOptions argument.
**
**
*/
void LCD_InitSequence(unsigned char bDisplaySetOptions)
{
	//	wait 40 ms

	DelayAprox10Us(40000);
	// Function Set
	LCD_WriteCommand(cmdLcdFcnInit);
	// Wait ~100 us
	DelayAprox10Us(10);
	// Function Set
	LCD_WriteCommand(cmdLcdFcnInit);
	// Wait ~100 us
	DelayAprox10Us(10);	// Display Set
	LCD_DisplaySet(bDisplaySetOptions);
	// Wait ~100 us
	DelayAprox10Us(10);
	// Display Clear
	LCD_DisplayClear();
	// Wait 1.52 ms
	DelayAprox10Us(160);
    // Entry mode set
	LCD_WriteCommand(cmdLcdEntryMode);
    	// Wait 1.52 ms
	DelayAprox10Us(160);
}

/* ------------------------------------------------------------ */
/*        LCD_DisplayShift
**
**        Synopsis:
**				LCD_DisplayShift(fBtn1Process);
**        Parameters:
**				unsigned char fRight
**						- fTrue in order to shift right
**						- fFalse in order to shift left
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Shifts the display one position right or left, depending on the fRight parameter.
**
**
**
*/
void LCD_DisplayShift(unsigned char fRight)
{
	unsigned char bCmd = cmdLcdDisplayShift | (fRight ? mskShiftRL: 0);
	LCD_WriteCommand(bCmd);
}


/* ------------------------------------------------------------ */
/*        LCD_CursorShift
**
**        Synopsis:
**				LCD_CursorShift(fBtn1Process);
**        Parameters:
**				unsigned char fRight
**						- fTrue in order to shift right
**						- fFalse in order to shift left
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Shifts the cursor one position right or left, depending on the fRight parameter.
**
**
*/
void LCD_CursorShift(unsigned char fRight)
{
	unsigned char bCmd = cmdLcdCursorShift | (fRight ? mskShiftRL: 0);
	LCD_WriteCommand(bCmd);
}


/* ------------------------------------------------------------ */
/*        LCD_WriteStringAtPos
**
**        Synopsis:
**				LCD_WriteStringAtPos(szInfo1, 0, 0);
**        Parameters:
**				char *szLn	- string to be written
**				int idxLine	- line where the string will be written
**				unsigned char idxPos	- the starting position of the string within the line
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Writes the specified string at the specified position on the specified line. 
**				It sets the corresponding write position and then writes data bytes as the device is ready.
**				Strings longer than 0x27 are trimmed.
**
**
*/
void LCD_WriteStringAtPos(char *szLn, int idxLine, unsigned char idxPos)
{
	// crop string to 0x27 chars
	/*int len = strlen(szLn);
    
	if(len > 0x27)
	{
	szLn[0x27] = 0; // trim the string so it contains 40 characters 
		len = 0x27;
	}*/

	// Set write position
	unsigned char bAddrOffset = (idxLine == 0 ? 0: 0x40) + idxPos;
	LCD_SetWriteDdramPosition(bAddrOffset);

	unsigned char bIdx = 0;
	while(bIdx < 16)
	{
		LCD_WriteDataByte(szLn[bIdx]);
		bIdx++;
	}
}
/* ------------------------------------------------------------ */
/*        LCD_SetWriteCgramPosition
**
**        Synopsis:
**				LCD_SetWriteCgramPosition(bAdr);
**        Parameters:
**				unsigned char bAdr	- the write location. The position in CGRAM where the next data writes will put bytes.
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Sets the DDRAM write position. This is the location where the next data write will be performed.
**				Be aware that writing to a location auto-increments the write location.
**
*/
void LCD_SetWriteCgramPosition(unsigned char bAdr)
{
	unsigned char bCmd = cmdLcdSetCgramPos | bAdr;
	LCD_WriteCommand(bCmd);
}




/* ------------------------------------------------------------ */
/*        LCD_WriteBytesAtPosCgram
**
**        Synopsis:
**				LCD_WriteBytesAtPosCgram(userDefArrow, 8, posCgramChar0);
**        Parameters:
**				unsigned char *pBytes	- pointer to the string of bytes
**				unsigned char len		- the number of bytes to be written
**				unsigned char bAdr		- the position in CGRAM where bytes will be written
**
**
**        Return Values:
**                void 
**
**        Errors:
**
**
**        Description:
**				Writes the specified number of bytes to CGRAM starting at the specified position.
**				It sets the corresponding write position and then writes data bytes as the device is ready.
**
*/
void LCD_WriteBytesAtPosCgram(unsigned char *pBytes, unsigned char len, unsigned char bAdr)
{
	// Set write position
	LCD_SetWriteCgramPosition(bAdr);

	// Write the string of bytes that define the character to CGRAM
	unsigned char idx = 0;
	while(idx < len)
	{
		LCD_WriteDataByte(pBytes[idx]);
		idx++;
	}
}

/* *****************************************************************************
 End of File
 */
