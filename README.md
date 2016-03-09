#16F8xxA Bootloader Quick Start

*For more C sample code, see [www.MicrochipC.com](http://www.MicrochipC.com/).*

Once you have a bootloader set up for your PIC, you can download new .hex files in seconds. Most .hex files work with no modifications!

On average, users are up and running in about 10 minutes after following these instructions.

For full instructions, see [http://www.microchipc.com/PIC16bootload/](http://www.microchipc.com/PIC16bootload/).

> Do you have any improvements to share with the community? We honour 98% of GIT pull requests within a few days!

# Processors supported

- 16F870
- 16F871 (2k flash) 
- The 16F872 has no UART so it is not supported
- 16F873
- 16F874
- 16F73
- 16F74 (4k flash)
- 16F876
- 16F877
- 16F76
- 16F77 (8k flash)
- 16F87xA (select appropriate files).

# How to use this bootloader

This is a updated form of the "__readme.txt" text that describes how to use this bootloader. 

## Step 1: Connect to PC using RS232 serial port

- Connect the serial port of your computer to the PIC micro. The serial port operates at +/- 13V, and the PIC serial operates TTL levels of
+5V/0V.  Use a MAX232, MAX3222, or SIPEX232 level shifter to connect the serial
port of the computer to pins RX/TX on PIC.  For the schematic, see the .gif and
Protel 99 .sch files in this .zip file. The FAQ at [http://www.microchipc.com/](http://www.microchipc.com/) has the same schematic diagrams, plus a
troubleshooter guide for non-working serial ports.
- Select the correct .hex file, eg: "_61619 - bootldr-16F876-77-16Mhz-19200bps.hex". The .hex file above is 16F876 or 16F877, 16Mhz, downloading at 19200 baud. Crystal speed: 3.6864, 4, 16 and 20Mhz

## Step 2: Recompile your C code to avoid overwriting the bootloader

Recompile your C code so it doesnt overwrite the bootloader.

See the [website](http://www.microchipc.com/PIC16bootload/) - it has more complete instructions.

The bootloader uses 255 instructions at the top of flash.  For the 16F876/7, which has 8k of flash, this is flash locations 0x1F00 to 0x1FFF.  For the 16F873/4, which has 4k of flash, this is flash locations 0x0F00 to 0x0FFF.

- Hi-Tech C: Add -ICD to the PICC options. This reserves the top 256 bytes of the flash for the ICD (In-Circuit-Debugger), which means the bootloader works also.
- CCS C: see website.
- CC5X: see website.
- Assembly: Add an equivalent line reserving these program locations.

The latest version now works with a long or short jump in first 4 instructions of the .hex file.

## Step 3: Program PIC Micro

Program the target pic micro with this .hex file with a programmer such as
the PICStart Plus or P16PRO.

## Step 4: Bootload using host program on PC

Theres a choice of two Windows downloaders (one written in Delphi, one written in BC++) - pick  you favourite. The BC++ version has a built in terminal. Run "PICdownloader.exe", choose 19200bps, tick the "write eeprom" box, select the .hex file to download.  Again, get the crystal speed correct.  For 
example, for a 16Mhz crystal, choose "_19160 - test_serial_19200baud_16Mhz.hex".

Click 'write' and the windows program will begin searching for bootloader.

## Step 5: Reset PIC micro

Reset the PIC micro now.  The bootloader is active for 0.2 seconds after reset.

## Step 6: Observe output

The test program simply prints out a single text message @ 19200 baud on powerup.

In future, after powerup, bootloader times out in 0.2 sec and then runs the target program normally.

Close PICdownloader.exe (it reserves the serial port for itself), then run HyperTerminal, by clicking on 'hyperterminal_shortcut_COM2_19200bps-N81.ht'

Reset the PIC micro now, and you should see it pause for 0.2 sec, then start printing the test serial.

This is what it prints out:

> "Starting up serial @ 19200 baud, N,8,1, no flow control ..."
> (c)2003 Shane Tolmie, shane@microchipc.com, see http://www.microchipc.com/ for more projects like this and a huge FAQ on Hi-Tech C.

## More C Sample Code

For more sample code, see [www.MicrochipC.com](http://www.MicrochipC.com/).

> Do you have any enhancements to share with the community? We honour 95% of pull requests within a few days!