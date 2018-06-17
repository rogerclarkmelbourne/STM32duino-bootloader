# STM32duino-bootloader

Please Note: This code does not work with all STM32F103 Boards

Also Note: Use GCC 4.8 (not 4.9 or newer, as those versions have more aggressive optimisation which cause hardware registers to be read incorrectly and consequently the bootloader does not work)


Bootloader for STM32F103 boards, for use with the Arduino_STM32 repo and the Arduino IDE

This repo is a derivation of  https://github.com/jonatanolofsson/maple-bootloader (mini-boot branch) which is in turn a derivation of the maple-bootloader created by Leaflabs http://github.com/leaflabs/maple-bootloader

The bootloader has been reworked so that all versions are contained in the Master branch, rather than 1 branch per version / board.

Note.
The upload to RAM option has effectively been removed, as it was useless and caused problems.
The bootloader still has a DFU - AltID for RAM uploads, however it returns an error if the host attempts to uplaod to the AltID. The AltID was kept to allow backward compatibility with anyone still using the old Maple IDE which has a upload to RAM option.

The makefile now has multple build targets including "maple-mini" (roughly equivalent to the old mini-boot branch), maple-rev3 etc etc.

Additionally, the bootloader now works with "generic" STM32F103 boards that do not have the additional USB reset hardware which all Maple and Maple mini boards have.

On "generic" boards, the USB reset (to force re-enumeration by the host), is triggered by reconfiguring the USB D+ line (PA12) into GPIO mode, and driving PA12 low for a short period, before setting the pin back to its USB operational mode.
This USB reset method was written by @Victor_pv.
Note: It is not guaranteed to work on all "generic" STM32 boards, and relies on PA12 having a pull-up resistor of around 1.5k - however most "generic" boards seem to have this.
Its unclear if this method to reset the USB bus conforms precisely to the USB standard, but it seems to work fine on all PCs, Macs and Linux boxes on which it's been tested - and seems usable for hobby / non commericial / non-critical systems.


There are multiple build targets for "generic" STM32F103 boards - because each vendor seems to have the "LED" on a different port/pin - and the bootloader flashes the LED to indicate the current operation / state.

So if your board has an LED on pin PC13 the makefile target is "generic-pc13." At the time of writing, the following generic targets are available (more may be added without this readme being updated each time, so please check the makefile to see the latest list of build targets)

generic-pc13
generic-pg15
generic-pd2
generic-pd1
generic-pa1
generic-pb9

In addition to the LED, differences in the build targets are:
(a) whether the device has maple USB reset hardware.
(b) Which pin the "button" is attached to.

Note: Most "generic" STM32F103 boards only have a reset button, and not a user / test button. So the bootloader code always configures the Button input pin as PullDown. Hence, if a button is not present on the Button pin (Default is PC14), the pin should remain in a LOW state, and the bootloader will assume the Button is not being pressed.

IMPORTANT!
If your board has external hardware attached to pin PC12 and it pulls that pin HIGH, you will need to make a new build target for your board which uses a different pin for the Button, or change the code to make it ignore the Button.

The configuration for each build target / board is defined in config.h

The Makefile sets the board name as a #define e.g. #define TARGET_GENERIC_F103_PD2, and config.h contains a block of config defines for each board.

e.g.

```
#define HAS_MAPLE_HARDWARE 1
#define LED_BANK GPIOB
#define LED_PIN 1
#define LED_ON_STATE 1
/* On the Mini, BUT is PB8 */
#define BUTTON_BANK GPIOB
#define BUTTON 8
/* USB Disc Pin Setup. USB DISC is PB9 */
#define USB_DISC_BANK GPIOB
#define USB_DISC 9
#define USER_CODE_RAM ((u32)0x20000C00)
#define RAM_END ((u32)0x20005000)
```

Boards which have the Maple USB reset hardware need to defined HAS_MAPLE_HARDWARE

There are definitions for LED_BANK (GPIO Port)
LED pin (Pin in that port)
LED_ON_STATE (Whether the LED is lit when the pin is high (1) or low (0) - Note: this is because some generic boards have the LED between Vcc and the pin, hence are on when the LED PIN is LOW)

Button bank (port) and pin

For boards with Maple hardware, the "Disconnect" pin USB_DISC need to be defined.
On generic" boards, as this is always PA12 - as USB D+ is always on this pin on STM32F103 devices.
(Note: For generic boards, this define could probably be moved to the usb code as it never changes)

Note:
USER_CODE_RAM and RAM_END are allways the same, and probably need to be consolidated, rather than defined for each board.

There is no longer a need to configure FLASH SIZE, as it is detemined by reading the size from a register in the processor.
The Flash page (and hence DFU upload block size) no longer needs to be defined either, as the flash page size is inferred from the total flash size. All devices up to 128kb have a 1k page size, and boards with more than 128k of flash have a 2k page size.

Note the code directly links flash page size to DFU block size. It would probably be possible to decouple this and possibly increase upload speeds, but it has not been done yet.


#####Other improvements on the original Maple bootloader

1. Smaller footprint - now only 8k (This became possible due to code changes by @jonatanolofsson which allowed the GCC optimisation for size flag to be used
2. Additional DFU AltID upload type was added, which allows the sketch to be loaded at 0x8002000 instead of 0x8005000 (due to the reduced size of the bootloader itself),
Note: Upload to 0x8005000 was retained for backward compatibility.


If you have questions about the bootloader please raise an issue and I will attempt to answer them.




