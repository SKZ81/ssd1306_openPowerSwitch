# SSD1306 driver - dedicated to openPowerSwitch

## Description

**openPowerSwitch** is an application that will use most of the RAM available on an atmega328p.

Also it will eat most of flash space, leave few Kbytes available.

It is then impossible to use the standard adafruit driver + GFX lib. Also it is not possible to reserve half the RAM for a framebuffer.
Anyway, this application doesn't need advanced graphic functions. What is needed is :
- to be able to provide some information about the network link (mainly the configured IP when using DHCP)
- display the outlet (main power plugs) state, ON or OFF
- if possible, display a nice logo on startup

To do that, no framebuffer is needed : only storing a font, few icons and the logo in flash space, and stream them using the I²C link to the graphic RAM (GDDRAM) of the **SSD1306**.



## Build

I use [arduino-makefile](https://github.com/sudar/Arduino-Makefile) to build and upload the example sketches.
To have it working, rename or symlink this repo as `ssd1306.ops` in your own `$USER_LIB_PATH` (you will have to change it in the makefile).

Also some examples are depending on **avr_uart** lib, which is very simple : just get the `avr_uart.c` and `avr_uart.h` files (for instance [here](https://github.com/SKZ81/avr_misc_sketches/tree/DHT22)) and copy/move them in `$USER_LIB_PATH/avr_uart` : that will do the job nicely.

Adapt the rest of the **Makefile** according to your setup and you'll be able to `make && make upload`


## xbm2ssd1306

The "problem" addressed here is the following : each byte you send to the ssd1306 is displayed in a given column of 8 rows.
So the data need to be re-formatted given this constraint.

This utility allows to create a graphical design in a graphic software (for instance, **kolourpaint** ; just remember to keep it Black&White !)
As long as it is provided in **XBM** format, the actual source doesn't matter...

then call this little utility (written in python) :
`$./xbm2ssd1306 design.xbm source.c`

the `#define`s that are outputted are optional, just as reminders of the image format. You will probably want to rename the array (for now, the utility lacks an option parser, I probably won't do it).

Define an `external` reference to this array, or copy it into your main code, and here you are !

(**NOTE** : actually not exactly, for now... You'll have to write a custom function to actually send it to the display, but see the `openPowerSwitch_logo()` for an example)

### Have fun !!

![ssd1306 display with openPowerSwitch logo](https://github.com/SKZ81/ssd1306_openPowerSwitch/blob/master/images/ssd1306_ops_logo.jpg "openPowerSwitch logo on ssd1306 display")

![ssd1306 display leds to show outlet status](https://github.com/SKZ81/ssd1306_openPowerSwitch/blob/master/images/outlet_status.jpg "openPowerSwitch outlet status display")

