/*
 * TFT_program.c
 *
 *  Created on: Feb 5, 2024
 *      Author: philo
 */
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "GPIO_interface.h"
//#include "SYSTICK_interface.h"
#include "SPI_interface.h"
#include "TFT_private.h"
#include "TFT_config.h"
#include "TFT_interface.h"
#include "GPTIM_interface.h"

//#define TFT_WIDTH  480
//#define TFT_HEIGHT 320



void HTFT_voidInit(void)
{
	MGPIO_vSetMode(HTFT_A0_PORT,HTFT_A0_PIN,MODE_OUTPUT);
	MGPIO_vSetMode(HTFT_RST_PORT,HTFT_RST_PIN,MODE_OUTPUT);
	//******************************************************/
	MGPIO_vSetOutputMode(HTFT_A0_PORT,HTFT_A0_PIN,OUTPUT_PUSHPULL,SPEED_LOW);
	MGPIO_vSetOutputMode(HTFT_RST_PORT,HTFT_RST_PIN,OUTPUT_PUSHPULL,SPEED_LOW);
	//******************************************************/
	MGPIO_vWritePin(HTFT_RST_PORT,HTFT_RST_PIN,GPIO_HIGH);
	TIM2_voidSetBusyWait(100);
	MGPIO_vWritePin(HTFT_RST_PORT,HTFT_RST_PIN,GPIO_LOW);
	TIM2_voidSetBusyWait(1);
	MGPIO_vWritePin(HTFT_RST_PORT,HTFT_RST_PIN,GPIO_HIGH);
	TIM2_voidSetBusyWait(100);
	MGPIO_vWritePin(HTFT_RST_PORT,HTFT_RST_PIN,GPIO_LOW);
	TIM2_voidSetBusyWait(100);
	MGPIO_vWritePin(HTFT_RST_PORT,HTFT_RST_PIN,GPIO_HIGH);
	TIM2_voidSetBusyWait(120);
	//******************************************************/
	HTFT_voidSendCommand(SLPOUT_CMD);
	TIM2_voidSetBusyWait(100);
	HTFT_voidSendCommand(COLMOD_CMD);
	HTFT_voidSendData(0x05);
	HTFT_voidSendCommand(DISPON_CMD);
	//******************************************************/
	HTFT_voidSendData(0x2E); // Disable write
	HTFT_voidSendData(0x2C); // Enable write
}

static void HTFT_voidSendCommand(u8 Copy_u8Command)
{
	MGPIO_vWritePin(HTFT_A0_PORT,HTFT_A0_PIN ,GPIO_LOW);
	(void)SPI1_u16Transceive(Copy_u8Command);
}


static void HTFT_voidSendData(u8 Copy_u8Data)
{
	MGPIO_vWritePin(HTFT_A0_PORT,HTFT_A0_PIN ,GPIO_HIGH);
	(void)SPI1_u16Transceive(Copy_u8Data);
}

void HTFTs_voidSendCommand(u8 Copy_u8Command)
{
	MGPIO_vWritePin(HTFT_A0_PORT,HTFT_A0_PIN ,GPIO_LOW);
	(void)SPI1_u16Transceive(Copy_u8Command);
}


void HTFTs_voidSendData(u8 Copy_u8Data)
{
	MGPIO_vWritePin(HTFT_A0_PORT,HTFT_A0_PIN ,GPIO_HIGH);
	(void)SPI1_u16Transceive(Copy_u8Data);
}


void HTFT_voidDisplayImage(const u16 * Copy_pu16Image)
{
	/*Send Set X - Y Coordinates */
	HTFT_voidSetWindow(0, 127, 0, 159);
	/*Send Write RAM Command*/
	HTFT_voidSendCommand(0x2C);
	u16 Local_u16LoopCounter;
	u8 Local_u8Data = 0;
	for(Local_u16LoopCounter=0;Local_u16LoopCounter<=20480;Local_u16LoopCounter++)
	{
		Local_u8Data=~(Copy_pu16Image[Local_u16LoopCounter]>>8);
		HTFT_voidSendData(Local_u8Data);
		Local_u8Data=~((u8)Copy_pu16Image[Local_u16LoopCounter]);
		HTFT_voidSendData(Local_u8Data);

	}
}

void HTFT_voidSetWindow(u8 Copy_u8X0, u8 Copy_u8X1, u8 Copy_u8Y0, u8 Copy_u8Y1)
{
	/*Send Set X Address Command*/
	HTFT_voidSendCommand(0x2A);
	HTFT_voidSendData(0);
	HTFT_voidSendData(Copy_u8X0);
	HTFT_voidSendData(0);
	HTFT_voidSendData(Copy_u8X1);
	/*Send Set Y Address Command*/
	HTFT_voidSendCommand(0x2B);
	HTFT_voidSendData(0);
	HTFT_voidSendData(Copy_u8Y0);
	HTFT_voidSendData(0);
	HTFT_voidSendData(Copy_u8Y1);
}


void HTFT_voidFillColor(u8 color) {

	/*Send Set X - Y Coordinates */
		HTFT_voidSetWindow(0, 127, 0, 159);
		/*Send Write RAM Command*/
		HTFT_voidSendCommand(0x2C);
		u16 Local_u16LoopCounter;
		for(Local_u16LoopCounter=0;Local_u16LoopCounter<=20480;Local_u16LoopCounter++)
		{

			HTFT_voidSendData(color);
			HTFT_voidSendData(color);


		}

}

