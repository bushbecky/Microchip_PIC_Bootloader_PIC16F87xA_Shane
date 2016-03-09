16F8xx Bootloader Quick Start

Once you have a bootloader set up for your PIC, you can download new .hex files
in seconds. Most .hex files work with no modifications!

On average, users are up and running in about 10 minutes after following these
instructions.

http://www.microchipc.com/
 - then follow link to bootloader.

**************************
How to use this bootloader

1.  First, connect the serial port of your computer to the PIC micro.

The serial port operates at +/- 13V, and the PIC serial operates TTL levels of
+5V/0V.  Use a MAX232, MAX3222, or SIPEX232 level shifter to connect the serial
port of the computer to pins RX/TX on PIC.  For the schematic, see the .gif and
Protel 99 .sch files in this .zip file.

The FAQ at http://www.microchipc.com/ has the same schematic diagrams, plus a
troubleshooter guide for non-working serial ports.

2.  Select the correct .hex file.

eg: "_61619 - bootldr-16F876-77-16Mhz-19200bps.hex"

The .hex file above is 16F876 or 16F877, 16Mhz, downloading at 19200 baud.

Crystal speed: 3.6864, 4, 16 and 20Mhz

Processors supported:

16F870,16F871 (2k flash) (the 16F872 has no UART so it is not supported)
16F873,16F874,16F73,16F74 (4k flash)
16F876,16F877,16F76,16F77 (8k flash)

16F87xA (select appropriate files).

3.  Recompile your code so it doesnt overwrite the bootloader.

See the website - it has more complete instructions.

The bootloader uses 255 instructions at the top of flash.  For the 16F876/7,
which has 8k of ROM, this is ROM locations 0x1F00 to 0x1FFF.  For the 16F873/4,
which has 4k of ROM, this is ROM locations 0x0F00 to 0x0FFF.

Hi-Tech C:
  Add -ICD to the PICC options. This reserves the top 256 bytes of the ROM for
  the ICD (In-Circuit-Debugger), which means the bootloader works also.

CCS C:
  see website

CC5X:
  see website

Assembly:
  Add an equivalent line reserving these program locations.

4.  The latest version now works with a long or short jump in first 4
instructions of the .hex file.

5.  Program the target pic micro with this .hex file with a programmer such as
the PICStart Plus or P16PRO.

6.  Theres a choice of two Windows downloaders (one written in Delphi, one 
written in BC++) - pick  you favourite. The BC++ version has a built in 
terminal. Run "PICdownloader.exe", choose 19200bps, tick the "write eeprom" box, 
select the .hex file to download.  Again, get the crystal speed correct.  For 
example, for a 16Mhz crystal, choose "_19160 - test_serial_19200baud_16Mhz.hex".

Click 'write' and the windows program will begin searching for bootloader.

7.  Reset the PIC micro now.  The bootloader is active for 0.2 seconds after
reset.

8.  The test program simply prints out a single text message @ 19200 baud on
powerup.

9.  In future, after powerup, bootloader times out in 0.2 sec and then runs the
target program normally.

10. Close PICdownloader.exe (it reserves the serial port for itself), then run
HyperTerminal, by clicking on 'hyperterminal_shortcut_COM2_19200bps-N81.ht'

11. Reset the PIC micro now, and you should see it pause for 0.2 sec, then start
printing the test serial.

This is what it prints out:

"PICTest (c)2001 Shane Tolmie - see http://www.workingtex.com/htpic"
"Starting up serial @ 19200 baud, N,8,1, no flow control ..."

(c)2003 Shane Tolmie, shane@microchipc.com, see http://www.microchipc.com/
for more projects like this and a huge FAQ on Hi-Tech C.

