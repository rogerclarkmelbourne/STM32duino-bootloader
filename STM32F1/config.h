/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 *  @file config.h
 *
 *  @brief bootloader settings and macro defines
 *
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "common.h"

/* Speed controls for strobing the LED pin */
#define BLINK_FAST 0x50000
#define BLINK_SLOW 0x100000


// The USB clock is the same for all boards
#define RCC_APB1ENR_USB_CLK   0x00800000 



/* On the Mini, LED is PB1 */
#if defined TARGET_MAPLE_MINI

/* Porting information Please read.

	These defineds are use to setup the hardware of the GPIO. 
	See http://www.st.com/web/en/resource/technical/document/reference_manual/CD00171190.pdf
	

	Two GPIO pins need to be defined, the LED and the Button.
	
	
	For each pin, the following is required
	
	LED_BANK, this is the GPIO port, e.g. GPIOA,GPIOB, GPIOC etc etc etc
	LED, this is the pin number e.g PB1 = 1
	LED_BANK_CR, this is the address of the port control register 
	
	for pins  0 to 7 its GPIO_CRL(LED_BANK)
	for pins  8 to 15 its GPIO_CRH(LED_BANK)

	This is because 2 different control registers are needed for the lower and upper 8 bits in the GPIO port (see the programming manual)
	
	LED_CR_MASK, this is the 4 bit mask used to mask in the CR mode , i.e there are 4 bits per pin, hence pin 1 = 0xFFFFFF0F  pin 0 would be 0xFFFFFFF0 etc
	
	RCC_APB2ENR_LED, this is the clock control register for the port, without this setting to the RCC m the whole GPIO port wont work at all
	See 7.3.7 APB2 peripheral clock enable register (RCC_APB2ENR) in the manual. page 112 and 113
	Bit 8 IOPGEN: IO port G clock enable

	Bit 7 IOPFEN: IO port F clock enable
	Bit 6 IOPEEN: IO port E clock enable
	Bit 5 IOPDEN: IO port D clock enable
	Bit 4 IOPCEN: IO port C clock enable
	Bit 3 IOPBEN: IO port B clock enable
	Bit 2 IOPAEN: IO port A clock enable
	
*/

#warning "Target MAPLE_MINI"

	#define HAS_MAPLE_HARDWARE 1

	#define FLASH_PAGE_SIZE 0x400

	#define LED_BANK         GPIOB
	#define LED              1
	#define LED_BANK_CR      GPIO_CRL(LED_BANK)
	#define LED_CR_MASK      0xFFFFFF0F
	#define LED_CR_MODE      0x00000010
	#define RCC_APB2ENR_LED  0x00000008 /* enable Port B (bit 3 - see table above IOPBEN)*/

	/* On the Mini, BUT is PB8 */
	#define BUTTON_BANK      GPIOB
	#define BUTTON           8
	#define BUT_BANK_CR      GPIO_CRH(BUTTON_BANK)
	#define BUT_CR_MASK      0xFFFFFFF0
	#define BUT_CR_INPUT_PU_PD  0x00000008
	#define RCC_APB2ENR_BUT  0x00000008 /* enable Port B (bit 3 - see table above IOPBEN)*/
	
	/* USB Disc Pin Setup.   USB DISC is PB9 */
	#define USB_DISC_BANK         GPIOB
	#define USB_DISC              9
	#define USB_DISC_CR           GPIO_CRH(USB_DISC_BANK)
	#define USB_DISC_CR_MASK      0xFFFFFF0F
	#define USB_DISC_CR_OUTPUT_OD 0x00000050

	#define RCC_APB2ENR_USB       0x00000008



	
	
// Use the usb_description_strings_util.html to make new strngs for the next 3 arrays if you need to change the text.

	#define ALT0_STR_LEN 0x8E
	#define ALT0_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'E',0,'R',0,'R',0,'O',0,'R',0,'.',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'R',0,'A',0,'M',0,' ',0,'i',0,'s',0,' ',0,'n',0,'o',0,'t',0,' ',0,'s',0,'u',0,'p',0,'p',0,'o',0,'r',0,'t',0,'e',0,'d',0,'.',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'m',0,'i',0,'n',0,'i',0,')',0

	#define ALT1_STR_LEN 0x84
	#define ALT1_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'5',0,'0',0,'0',0,'0',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'m',0,'i',0,'n',0,'i',0,')',0

	#define ALT2_STR_LEN 0x84
	#define ALT2_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'2',0,'0',0,'0',0,'0',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'m',0,'i',0,'n',0,'i',0,')',0

	#define USER_CODE_RAM     			((u32)0x20000C00)
	#define RAM_END           			((u32)0x20005000)
	#define USER_CODE_FLASH0X8005000   	((u32)0x08005000)
	#define USER_CODE_FLASH0X8002000	((u32)0x08002000)
	#define FLASH_END         			((u32)0x08020000)
	
	
	
	
#elif defined TARGET_MAPLE_REV3

#warning "Target MAPLE_REV3"

	#define HAS_MAPLE_HARDWARE 1
	#define FLASH_PAGE_SIZE 0x800
	
	#define LED_BANK         GPIOB
	#define LED              1
	#define LED_BANK_CR      GPIO_CRL(LED_BANK)
	#define LED_CR_MASK      0xFFFFFF0F
	#define LED_CR_MODE      0x00000010
	#define RCC_APB2ENR_LED  0x00000008 /* enable PB */

	/* On the Mini, BUT is PB8 */
	#define BUTTON_BANK      GPIOB
	#define BUTTON           8
	#define BUT_BANK_CR      GPIO_CRH(BUTTON_BANK)
	#define BUT_CR_MASK      0xFFFFFFF0
	#define BUT_CR_INPUT_PU_PD 0x00000008
	#define RCC_APB2ENR_BUT  0x00000008 /* enable PB */

	/* USB Disc Pin Setup.   USB DISC is PB9 */
	#define USB_DISC_BANK         GPIOB
	#define USB_DISC              9
	#define USB_DISC_CR           GPIO_CRH(USB_DISC_BANK)
	#define USB_DISC_CR_MASK      0xFFFFFF0F
	#define BUT_CR_INPUT_PD 0x00000050
	#define RCC_APB2ENR_USB       0x00000008

	
// Use the usb_description_strings_util.html to make new strngs for the next 3 arrays if you need to change the text.

	#define ALT0_STR_LEN 0x8E
	#define ALT0_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'E',0,'R',0,'R',0,'O',0,'R',0,'.',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'R',0,'A',0,'M',0,' ',0,'i',0,'s',0,' ',0,'n',0,'o',0,'t',0,' ',0,'s',0,'u',0,'p',0,'p',0,'o',0,'r',0,'t',0,'e',0,'d',0,'.',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'R',0,'e',0,'v',0,'3',0,')',0

	#define ALT1_STR_LEN 0x84
	#define ALT1_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'5',0,'0',0,'0',0,'0',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'R',0,'e',0,'v',0,'3',0,')',0

	#define ALT2_STR_LEN 0x84
	#define ALT2_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'2',0,'0',0,'0',0,'0',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'R',0,'e',0,'v',0,'3',0,')',0

	#define USER_CODE_RAM     			((u32)0x20000C00)
	#define RAM_END           			((u32)0x20005000)
	#define USER_CODE_FLASH0X8005000   	((u32)0x08005000)
	#define USER_CODE_FLASH0X8002000	((u32)0x08002000)
	#define FLASH_END         			((u32)0x08020000)	
	
	#define FLASH_PAGE_SIZE 0x400
	
#elif defined TARGET_MAPLE_REV5
#warning "Target MAPLE_REV5"

	#define HAS_MAPLE_HARDWARE 1
	#define FLASH_PAGE_SIZE 0x800

	#define LED_BANK         GPIOA
	#define LED              5
	#define LED_BANK_CR      GPIO_CRL(LED_BANK)
	#define LED_CR_MASK      0xFF0FFFFF
	#define LED_CR_MODE      0x00000010
	#define RCC_APB2ENR_LED  0x00000004 /* enable Part A (Bit 2) */

	/* On the Mini, BUT is PB8 */
	#define BUTTON_BANK      GPIOC
	#define BUTTON           9
	#define BUT_BANK_CR      GPIO_CRH(BUTTON_BANK)
	#define BUT_CR_MASK      0xFFFFFF0F
	#define BUT_CR_INPUT_PU_PD 0x00000008
	#define RCC_APB2ENR_BUT  0x00000010 /* enable Port C (Bit 4)*/

	/* USB Disc Pin Setup.   USB DISC is PB9 */
	#define USB_DISC_BANK         GPIOB
	#define USB_DISC              9
	#define USB_DISC_CR           GPIO_CRH(USB_DISC_BANK)
	#define USB_DISC_CR_MASK      0xFFFFFF0F
	#define USB_DISC_CR_OUTPUT_OD 0x00000050
	#define RCC_APB2ENR_USB       0x00000080 
	

	
// Use the usb_description_strings_util.html to make new strngs for the next 3 arrays if you need to change the text.
	#define ALT0_STR_LEN 0x8E
	#define ALT0_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'E',0,'R',0,'R',0,'O',0,'R',0,'.',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'R',0,'A',0,'M',0,' ',0,'i',0,'s',0,' ',0,'n',0,'o',0,'t',0,' ',0,'s',0,'u',0,'p',0,'p',0,'o',0,'r',0,'t',0,'e',0,'d',0,'.',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'R',0,'e',0,'v',0,'5',0,')',0

	#define ALT1_STR_LEN 0x84
	#define ALT1_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'5',0,'0',0,'0',0,'0',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'R',0,'e',0,'v',0,'5',0,')',0

	#define ALT2_STR_LEN 0x84
	#define ALT2_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'2',0,'0',0,'0',0,'0',0,' ',0,'(',0,'M',0,'a',0,'p',0,'l',0,'e',0,'-',0,'R',0,'e',0,'v',0,'5',0,')',0

	#define USER_CODE_RAM     			((u32)0x20000C00)
	#define RAM_END           			((u32)0x20005000)
	#define USER_CODE_FLASH0X8005000   	((u32)0x08005000)
	#define USER_CODE_FLASH0X8002000	((u32)0x08002000)
	#define FLASH_END         			((u32)0x08040000)		
	
#elif defined TARGET_GENERIC_MEDIUM_DENSITY


/* Most generic STM32F103C8 boards have the LED on PC13 */
	#warning "Target GENERIC_MEDIUM_DENSITY"
	
	#define FLASH_PAGE_SIZE 0x400	
	/*
	#define LED_BANK         GPIOC
	#define LED              12
	// Note GPIO_CRH is high register for bits 8 to 15. (GPIO_CRL would be for bits 0 to 7)
	#define LED_BANK_CR      GPIO_CRH(LED_BANK)
	// Bit mask for pin 13. Thus is the high 32 bits of the control register with 4 bits per pin 
	#define LED_CR_MASK      0xFF0FFFFF
	#define LED_CR_MODE      0x00100000
	#define RCC_APB2ENR_LED  0x00100000 // enable Port C  . Bit 4 IOPAEN: IO port C clock enable
*/
	
	#define LED_BANK         GPIOB
	#define LED              1
	#define LED_BANK_CR      GPIO_CRL(LED_BANK)
	#define LED_CR_MASK      0xFFFFFF0F
	#define LED_CR_MODE      0x00000010
	#define RCC_APB2ENR_LED  0x00000008 /* enable Port B (bit 3 - see table above IOPBEN)*/
	
	#define BUTTON_BANK GPIOC
	#define BUTTON 9
	#define BUT_BANK_CR GPIO_CRH(BUTTON_BANK)
	#define BUT_CR_MASK 0xFFFFFF0F
	#define BUT_CR_INPUT_PU_PD 0x00000080 // Input PU/PD
	#define RCC_APB2ENR_BUT 0x00000010 // enable PC		
	
	/* Generic boards don't have disconnect hardware, so we drive PA12 which is connected to the usb D- line*/
	#define USB_DISC_BANK GPIOA
	#define USB_DISC 12
	#define USB_DISC_CR GPIO_CRH(USB_DISC_BANK)
	#define USB_DISC_CR_MASK 0xFFF0FFFF
	#define USB_DISC_CR_OUTPUT_PP 	0x00010000
	#define USB_DISC_CR_INPUT 		0x00040000
	#define RCC_APB2ENR_USB 		0x00000004 // enable PA */

	
	

// Use the usb_description_strings_util.html to make new strngs for the next 3 arrays if you need to change the text.
	#define ALT0_STR_LEN 0x90
	#define ALT0_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'E',0,'R',0,'R',0,'O',0,'R',0,'.',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'R',0,'A',0,'M',0,' ',0,'i',0,'s',0,' ',0,'n',0,'o',0,'t',0,' ',0,'s',0,'u',0,'p',0,'p',0,'o',0,'r',0,'t',0,'e',0,'d',0,'.',0,' ',0,'(',0,'S',0,'T',0,'M',0,'3',0,'2',0,'F',0,'1',0,'0',0,'3',0,'C',0,'8',0,')',0

	#define ALT1_STR_LEN 0x86
	#define ALT1_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'5',0,'0',0,'0',0,'0',0,' ',0,'(',0,'S',0,'T',0,'M',0,'3',0,'2',0,'F',0,'1',0,'0',0,'3',0,'C',0,'8',0,')',0

	#define ALT2_STR_LEN 0x86
	#define ALT2_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'2',0,'0',0,'0',0,'0',0,' ',0,'(',0,'S',0,'T',0,'M',0,'3',0,'2',0,'F',0,'1',0,'0',0,'3',0,'C',0,'8',0,')',0


	#define USER_CODE_RAM     			((u32)0x20000C00)
	#define RAM_END           			((u32)0x20005000)
	#define USER_CODE_FLASH0X8005000   	((u32)0x08005000)
	#define USER_CODE_FLASH0X8002000	((u32)0x08002000)
	
	// 64k flash
	#define FLASH_END         			((u32)0x08010000)		

#elif defined TARGET_GENERIC_HIGH_DENSITY

/* Most generic STM32F103C8 boards have the LED on PC13 */
	//#warning "Data for STM32F103C8"
	
	#define FLASH_PAGE_SIZE 0x800	
	/*
	#define LED_BANK         GPIOC
	#define LED              12
	// Note GPIO_CRH is high register for bits 8 to 15. (GPIO_CRL would be for bits 0 to 7)
	#define LED_BANK_CR      GPIO_CRH(LED_BANK)
	// Bit mask for pin 13. Thus is the high 32 bits of the control register with 4 bits per pin 
	#define LED_CR_MASK      0xFF0FFFFF
	#define LED_CR_MODE      0x00100000
	#define RCC_APB2ENR_LED  0x00100000 // enable Port C  . Bit 4 IOPAEN: IO port C clock enable
*/

	#define LED_BANK         GPIOB
	#define LED              1
	#define LED_BANK_CR      GPIO_CRL(LED_BANK)
	#define LED_CR_MASK      0xFFFFFF0F
	#define LED_CR_MODE      0x00000010
	#define RCC_APB2ENR_LED  0x00000008 /* enable Port B (bit 3 - see table above IOPBEN)*/
	
	#define BUTTON_BANK GPIOC
	#define BUTTON 9
	#define BUT_BANK_CR GPIO_CRH(BUTTON_BANK)
	#define BUT_CR_MASK 0xFFFFFF0F
	#define BUT_CR_INPUT_PU_PD 0x00000080 // Input PU/PD
	#define RCC_APB2ENR_BUT 0x00000010 // enable PC	
	
	/* Generic boards don't have disconnect hardware, so we drive PA12 which is connected to the usb D- line*/
	#define USB_DISC_BANK GPIOA
	#define USB_DISC 12
	#define USB_DISC_CR GPIO_CRH(USB_DISC_BANK)
	#define USB_DISC_CR_MASK 		0xFFF0FFFF
	#define USB_DISC_CR_OUTPUT_PP 	0x00010000
	#define USB_DISC_CR_INPUT 		0x00040000
	#define RCC_APB2ENR_USB 		0x00000004 // enable PA */

	
	

// Use the usb_description_strings_util.html to make new strngs for the next 3 arrays if you need to change the text.
	#define ALT0_STR_LEN 0x90
	#define ALT0_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'E',0,'R',0,'R',0,'O',0,'R',0,'.',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'R',0,'A',0,'M',0,' ',0,'i',0,'s',0,' ',0,'n',0,'o',0,'t',0,' ',0,'s',0,'u',0,'p',0,'p',0,'o',0,'r',0,'t',0,'e',0,'d',0,'.',0,' ',0,'(',0,'S',0,'T',0,'M',0,'3',0,'2',0,'F',0,'1',0,'0',0,'3',0,'C',0,'8',0,')',0

	#define ALT1_STR_LEN 0x86
	#define ALT1_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'5',0,'0',0,'0',0,'0',0,' ',0,'(',0,'S',0,'T',0,'M',0,'3',0,'2',0,'F',0,'1',0,'0',0,'3',0,'C',0,'8',0,')',0

	#define ALT2_STR_LEN 0x86
	#define ALT2_MSG_STR 'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,'.',0,'r',0,'c',0,'1',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'2',0,'0',0,'0',0,'0',0,' ',0,'(',0,'S',0,'T',0,'M',0,'3',0,'2',0,'F',0,'1',0,'0',0,'3',0,'C',0,'8',0,')',0


	#define USER_CODE_RAM     			((u32)0x20000C00)
	#define RAM_END           			((u32)0x20005000)
	#define USER_CODE_FLASH0X8005000   	((u32)0x08005000)
	#define USER_CODE_FLASH0X8002000	((u32)0x08002000)
	
	// 64k flash
	#define FLASH_END         			((u32)0x08010000)		
	
	
#else
#error "No config for this target"

#endif

#define STARTUP_BLINKS 5
#define BOOTLOADER_WAIT 6



#define VEND_ID0 0xAF
#define VEND_ID1 0x1E
#define PROD_ID0 0x03
#define PROD_ID1 0x00

#endif
