# Snek — a tiny python-inspired language for embedded computing

![Snek picture](snek.svg)

Snek is a tiny embeddable language targeting processors with only a
few kB of flash and ram. Think of something that would have been
running BASIC years ago and you'll have the idea. These processors are
too small to run [MicroPython](https://micropython.org/).

## Documentation

 * The [Snek Home Page](https://keithp.com/snek) provides more information
   about the language and community.

 * [Snek Manual in HTML format](https://keithp.com/snek/snek.html)

 * [Snek Manual in PDF format](https://keithp.com/snek/snek.pdf)

## Downloads

 * Snek is packaged for Debian but not yet in the main archive. I have
   made packages available in my personal archive for
   now. Instructions for using that can be found
   [here](http://keithp.com/archive/README).  This archive also
   includes _lola_, the LL parser generator used in building Snek.

 * [Windows package](http://keithp.com/snek/dist)

 * [General Linux package](http://keithp.com/snek/dist)

## To Do list

I think the basic Snek language is pretty complete at this point, but
there are always improvements that can be made.

## Recent Changes

Here's some places that have seen recent work

 * Documentation. A reference manual for the Snek language is getting
   more usable. This manual includes a tutorial section for learning
   the language.

 * Packaging. Debian, General Linux and Windows packages are
   available. These packages include menu entries and icons to run the
   host snek implementation and snekde in a terminal window. The Linux
   packages also include a shell script to install snek-duino onto an
   Arduino Duemilanova.

 * Dictionaries. Implemented using the list code, these can be indexed
   with any immutable type (everything other than lists and
   dictionaries). Making ROM space on Arduino for these required
   significant changes in lola to pack the parse tables even smaller.

## Build and Run!

### Dependencies
To build Snek you need these dependencies:

  * [Lola](https://keithp.com/cgit/lola.git/)
  * [gcc-avr](https://ccrma.stanford.edu/~juanig/articles/wiriavrlib/AVR_GCC.html)
  * [avr-libc](https://www.nongnu.org/avr-libc/)
  * [python3](https://www.python.org/)
  * [pyserial](https://github.com/pyserial/)
  * [python curses](https://docs.python.org/3/library/curses.html)

On Debian, you can get everything other than Lola from the main archive:

	# apt install gcc-avr avr-libc python3-serial

To install Lola, download the source and install it;

	$ git clone git://keithp.com/git/lola.git
	$ cd lola && make install

### Building and install

In the source of the project run:

	$ make
	$ make install

### Running on Linux

	$ snek

Then, just enjoy!

### Running on Arduino

Snek takes over the entire Arduino device, without leaving room for
the serial boot loader. Because of this, you will need an AVR programming device, such as a
[USBtiny from Adafruit](https://www.adafruit.com/product/46).

Once snek has been compiled, first reset the fuses on the device to
make the Arduino run Snek instead of attempting to run the
non-existent boot loader. You will only need to set the fuses once per device.

	$ (cd snek-duino && make set-fuse)

Next, flash the Snek interpreter:

	$ (cd snek-duino && make load)

Now, you can run the snek development environment:

	$ snekde

The snekde window is split into two parts. The upper 2/3 is a text
editor for source code. The bottom 1/3 lets you interact with the
Arduino over the serial port. The very top line lists functions that
you can invoke by pressing the associated function key:

 * F1 — Device. Connect to a serial port.
 * F2 — Get. Get source code saved to the Arduino eeprom into the editor pane.
 * F3 — Put. Put code from the editor pane into the Arduino eeprom.
 * F4 — Quit. Exit snekde.
 * F5 — Load. Read source code from the file system into the editor pane.
 * F6 — Save. Write source code from the editor pane to the file system.

There are a couple more keybindings which you'll want to know:

 * Page-up/Page-down — Switch between the editor pane and the interaction pane.
 * Ctrl-X/Ctrl-C/Ctrl-V — Usual cut/copy/paste commands.
 * Ctrl-C — In the interaction pane, this interrupts any snek program running on the device. Note that
   this means you don't get a Copy command in the interaction pane.
 * Ctrl-Z — Undo.

Tab auto-indents the current line. Backspace backs up over a tabstop
when appropriate.

### Examples

There are examples provided which work with both Python and Snek.

## Contributions

I'd love to receive more contributions in the form of patches,
suggestions and bug reports. Please feel free to send mail or use the
github process. I've created a mailing list for collaboration; 
you'll need to subscribe to be able to post. Subscribe to the [snek
mailing list here](https://keithp.com/mailman/listinfo/snek)
