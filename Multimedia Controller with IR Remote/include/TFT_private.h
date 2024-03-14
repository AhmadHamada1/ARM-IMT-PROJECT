/*
 * TFT_private.h
 *
 *  Created on: Feb 5, 2024
 *      Author: philo
 */

#ifndef TFT_PRIVATE_H_
#define TFT_PRIVATE_H_


#define SLPOUT_CMD      0x11
#define COLMOD_CMD      0x3A
#define DISPON_CMD      0x29

static void HTFT_voidSendCommand(u8 Copy_u8Command);
static void HTFT_voidSendData(u8 Copy_u8Data);


#endif /* TFT_PRIVATE_H_ */
