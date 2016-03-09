* Linux implementation of a .hex file downloader

This file documents the installation and operation of a Linux
implementation of a PIC program downloader to match a bootloader for
a PIC.  See http://www.workingtechnologies.com/htpic/PIC_bootloader.htm
for documentation and the latest versions.

** Downloader Installation under Linux

(other *nixes should be similar)


Compilation/installtion Under GNU (Linux, Cygwin on Windows?, FreeBSD)
- ensure that the gnu libc6 *development* headers and libraries are installed
- ensure that gnu make is installed
- ensure that you have a working c compiler (e.g. gcc)
- invoke gnu make (Makefile is gmake specific for now, generic versions
  gladly accepted)
- picdl is the executable.  Copy it to some directory on your $PATH
- The default serial port is /dev/PICdown. 
	+ suggestion: (as root) create a symlink to your adaptor's 
	 serial port device file
		"# ln -s /dev/ttyS0 /dev/PICdown"
- make sure that the real device is readable and writable to the users who
  should be able to download. (perform the following as root)
	"# <addgroup> picdev"
	"# <adduserstogroup> picdev <your.uid> <her.uid>"
	"# chown root:picdev /dev/ttyS0"
	"# chmod g+rw /dev/ttyS0"

** Downloader & Bootloader Operations

Get the appropriate bootloader implementation from the above zipfile
onto the PIC using a "classical" programmer.
http://hyvatti.iki.fi/~jaakko/pic/picprog.html has a serial port
programmer that may work for you.  Some circuit construction will be necessary.

Now aquire or construct the RS-232 <-> TTL level shifter needed for serial communication with the 16F87x PICs

There are example .hex files in the .zipfile that are good tests of the
serial hardware.  Load them with the same programmer you used above,
or live high and wide and just try the downloader & bootloader out
immediately.  Be sure to use the appropriate file for the serial speed
and clock rate of your chip!

The command line invocation synopsis of the picdl program is:

	picdl [-S<baudrate>] [-P<rs232devfile] [-DEBUG_MODE={1,2}] filename.hex

where
	the default baud rate is 19200
	the default RS-232 device file is /dev/PICdown
	and -DEBUG_MODE=1 prints out less info than -DEBUG_MODE=2

Example invocation for 9600bps serial port /dev/ttyS1:

	picdl -S9600 -P/dev/ttyS1 file.hex

To use the downloader, type "picdl filename.hex" hit return, press the reset button when prompted, and wait for the download to finish and your program to be started.

Here is a transcript of downloading one of the sample .hex files:

	albertd@scout:~/robodev/LinuxPICDownloader$ picdl "PIC bootloader/hex files for testing serial comms on PIC/_19040 - test_serial_19200baud_4Mhz.hex"
	HEX FILE DOWNLOADER 1.20 FOR PIC16F87x
	Copyright (C) 1999 HI-TECH Software
	Modified to work with 'http://www.workingex.com/htpic' bootloader

	Looking for bootloader - ............ - OK
	Writing: .............................................................................................. - OK
	Activating program... - OK
	albertd@scout:~/robodev/LinuxPICDownloader$

NOTE: the tester programs provided in the zipfile accidentally
spoofs the bootloader by returning exactly what the bootloader would
during the identification phase.  The workaround is to hold the PIC
in reset until the downloader has been invoked on the host.

See the bootloader documentation for the program restrictions {size,
reset behavior} that the bootloader introduces to your program.

Strongly suggested: If you have any trouble, verify that
your hardware works with the Windows GUI implementation
that is available in the same zipfile as this *nix one at
http://www.workingtechnologies.com/htpic/PIC_bootloader.htm.  

** Projects for the ambitious

+ package this and the .hex files in (LSB?) binary package for conveinence's
  sake
+ write a proper man page.

	Albert den Haan
	albert.denhaan@sympatico.ca
