/* PROJECT ON STM32F401
 * showing picture on TFT and playing song with it
 * Changing the song with IR remote Control
 * Additional: can show the name in Led Matrix
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "MRCC_interface.h"
#include "GPIO_interface.h"
#include "EXTI_interface.h"
#include "NVIC_interface.h"
#include "GPTIM_interface.h"
#include "SYSTICK_interface.h"
#include "SPI_interface.h"
#include "TFT_interface.h"
#include "R2RDAC_INTERFACE.h"
//#include "song.h"
#include "messi.h"
//#include "messi2.h"
#include "abo.h"
#include "img.h"
#include "img2.h"

// Flag to interrupt the song while playing
u8 flag = 0;

//#define DAC_DATA_LENGTH 37731
#define messi_DATA_LENGTH 70211
//#define abotrika_DATA_LENGTH 56928
#define abo_DATA_LENGTH 76825

// Variables for IR remote control
volatile u8   Local_u8StartFlag       = 0;
volatile u32  Local_u32FrameData[50]  = {0};
volatile u8   Local_u8EdgeCounter     = 0;
volatile u8   Localu8Data             = 0;

// Function declarations
void voidPlay(void);
void voidTakeAction(void);
void voidGetFrame(void);
void System_Init(void);
void GPIO_Init(void);

// Function to play songs based on IR remote signals
void voidPlay(void) {
    switch (Localu8Data) {
        case 69:
            // Stop
            MGPIO_vWritePin(PORT_A, PIN_1, GPIO_LOW);
            MGPIO_vWritePin(PORT_A, PIN_2, GPIO_LOW);
            MGPIO_vWritePin(PORT_A, PIN_3, GPIO_LOW);
            break;
        case 18:  // vol+
        case 17:
            // Increase volume
            MGPIO_vWritePin(PORT_A, PIN_1, GPIO_HIGH);
            MGPIO_vWritePin(PORT_A, PIN_2, GPIO_LOW);
            MGPIO_vWritePin(PORT_A, PIN_3, GPIO_LOW);
            flag = 1;
            HTFT_voidDisplayImage(img);
            R2RDAC_SetData(abo_raw, abo_DATA_LENGTH, &flag);
            break;
        case 26:  // vol-
            // Decrease volume
            MGPIO_vWritePin(PORT_A, PIN_1, GPIO_LOW);
            MGPIO_vWritePin(PORT_A, PIN_2, GPIO_HIGH);
            MGPIO_vWritePin(PORT_A, PIN_3, GPIO_LOW);
            flag = 1;
            HTFT_voidDisplayImage(img2);
            R2RDAC_SetData(messi_raw, messi_DATA_LENGTH, &flag);
            break;
        case 6:
            // OK
            MGPIO_vWritePin(PORT_A, PIN_1, GPIO_LOW);
            MGPIO_vWritePin(PORT_A, PIN_2, GPIO_LOW);
            MGPIO_vWritePin(PORT_A, PIN_3, GPIO_HIGH);
            break;
    }
}

// Function to handle actions based on received IR signals
void voidTakeAction(void) {
    u8 i;
    Localu8Data = 0;

    if ((Local_u32FrameData[0] >= 10000) && (Local_u32FrameData[0] <= 15000)) {
        for (i = 0; i < 8; i++) {
            if ((Local_u32FrameData[17 + i] >= 2000) && (Local_u32FrameData[17 + i] <= 2300)) {
                SET_BIT(Localu8Data, i);
            } else {
                CLR_BIT(Localu8Data, i);
            }
        }
        // Flag is zero as the song can be interrupted in the middle
        flag = 0;
        voidPlay();
    } else {
        // Invalid Frame
    }

    Local_u8StartFlag     = 0;
    Local_u32FrameData[0] = 0;
    Local_u8EdgeCounter   = 0;
}

// Function to get the frame data from IR remote
void voidGetFrame(void) {
    if (Local_u8StartFlag == 0) {
        /* Start Timer */
        MSTK_voidSetInterval_single(100000, voidTakeAction);
        Local_u8StartFlag = 1;
    } else {
        Local_u32FrameData[Local_u8EdgeCounter] = (MSTK_u32GetElapsedTime() / 2);
        MSTK_voidSetInterval_single(100000, voidTakeAction);
        Local_u8EdgeCounter++;
    }
    if (Local_u8EdgeCounter == 50) {
        asm("NOP");
    }
}

// System initialization
void System_Init(void) {
    MRCC_vInt();
    MRCC_vEnable(RCC_AHB1, AHB1_GPIOA);
    MRCC_vEnable(RCC_APB2, APB2_SPI1);
    MRCC_vEnable(RCC_AHB1, AHB1_GPIOB);
    MRCC_vEnable(RCC_APB1, APB1_TIM2);
    TIM2_voidInit();
}

// GPIO initialization
void GPIO_Init(void) {
    // RST for TFT
    MGPIO_vSetMode(PORT_A, PIN_8, MODE_OUTPUT);
    MGPIO_vSetMode(PORT_A, PIN_9, MODE_OUTPUT);
    MGPIO_vSetOutputMode(PORT_A, PIN_8, OUTPUT_PUSHPULL, SPEED_LOW);
    MGPIO_vSetOutputMode(PORT_A, PIN_9, OUTPUT_PUSHPULL, SPEED_LOW);

    // Set SPI pins
    MGPIO_vSetMode(PORT_A, PIN_4, MODE_ALTERNATIVE);
    MGPIO_vSetMode(PORT_A, PIN_5, MODE_ALTERNATIVE);
    MGPIO_vSetMode(PORT_A, PIN_6, MODE_ALTERNATIVE);
    MGPIO_vSetMode(PORT_A, PIN_7, MODE_ALTERNATIVE);

    MGPIO_vSetAlternativeMode(PORT_A, PIN_4, ALTR_SPI);
    MGPIO_vSetAlternativeMode(PORT_A, PIN_5, ALTR_SPI);
    MGPIO_vSetAlternativeMode(PORT_A, PIN_6, ALTR_SPI);
    MGPIO_vSetAlternativeMode(PORT_A, PIN_7, ALTR_SPI);
}

// Main function
int main(void) {
    // Initialize system and GPIO
    System_Init();
    GPIO_Init();

    // Initialize SPI, SysTick, TFT, and R2RDAC
    SPI1_voidInit();
    MSYSYTICK_voidInit();
    HTFT_voidInit();
    R2RDAC_Init();
    MSYSYTICK_voidCtrlIntState(SYSTICKEnable);

    // Initialize pins for IR remote
    MGPIO_vSetMode(PORT_A, PIN_1, MODE_OUTPUT);
    MGPIO_vSetMode(PORT_A, PIN_2, MODE_OUTPUT);
    MGPIO_vSetMode(PORT_A, PIN_3, MODE_OUTPUT);
    MGPIO_vSetMode(PORT_A, 0, MODE_INPUT);
    MGPIO_vSetInputMode(PORT_A, 0, INPUT_PULLUP);
    MGPIO_vSetOutputMode(PORT_A, PIN_1, OUTPUT_PUSHPULL, SPEED_LOW);
    MGPIO_vSetOutputMode(PORT_A, PIN_2, OUTPUT_PUSHPULL, SPEED_LOW);
    MGPIO_vSetOutputMode(PORT_A, PIN_3, OUTPUT_PUSHPULL, SPEED_LOW);

    /* EXTI Initialization */
    MEXTI_voidInerruptSetPort(EXTI_LineZero, EXTI_PortA);
    MEXTI0_voidCallBack(voidGetFrame);
    MEXTI_voidInterruptEnableDisable(EXTI_LineZero, EXTI_Enable);
    MEXTI_voidInterruptTrigger(EXTI_LineZero, EXTI_Falling);

    /* Enable EXTI0 from NVIC */
    NVIC_vEnable(6);

    while (1) {
        // Infinite loop
    }
}
