/* *****************************************************************************
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
 * ****************************************************************************/

/**
 *  @file hardware.c
 *
 *  @brief init routines to setup clocks, interrupts, also destructor functions.
 *  does not include USB stuff. EEPROM read/write functions.
 *
 */
#include "common.h"
#include "hardware.h"
/*
void setPin(u32 bank, u8 pin) {
    u32 pinMask = 0x1 << (pin);
    SET_REG(GPIO_BSRR(bank), pinMask);
}

void resetPin(u32 bank, u8 pin) {
    u32 pinMask = 0x1 << (16 + pin);
    SET_REG(GPIO_BSRR(bank), pinMask);
}
*/
void gpio_write_bit(u32 bank, u8 pin, u8 val) {
    val = !val;          // "set" bits are lower than "reset" bits
    SET_REG(GPIO_BSRR(bank), (1U << pin) << (16 * val));
}

bool readPin(u32 bank, u8 pin) {
    // todo, implement read
    if (GET_REG(GPIO_IDR(bank)) & (0x01 << pin)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

bool readButtonState() {
    // todo, implement read
    bool state=FALSE;
#if defined(BUTTON_BANK) && defined (BUTTON_PIN) && defined (BUTTON_PRESSED_STATE)
    if (GET_REG(GPIO_IDR(BUTTON_BANK)) & (0x01 << BUTTON_PIN))
    {
        state = TRUE;
    }

    if (BUTTON_PRESSED_STATE==0)
    {
        state=!state;
    }
#endif
    return state;
}

void strobePin(u32 bank, u8 pin, u8 count, u32 rate,u8 onState)
{
    gpio_write_bit( bank,pin,1-onState);

    u32 c;
    while (count-- > 0)
    {
        for (c = rate; c > 0; c--)
        {
            asm volatile("nop");
        }

        gpio_write_bit( bank,pin,onState);

        for (c = rate; c > 0; c--)
        {
            asm volatile("nop");
        }
        gpio_write_bit( bank,pin,1-onState);
    }
}

void systemReset(void) {
    SET_REG(RCC_CR, GET_REG(RCC_CR)     | 0x00000001);
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) & 0xF8FF0000);
    SET_REG(RCC_CR, GET_REG(RCC_CR)     & 0xFEF6FFFF);
    SET_REG(RCC_CR, GET_REG(RCC_CR)     & 0xFFFBFFFF);
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) & 0xFF80FFFF);

    SET_REG(RCC_CIR, 0x00000000);  /* disable all RCC interrupts */
}

void setupCLK(void) {
    unsigned int StartUpCounter=0;
    /* enable HSE */
    SET_REG(RCC_CR, GET_REG(RCC_CR) | 0x00010001);
    while ((GET_REG(RCC_CR) & 0x00020000) == 0); /* for it to come on */

    /* enable flash prefetch buffer */
    SET_REG(FLASH_ACR, 0x00000012);

    /* Configure PLL */
#ifdef XTAL12M
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) | 0x00110400); /* pll=72Mhz(x6),APB1=36Mhz,AHB=72Mhz */
#else
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) | 0x001D0400); /* pll=72Mhz(x9),APB1=36Mhz,AHB=72Mhz */
#endif

    SET_REG(RCC_CR, GET_REG(RCC_CR)     | 0x01000000); /* enable the pll */


#if !defined  (HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT    ((unsigned int)0x0500)   /*!< Time out for HSE start up */
#endif /* HSE_STARTUP_TIMEOUT */

    while ((GET_REG(RCC_CR) & 0x03000000) == 0 && StartUpCounter < HSE_STARTUP_TIMEOUT)
    {
//      StartUpCounter++; // This is commented out, so other changes can be committed. It will be uncommented at a later date
    }   /* wait for it to come on */

    if (StartUpCounter>=HSE_STARTUP_TIMEOUT)
    {
        // HSE has not started. Try restarting the processor
        systemHardReset();
    }

    /* Set SYSCLK as PLL */
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) | 0x00000002);
    while ((GET_REG(RCC_CFGR) & 0x00000008) == 0); /* wait for it to come on */

    pRCC->APB2ENR |= 0B111111100;// Enable All GPIO channels (A to G)
    pRCC->APB1ENR |= RCC_APB1ENR_USB_CLK;
}


void setupLEDAndButton (void) {
    // SET_REG(AFIO_MAPR,(GET_REG(AFIO_MAPR) & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_NO_JTAG_NO_SW);// Try to disable SWD AND JTAG so we can use those pins (not sure if this works).

#if defined(BUTTON_BANK) && defined (BUTTON_PIN) && defined (BUTTON_PRESSED_STATE)
    SET_REG(GPIO_CR(BUTTON_BANK,BUTTON_PIN),(GPIO_CR(BUTTON_BANK,BUTTON_PIN) & crMask(BUTTON_PIN)) | BUTTON_INPUT_MODE << CR_SHITF(BUTTON_PIN));

    gpio_write_bit(BUTTON_BANK, BUTTON_PIN,1-BUTTON_PRESSED_STATE);// set pulldown resistor in case there is no button.
#endif
    SET_REG(GPIO_CR(LED_BANK,LED_PIN),(GET_REG(GPIO_CR(LED_BANK,LED_PIN)) & crMask(LED_PIN)) | CR_OUTPUT_PP << CR_SHITF(LED_PIN));
}

void setupFLASH() {
    /* configure the HSI oscillator */
    if ((pRCC->CR & 0x01) == 0x00) {
        u32 rwmVal = pRCC->CR;
        rwmVal |= 0x01;
        pRCC->CR = rwmVal;
    }

    /* wait for it to come on */
    while ((pRCC->CR & 0x02) == 0x00) {}
}

bool checkUserCode(u32 usrAddr) {
    u32 sp = *(vu32 *) usrAddr;

    if ((sp & 0x2FFE0000) == 0x20000000) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}

void setMspAndJump(u32 usrAddr) {
    // Dedicated function with no call to any function (appart the last call)
    // This way, there is no manipulation of the stack here, ensuring that GGC
    // didn't insert any pop from the SP after having set the MSP.
    typedef void (*funcPtr)(void);
    u32 jumpAddr = *(vu32 *)(usrAddr + 0x04); /* reset ptr in vector table */

    funcPtr usrMain = (funcPtr) jumpAddr;

    SET_REG(SCB_VTOR, (vu32) (usrAddr));

    asm volatile("msr msp, %0"::"g"(*(volatile u32 *)usrAddr));

    usrMain();                                /* go! */
}


void jumpToUser(u32 usrAddr) {

    /* tear down all the dfu related setup */
    // disable usb interrupts, clear them, turn off usb, set the disc pin
    // todo pick exactly what we want to do here, now its just a conservative
    flashLock();
    usbDsbISR();
    nvicDisableInterrupts();

#ifndef HAS_MAPLE_HARDWARE
    usbDsbBus();
#endif

// Does nothing, as PC12 is not connected on teh Maple mini according to the schemmatic     setPin(GPIOC, 12); // disconnect usb from host. todo, macroize pin
    systemReset(); // resets clocks and periphs, not core regs

    setMspAndJump(usrAddr);
}

void bkp10Write(u16 value)
{
        // Enable clocks for the backup domain registers
        pRCC->APB1ENR |= (RCC_APB1ENR_PWR_CLK | RCC_APB1ENR_BKP_CLK);

        // Disable backup register write protection
        pPWR->CR |= PWR_CR_DBP;

        // store value in pBK DR10
        pBKP->DR10 = value;

        // Re-enable backup register write protection
        pPWR->CR &=~ PWR_CR_DBP;
}

int checkAndClearBootloaderFlag()
{
    bool flagSet = 0x00;// Flag not used

    // Enable clocks for the backup domain registers
    pRCC->APB1ENR |= (RCC_APB1ENR_PWR_CLK | RCC_APB1ENR_BKP_CLK);

    switch (pBKP->DR10)
    {
        case RTC_BOOTLOADER_FLAG:
            flagSet = 0x01;
            break;
        case RTC_BOOTLOADER_JUST_UPLOADED:
            flagSet = 0x02;
            break;
    }

    if (flagSet!=0x00)
    {
        bkp10Write(0x0000);// Clear the flag
        // Disable clocks
        pRCC->APB1ENR &= ~(RCC_APB1ENR_PWR_CLK | RCC_APB1ENR_BKP_CLK);
    }



    return flagSet;
}



void nvicInit(NVIC_InitTypeDef *NVIC_InitStruct) {
    u32 tmppriority = 0x00;
    u32 tmpreg      = 0x00;
    u32 tmpmask     = 0x00;
    u32 tmppre      = 0;
    u32 tmpsub      = 0x0F;

    SCB_TypeDef *rSCB = (SCB_TypeDef *) SCB_BASE;
    NVIC_TypeDef *rNVIC = (NVIC_TypeDef *) NVIC_BASE;


    /* Compute the Corresponding IRQ Priority --------------------------------*/
    tmppriority = (0x700 - (rSCB->AIRCR & (u32)0x700)) >> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (u32)NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub;

    tmppriority = tmppriority << 0x04;
    tmppriority = ((u32)tmppriority) << ((NVIC_InitStruct->NVIC_IRQChannel & (u8)0x03) * 0x08);

    tmpreg = rNVIC->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)];
    tmpmask = (u32)0xFF << ((NVIC_InitStruct->NVIC_IRQChannel & (u8)0x03) * 0x08);
    tmpreg &= ~tmpmask;
    tmppriority &= tmpmask;
    tmpreg |= tmppriority;

    rNVIC->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)] = tmpreg;

    /* Enable the Selected IRQ Channels --------------------------------------*/
    rNVIC->ISER[(NVIC_InitStruct->NVIC_IRQChannel >> 0x05)] =
        (u32)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (u8)0x1F);
}

void nvicDisableInterrupts() {
    NVIC_TypeDef *rNVIC = (NVIC_TypeDef *) NVIC_BASE;
    rNVIC->ICER[0] = 0xFFFFFFFF;
    rNVIC->ICER[1] = 0xFFFFFFFF;
    rNVIC->ICPR[0] = 0xFFFFFFFF;
    rNVIC->ICPR[1] = 0xFFFFFFFF;

    SET_REG(STK_CTRL, 0x04); /* disable the systick, which operates separately from nvic */
}

void systemHardReset(void) {
    SCB_TypeDef *rSCB = (SCB_TypeDef *) SCB_BASE;

    /* Reset  */
    rSCB->AIRCR = (u32)AIRCR_RESET_REQ;

    /*  should never get here */
    while (1) {
        asm volatile("nop");
    }
}

bool flashErasePage(u32 pageAddr) {
    u32 rwmVal = GET_REG(FLASH_CR);
    rwmVal = FLASH_CR_PER;
    SET_REG(FLASH_CR, rwmVal);

    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    SET_REG(FLASH_AR, pageAddr);
    SET_REG(FLASH_CR, FLASH_CR_START | FLASH_CR_PER);
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}

    /* todo: verify the page was erased */

    rwmVal = 0x00;
    SET_REG(FLASH_CR, rwmVal);

    return TRUE;
}
bool flashErasePages(u32 pageAddr, u16 n) {
    while (n-- > 0) {
        if (!flashErasePage(pageAddr + wTransferSize * n)) {
            return FALSE;
        }
    }

    return TRUE;
}

bool flashWriteWord(u32 addr, u32 word) {
    vu16 *flashAddr = (vu16 *)addr;
    vu32 lhWord = (vu32)word & 0x0000FFFF;
    vu32 hhWord = ((vu32)word & 0xFFFF0000) >> 16;

    u32 rwmVal = GET_REG(FLASH_CR);
    SET_REG(FLASH_CR, FLASH_CR_PG);

    /* apparently we need not write to FLASH_AR and can
       simply do a native write of a half word */
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    *(flashAddr + 0x01) = (vu16)hhWord;
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    *(flashAddr) = (vu16)lhWord;
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}

    rwmVal &= 0xFFFFFFFE;
    SET_REG(FLASH_CR, rwmVal);

    /* verify the write */
    if (*(vu32 *)addr != word) {
        return FALSE;
    }

    return TRUE;
}

void flashLock() {
    /* take down the HSI oscillator? it may be in use elsewhere */

    /* ensure all FPEC functions disabled and lock the FPEC */
    SET_REG(FLASH_CR, 0x00000080);
}

void flashUnlock() {
    /* unlock the flash */
    SET_REG(FLASH_KEYR, FLASH_KEY1);
    SET_REG(FLASH_KEYR, FLASH_KEY2);
}


// Used to create the control register masking pattern, when setting control register mode.
unsigned int crMask(int pin)
{
    unsigned int mask;
    if (pin>=8)
    {
        pin-=8;
    }
    mask = 0x0F << (pin<<2);
    return ~mask;
}

#define FLASH_SIZE_REG 0x1FFFF7E0
int getFlashEnd(void)
{
    unsigned short *flashSize = (unsigned short *) (FLASH_SIZE_REG);// Address register
    return ((int)(*flashSize & 0xffff) * 1024) + 0x08000000;
}

int getFlashPageSize(void)
{

    unsigned short *flashSize = (unsigned short *) (FLASH_SIZE_REG);// Address register
    if ((*flashSize & 0xffff) > 128)
    {
        return 0x800;
    }
    else
    {
        return 0x400;
    }
}
