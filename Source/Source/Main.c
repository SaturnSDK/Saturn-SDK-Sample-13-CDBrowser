#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <VDP.h>
#include <Debug.h>
#include <Peripheral.h>
#include <USBCart.h>
#include <CDC.h>

uint16_t Color16( uint8_t p_Red, uint8_t p_Green, uint8_t p_Blue );

void main( void )
{
	unsigned char *pDestination;
	unsigned int Index;
	uint32_t VInt;
	uint16_t PadNew, PadOld, PadDelta;
	uint16_t Red = 3, Green = 11, Blue = 19;
	uint32_t FrameCount = 0U;
	uint16_t AuthType;

	VDP_Initialize( );
	PER_Initialize( );
	DBG_Initialize( );

	USB_Print( "SEGA Saturn SDK Sample 13 - CD Browser\n" );

	VDP1_TVMR = 0x0000;
	VDP1_FBCR = 0x0000;
	VDP1_PTMR = 0x0002;
	VDP1_EWUL = 0x0000;
	VDP1_EWLR = 0x28EF;
	VDP1_EWDR = Color16( 31, 31, 31 );

	/* Enable the TV screen, 320x240NI */
	VDP2_TVMD = 0x8010;

	PadNew = 0x0000;
	PadOld = 0x0000;

	/* First color in VRAM set to a SEGA blue */
	VDP2_SetBackgroundColor( 3, 11, 19 );

	DBG_Print( 3, 26, 0xF0, "SEGA Saturn Sample 13 - CD Browser" );
	DBG_Print( 8, 27, 0xF0, "[saturnsdk.github.io]" );

	/* Initialise the CD block */
	if( CDC_CDInitialise( ) != CDC_ERROR_OK )
	{
		DBG_Print( 3, 3, 0xF0, "FAILED TO INITIALISE CD BLOCK" );
	}
	else
	{
		DBG_Print( 3, 3, 0xF0, "CD BLOCK INITIALISED" );
	}

	if( CDC_CDAuthenticate( ) != CDC_ERROR_OK )
	{
		char Buffer[ 80 ];
		CDC_CDIsAuthenticated( &AuthType );
		sprintf( Buffer,"FAILED TO AUTHENTICATE CD BLOCK: 0x%04X", AuthType );
		DBG_Print( 3, 4, 0xF0, Buffer );
	}
	else
	{
		char Buffer[ 80 ];
		CDC_CDIsAuthenticated( &AuthType );
		sprintf( Buffer,"CD BLOCK AUTHENTICATED: 0x%04X", AuthType );
		DBG_Print( 3, 4, 0xF0, Buffer );
	}

	for( ;; )
	{
		char PrintBuffer[ 80 ];
		size_t StringSize;

		VDP_WaitVBlankOut( );
		VDP_WaitVBlankIn( );

		VDP1_FBCR = 0x0001;
			
		/* Poll peripherals */
		PadOld = PadNew;
		PadNew = PER_Read( 0 );
		PadDelta = ( PadNew ^ PadOld ) & PadNew;

		if( PadNew & PER_START )
		{
			break;
		}

		++FrameCount;
	}

	PER_Shutdown( );
	VDP_Shutdown( );
}

uint16_t Color16( uint8_t p_Red, uint8_t p_Green, uint8_t p_Blue )
{
	return	( ( ( p_Red ) & 0x1F ) |
			( ( p_Green ) & 0x1F ) << 5 |
			( ( p_Blue ) & 0x1F ) << 10 |
			0x8000 );
}

