**Note:** this code does not work with all STM32F103 Boards.

*Note:** not all GCC versions work. Reported gcc-arm-none-eabi versions that work are: 4.8, 5.4, 6.0, 7.2. YMMV.

This is a source of bootloader for STM32F103 boards, for use with the Arduino_STM32 repo and the Arduino IDE. It allows a board to be programmed directly via USB, eliminating the need for a separate programmer hardware.

## Changes

This repo is a fork of [@jonatanolofsson's maple-bootloader](https://github.com/jonatanolofsson/maple-bootloader) (mini-boot branch), which in turn is a fork of the [maple-bootloader created by Leaflabs](http://github.com/leaflabs/maple-bootloader).

The bootloader has been reworked so that all versions are contained in the Master branch, rather than 1 branch per version / board.

The upload to RAM option has effectively been removed, as it was useless and caused problems.
The bootloader still has a DFU - AltID for RAM uploads, however this returns an error the host attempts to upload to this AltID. The AltID was kept to allow backwards compatibility with anyone still using the old Maple IDE which has a upload to RAM option.

The makefile now has multple build targets including `maple-mini` (roughly equivalent to the old mini-boot branch), `maple-rev3` etc.

## Generic USB reset support

Additionally the bootloader now works with "generic" STM32F103 boards, which do not have the additional USB reset hardware which all Maple and Maple mini boards have.

On "generic" boards, the USB reset (to force re-enumeration by the host) is triggered by reconfiguring USB line D+ (PA12) into GPIO mode, and driving PA12 low for a short period, before setting the pin back to its USB operational mode.
This system to reset the USB was written by [@victorpv](https://github.com/victorpv).

It is not guaranteed to work on all "generic" STM32 boards, and relies on having a 1.5K pull-up resistor between Vcc and USB D+ (which is wired to PC12), however most "generic" boards seem to have this. Some boards have a 10K or 4.7K resistor instead, so a hardware modification might be required.

It is unclear if this method to reset the USB bus conforms precisely to the USB standard, but it seems to work fine on all PC, Mac, and Linux machines on which its been tested, and seems to be usable for hobby / non commericial / non-critical systems.

## Build targets for generic boards

There are multiple build targets for generic STM32F103-based boards, because each vendor seems to have the LED on a different port/pin, and the bootloader blinks the LED to indicate the current operation / state.

So if your board has an LED on pin PC13, the makefile target is `generic-pc13`. At the time of writing the following generic targets are available (more may be added without this readme being updated each time, so please check the makefile to see the latest list of build targets):

* generic-pc13
* generic-pg15
* generic-pd2
* generic-pd1
* generic-pa1
* generic-pb9

In addition to the LED, the differences in the build targets are:
1. whether the device has Maple USB reset hardware;
2. which pin the "button" is attached to.

## Second button support

Most "generic" STM32F103 boards only have one button -- a reset one, and lack a second user/test button. The second button is also used by the bootloader to enter the "permanent bootloader" state, and for generic boards its pin is set to `PC14` in config.h. The bootloader code configures a the button input pin as pull-down, hence if a button is not physically present on that pin (`PC14`), the pin should remain in a LOW state, and the bootloader will assume that the button is not being pressed.

**IMPORTANT:** If you have a board where you have external hardware attached to pin `PC14` which will pull this pin HIGH, you will need to make a new build target for you board to either use a different pin for the button, or omit the `BUTTON_*` defines altogether. In the latter case the bootloader won't do anything special about the second button.


## Targets and configuration

The configuration for each build target / board is defined in `config.h`.

The Makefile sets the board name as a define, e.g. `#define TARGET_GENERIC_F103_PD2`, and `config.h` contains a block of config defines for each board.

For example, for the Maple Mini board:

```c
#define HAS_MAPLE_HARDWARE 1
#define LED_BANK GPIOB
#define LED_PIN 1
#define LED_ON_STATE 1
/* On the Mini, BUT is PB8 */
#define BUTTON_BANK GPIOB
#define BUTTON_PIN 8
/* USB Disc Pin Setup. USB DISC is PB9 */
#define USB_DISC_BANK GPIOB
#define USB_DISC_PIN 9
```

Let's go over the above.

1. Boards which have the Maple USB reset hardware need to define `HAS_MAPLE_HARDWARE`.

2. In config.h there are per-board definitions for:
* `LED_BANK`: GPIO Port (e.g. `GPIOA`, `GPIOB`, etc.)
* `LED_PIN`: pin in that port (e.g. `1`)
* `LED_ON_STATE`: whether the LED is lit when the pin is high (1) or low (0). This is because some generic boards have the LED between `Vcc` and the pin, hence are on when the `LED_PIN` is LOW.

3. Boards with Maple hardware have an extra button (in addition to "Reset") and a special USB reset/disconnect hardware circuit. Such boards need a few additional defines, namely:
** `BUTTON_BANK` and `BUTTON_PIN` for the extra button;
** `USB_DISC_BANK` and `USB_DISC_PIN` for the USB disconnect circuit.
For generic boards (ones that don't have `#define HAS_MAPLE_HARDWARE`), there is no need to define all this.

## Other improvements on the original Maple bootloader

1. Smaller footprint, now only 8k (this became possible due to code changes by [@jonatanolofsson](https://github.com/jonatanolofsson) which allowed the GCC optimisation for size flag to be used.

2. Additional DFU AltID upload type was added, which allows the sketch to be loaded at 0x8002000 instead of 0x8005000 (due to reduced size of the bootloader itself). **Note** upload to 0x8005000 was retained for backwards compatibility.

## Feedback

If you have questions about the bootloader, please raise an issue and I will attempt to answer it.
