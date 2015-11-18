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
 *  @file usb.c
 *
 *  @brief usb-specific hardware setup, NVIC, clocks, and usb activities
 *  in the pre-attached state. includes some of the lower level callbacks
 *  needed by the usb library, like suspend,resume,init,etc
 */

#include "usb.h"
#include "dfu.h"


extern u8 u8_usbConfigDescriptorDFU[];
extern u8 u8_usbFunctionalDescriptor[];

void setupUSB (void) {

#ifdef HAS_MAPLE_HARDWARE	
  /* Setup USB DISC pin as output open drain */	
#if defined(TARGET_MAPLE_REV3) || defined(TARGET_MAPLE_REV5) || defined(TARGET_MAPLE_MINI)
  SET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC),(GET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC)) & crMask(USB_DISC)) | CR_OUTPUT_OD << CR_SHITF(LED_PIN));  
#else
  SET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC),(GET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC)) & crMask(USB_DISC)) | CR_OUTPUT_OD << CR_SHITF(USB_DISC));  
#endif
  gpio_write_bit(USB_DISC_BANK,USB_DISC,1);

  /* turn on the USB clock */
  //pRCC->APB1ENR |= RCC_APB1ENR_USB_CLK;// done in setupCLK()

  gpio_write_bit(USB_DISC_BANK,USB_DISC,0);  /* present ourselves to the host */
#else

/* Generic boards don't have disconnect hardware, so we drive PA12 which is connected to the usb D+ line*/
#define USB_DISC_BANK         GPIOA
#define USB_DISC              12

  SET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC),
          (GET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC)) & crMask(USB_DISC)) | CR_OUTPUT_PP << CR_SHITF(USB_DISC));

  gpio_write_bit(USB_DISC_BANK,USB_DISC,0);  /* present ourselves to the host */
  
  volatile unsigned int delay;
  for(delay = 0;delay<256;delay++);

  //  volatile unsigned x = 1024; do { ; }while(--x);// wait a moment
  /* turn on the USB clock */
   SET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC),
          (GET_REG(GPIO_CR(USB_DISC_BANK,USB_DISC)) & crMask(USB_DISC)) | CR_INPUT << CR_SHITF(USB_DISC)); //Sets the PA12 as floating input
 //  pRCC->APB1ENR |= RCC_APB1ENR_USB_CLK;
#endif  
  /* initialize the usb application */
  
  wTransferSize=getFlashPageSize();
  u8_usbConfigDescriptorDFU[41]=(wTransferSize & 0x00FF);
  u8_usbConfigDescriptorDFU[42]=(wTransferSize & 0xFF00)>>8;
  
  u8_usbFunctionalDescriptor[5]=(wTransferSize & 0x00FF);
  u8_usbFunctionalDescriptor[6]=(wTransferSize & 0xFF00)>>8;  
  
  usbAppInit();

}




void usbDsbBus(void) {
// setPin(USB_DISC_BANK,USB_DISC);
usbPowerOff();
// SET_REG(USB_DISC_CR,
// (GET_REG(USB_DISC_CR) & USB_DISC_CR_MASK) | USB_DISC_CR_OUTPUT);
// resetPin(USB_DISC_BANK, USB_DISC); /* Pull DP+ down */
// volatile unsigned x = 500000; do { ; }while(--x);
// SET_REG(USB_DISC_CR,
// (GET_REG(USB_DISC_CR) & USB_DISC_CR_MASK) | USB_DISC_CR_INPUT); //Sets the PA12 as floating input
}

vu32 bDeviceState = UNCONNECTED;

/* tracks sequential behavior of the ISTR */
vu16 wIstr;
vu8 bIntPackSOF = 0;

DEVICE Device_Table = {
    NUM_ENDPTS,
    1
};

DEVICE_PROP Device_Property = {
    usbInit,
    usbReset,
    usbStatusIn,
    usbStatusOut,
    usbDataSetup,
    usbNoDataSetup,
    usbGetInterfaceSetting,
    usbGetDeviceDescriptor,
    usbGetConfigDescriptor,
    usbGetStringDescriptor,
    usbGetFunctionalDescriptor,
    0,
    bMaxPacketSize
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
    usbGetConfiguration,
    usbSetConfiguration,
    usbGetInterface,
    usbSetInterface,
    usbGetStatus,
    usbClearFeature,
    usbSetEndpointFeature,
    usbSetDeviceFeature,
    usbSetDeviceAddress
};

void (*pEpInt_IN[7])(void) = {
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
};

void (*pEpInt_OUT[7])(void) = {
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
    nothingProc,
};

struct {
    volatile RESUME_STATE eState;
    volatile u8 bESOFcnt;
} ResumeS;

/* dummy proc */
void nothingProc(void) {
}

/* Function Definitions */
void usbAppInit(void) {
    /* hook in to usb_core, depends on all those damn
       non encapsulated externs! */
    USB_Init();
}

void usbSuspend(void) {
    u16 wCNTR;
    wCNTR = _GetCNTR();
    wCNTR |= CNTR_FSUSP | CNTR_LPMODE;
    _SetCNTR(wCNTR);

    /* run any power reduction handlers */
    bDeviceState = SUSPENDED;
}

void usbResumeInit(void) {
    u16 wCNTR;

    /* restart any clocks that had been stopped */

    wCNTR = _GetCNTR();
    wCNTR &= (~CNTR_LPMODE);
    _SetCNTR(wCNTR);

    /* undo power reduction handlers here */

    _SetCNTR(ISR_MSK);
}

void usbResume(RESUME_STATE eResumeSetVal) {
    u16 wCNTR;

    if (eResumeSetVal != RESUME_ESOF)
        ResumeS.eState = eResumeSetVal;

    switch (ResumeS.eState) {
    case RESUME_EXTERNAL:
        usbResumeInit();
        ResumeS.eState = RESUME_OFF;
        break;
    case RESUME_INTERNAL:
        usbResumeInit();
        ResumeS.eState = RESUME_START;
        break;
    case RESUME_LATER:
        ResumeS.bESOFcnt = 2;
        ResumeS.eState = RESUME_WAIT;
        break;
    case RESUME_WAIT:
        ResumeS.bESOFcnt--;
        if (ResumeS.bESOFcnt == 0)
            ResumeS.eState = RESUME_START;
        break;
    case RESUME_START:
        wCNTR = _GetCNTR();
        wCNTR |= CNTR_RESUME;
        _SetCNTR(wCNTR);
        ResumeS.eState = RESUME_ON;
        ResumeS.bESOFcnt = 10;
        break;
    case RESUME_ON:
        ResumeS.bESOFcnt--;
        if (ResumeS.bESOFcnt == 0) {
            wCNTR = _GetCNTR();
            wCNTR &= (~CNTR_RESUME);
            _SetCNTR(wCNTR);
            ResumeS.eState = RESUME_OFF;
        }
        break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
        ResumeS.eState = RESUME_OFF;
        break;
    }
}

RESULT usbPowerOn(void) {
    u16 wRegVal;

    wRegVal = CNTR_FRES;
    _SetCNTR(wRegVal);

    wInterrupt_Mask = 0;
    _SetCNTR(wInterrupt_Mask);
    _SetISTR(0);
    wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM; /* the bare minimum */
    _SetCNTR(wInterrupt_Mask);

    return USB_SUCCESS;
}

RESULT usbPowerOff(void) {
    _SetCNTR(CNTR_FRES);
    _SetISTR(0);
    _SetCNTR(CNTR_FRES + CNTR_PDWN);

    /* note that all weve done here is powerdown the
       usb peripheral. we have no disabled the clocks,
       pulled the usb_disc pin back up, or reset the
       application state machines */

    return USB_SUCCESS;
}

void usbInit(void) {
    dfuInit();

    pInformation->Current_Configuration = 0;
    usbPowerOn();

    _SetISTR(0);
    wInterrupt_Mask = ISR_MSK;
    _SetCNTR(wInterrupt_Mask);

    usbEnbISR(); /* configure the cortex M3 private peripheral NVIC */
    bDeviceState = UNCONNECTED;
}

void usbReset(void) {
    dfuUpdateByReset();

    pInformation->Current_Configuration = 0;
    pInformation->Current_Feature = usbConfigDescriptorDFU.Descriptor[7];

    _SetBTABLE(BTABLE_ADDRESS);

    /* setup the ctrl endpoint */
    _SetEPType(ENDP0, EP_CONTROL);
    _SetEPTxStatus(ENDP0, EP_TX_STALL);

    _SetEPRxAddr(ENDP0, ENDP0_RXADDR);
    _SetEPTxAddr(ENDP0, ENDP0_TXADDR);

    Clear_Status_Out(ENDP0);

    SetEPRxCount(ENDP0, pProperty->MaxPacketSize);
    //  SetEPTxCount(ENDP0, pProperty->MaxPacketSize);
    SetEPRxValid(ENDP0);

    bDeviceState = ATTACHED;
    SetDeviceAddress(0); /* different than usbSetDeviceAddr! comes from usb_core */
}

void usbStatusIn(void) {
}

void usbStatusOut(void) {
}

RESULT usbDataSetup(u8 request) {
    u8 *(*CopyRoutine)(u16);
    CopyRoutine = NULL;

    /* handle dfu class requests */
    if ((pInformation->USBbmRequestType & (REQUEST_TYPE | RECIPIENT)) == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        if (dfuUpdateByRequest()) {
            /* successfull state transition, handle the request */
            switch (request) {
            case(DFU_GETSTATUS):
                CopyRoutine = dfuCopyStatus;
                break;
            case(DFU_GETSTATE):
                CopyRoutine = dfuCopyState;
                break;
            case(DFU_DNLOAD):
                CopyRoutine = dfuCopyDNLOAD;
                break;
            case(DFU_UPLOAD):
                CopyRoutine = dfuCopyUPLOAD;
                break;
            default:
                /* leave copy routine null */
                break;
            }
        }
    }

    if (CopyRoutine != NULL) {
        pInformation->Ctrl_Info.CopyData = CopyRoutine;
        pInformation->Ctrl_Info.Usb_wOffset = 0;
        (*CopyRoutine)(0);

        return USB_SUCCESS;
    }

    return USB_UNSUPPORT;
}

RESULT usbNoDataSetup(u8 request) {
    if ((pInformation->USBbmRequestType & (REQUEST_TYPE | RECIPIENT)) == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        /* todo, keep track of the destination interface, often stored in wIndex */
        if (dfuUpdateByRequest()) {
            return USB_SUCCESS;
        }
    }
    return USB_UNSUPPORT;
}

RESULT usbGetInterfaceSetting(u8 interface, u8 altSetting) {
    /* alt setting 0 -> program RAM, alt setting 1 -> FLASH */
    if (interface > NUM_ALT_SETTINGS) {
        return USB_UNSUPPORT;
    } else {
        return USB_SUCCESS;
    }
}

u8 *usbGetDeviceDescriptor(u16 len) {
    return Standard_GetDescriptorData(len, &usbDeviceDescriptorDFU);
}

u8 *usbGetConfigDescriptor(u16 len) {
    return Standard_GetDescriptorData(len, &usbConfigDescriptorDFU);
}

u8 *usbGetStringDescriptor(u16 len) {
    u8 strIndex = pInformation->USBwValue0;
    if (strIndex > STR_DESC_LEN) {
        return NULL;
    } else {
        return Standard_GetDescriptorData(len, &usbStringDescriptor[strIndex]);
    }
}

u8 *usbGetFunctionalDescriptor(u16 len) {
    return Standard_GetDescriptorData(len, &usbFunctionalDescriptor);
}



/***** start of USER STANDARD REQUESTS ******
 *
 * These are the USER STANDARD REQUESTS, they are handled
 * in the core but we are given these callbacks at the
 * application level
 *******************************************/

void usbGetConfiguration(void) {
    /* nothing process */
}

void usbSetConfiguration(void) {
    if (pInformation->Current_Configuration != 0) {
        bDeviceState = CONFIGURED;
    }
}

void usbGetInterface(void) {
    /* nothing process */
}

void usbSetInterface(void) {
    /* nothing process */
}

void usbGetStatus(void) {
    /* nothing process */
}

void usbClearFeature(void) {
    /* nothing process */
}

void usbSetEndpointFeature(void) {
    /* nothing process */
}

void usbSetDeviceFeature(void) {
    /* nothing process */
}

void usbSetDeviceAddress(void) {
    bDeviceState = ADDRESSED;
}
/***** end of USER STANDARD REQUESTS *****/


void usbEnbISR(void) {
    NVIC_InitTypeDef NVIC_InitStructure;


    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = TRUE;
    nvicInit(&NVIC_InitStructure);
}

void usbDsbISR(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = FALSE;
    nvicInit(&NVIC_InitStructure);
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
    wIstr = _GetISTR();

    /* go nuts with the preproc switches since this is an ISTR and must be FAST */
#if (ISR_MSK & ISTR_RESET)
    if (wIstr & ISTR_RESET & wInterrupt_Mask) {
        _SetISTR((u16)CLR_RESET);
        Device_Property.Reset();
    }
#endif


#if (ISR_MSK & ISTR_DOVR)
    if (wIstr & ISTR_DOVR & wInterrupt_Mask) {
        _SetISTR((u16)CLR_DOVR);
    }
#endif


#if (ISR_MSK & ISTR_ERR)
    if (wIstr & ISTR_ERR & wInterrupt_Mask) {
        _SetISTR((u16)CLR_ERR);
    }
#endif


#if (ISR_MSK & ISTR_WKUP)
    if (wIstr & ISTR_WKUP & wInterrupt_Mask) {
        _SetISTR((u16)CLR_WKUP);
        usbResume(RESUME_EXTERNAL);
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (ISR_MSK & ISTR_SUSP)
    if (wIstr & ISTR_SUSP & wInterrupt_Mask) {

        /* check if SUSPEND is possible */
        if (F_SUSPEND_ENABLED) {
            usbSuspend();
        } else {
            /* if not possible then resume after xx ms */
            usbResume(RESUME_LATER);
        }
        /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
        _SetISTR((u16)CLR_SUSP);
    }
#endif


#if (ISR_MSK & ISTR_SOF)
    if (wIstr & ISTR_SOF & wInterrupt_Mask) {
        _SetISTR((u16)CLR_SOF);
        bIntPackSOF++;
    }
#endif


#if (ISR_MSK & ISTR_ESOF)
    if (wIstr & ISTR_ESOF & wInterrupt_Mask) {
        _SetISTR((u16)CLR_ESOF);
        /* resume handling timing is made with ESOFs */
        usbResume(RESUME_ESOF); /* request without change of the machine state */
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (ISR_MSK & ISTR_CTR)
    if (wIstr & ISTR_CTR & wInterrupt_Mask) {
        /* servicing of the endpoint correct transfer interrupt */
        /* clear of the CTR flag into the sub */
        CTR_LP(); /* low priority ISR defined in the usb core lib */
    }
#endif

}
